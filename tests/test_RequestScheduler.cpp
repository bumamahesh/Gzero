/*
 * Copyright (c) [2025] [Uma Mahesh B]
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#include <gtest/gtest.h>
#include <atomic>
#include <chrono>
#include <fstream>
#include <iostream>
#include <numeric>  // Required for std::accumulate
#include <thread>
#include <vector>
#include "../Utils/include/RequestScheduler.h"

TEST(RequestSchedulerTest, BasicFunctionality) {
  std::atomic<int> counter(0);
  auto callback = [&](int request) {
    (void)request;
    /*sleep for  some load*/
    std::this_thread::sleep_for(std::chrono::milliseconds(1000 / 66));
    counter++;
  };
  int fps         = 60;
  int numRequests = fps * 2;  // make sure we have request twice  the fps
  RequestScheduler<int> scheduler(fps, callback);
  int tolerance = fps / 10;  // Allow for a tolerance of 10 % fps
  if (tolerance == 0) {
    tolerance = 1;
  }

  int lowerBound = fps - tolerance;
  int upperBound = fps + tolerance;

  scheduler.enableJitter(true);

  for (int i = 0; i < numRequests; ++i) {
    scheduler.queueRequest(i);
  }

  std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  // std::cerr << counter.load() << " " << lowerBound << " " << upperBound
  //           << std::endl;
  ASSERT_TRUE(counter.load() >= lowerBound && counter.load() <= upperBound);
}

TEST(RequestSchedulerTest, NoJitter) {
  std::atomic<int> counter(0);
  auto callback = [&](int request) {
    (void)request;
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    counter++;
  };
  RequestScheduler<int> scheduler(0, callback, false);
  scheduler.enableJitter(false);
  for (int i = 0; i < 50; ++i) {
    scheduler.queueRequest(i);
  }
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  /*after 100 ms we are expecting 50 calls as jitter is disabled*/
  ASSERT_TRUE(counter.load() == 50);
}

TEST(RequestSchedulerTest, EnableDisableJitter) {
  std::atomic<int> counter(0);
  auto callback = [&](int request) {
    (void)request;
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    counter++;
  };
  RequestScheduler<int> scheduler(0, callback, true);

  // Initially, jitter is enabled
  for (int i = 0; i < 50; ++i) {
    scheduler.queueRequest(i);
  }
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  /*after 100 ms we are expecting less then 50 calls as jitter is enabled*/
  ASSERT_TRUE(counter.load() < 50);

  // Disable jitter
  scheduler.enableJitter(false);
  for (int i = 0; i < 50; ++i) {
    scheduler.queueRequest(i);
  }
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  /*after 100 ms we are expecting 50 calls as jitter is disabled*/
  ASSERT_TRUE(counter.load() == 100);

  // Enable jitter again
  counter = 0;
  scheduler.enableJitter(true);
  for (int i = 0; i < 50; ++i) {
    scheduler.queueRequest(i);
  }
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  /*after 100 ms we are expecting less then 50 calls as jitter is enabled*/
  ASSERT_TRUE(counter.load() < 50);
}

// Structure to store trace events
struct TraceEvent {
  std::string name;
  std::string ph;
  uint64_t ts;
  std::string pid;
  std::string tid;

  TraceEvent(std::string event_name, std::string phase, uint64_t timestamp,
             std::string process, std::string thread)
      : name(std::move(event_name)),
        ph(std::move(phase)),
        ts(timestamp),
        pid(std::move(process)),
        tid(std::move(thread)) {}
};

// Convert trace events to JSON format for Chrome Tracing
std::string generateTraceJson(const std::vector<TraceEvent>& traceEvents) {
  std::stringstream ss;
  ss << "{ \"traceEvents\": [\n";
  for (size_t i = 0; i < traceEvents.size(); ++i) {
    ss << "  { \"name\": \"" << traceEvents[i].name << "\", \"ph\": \""
       << traceEvents[i].ph << "\", \"ts\": " << traceEvents[i].ts
       << ", \"pid\": \"" << traceEvents[i].pid << "\", \"tid\": \""
       << traceEvents[i].tid << "\" }";
    if (i != traceEvents.size() - 1) {
      ss << ",";
    }
    ss << "\n";
  }
  ss << "] }\n";
  return ss.str();
}

TEST(RequestSchedulerTest, GenerateCallbackTrace) {
  constexpr int fps              = 30;    // Target FPS
  constexpr int test_duration_ms = 1000;  // Run test for 1000ms

  std::vector<TraceEvent> traceEvents;
  std::atomic<int> callbackCount(0);
  std::vector<uint64_t> timestamps;  // Store execution timestamps
  auto start_time = std::chrono::steady_clock::now();

  std::mutex traceMutex;  // 🔹 Ensure thread-safe access to traceEvents

  auto callback = [&](int request) {
    (void)request;
    auto now = std::chrono::steady_clock::now();
    uint64_t timestamp_us =
        std::chrono::duration_cast<std::chrono::microseconds>(now - start_time)
            .count();

    {  // Lock mutex before modifying traceEvents
      std::lock_guard<std::mutex> lock(traceMutex);
      traceEvents.emplace_back("Callback Execution", "B", timestamp_us,
                               "Scheduler", "CallbackThread");
    }

    std::this_thread::sleep_for(
        std::chrono::milliseconds(10));  // Simulated processing delay

    auto end_time = std::chrono::steady_clock::now();
    uint64_t end_timestamp_us =
        std::chrono::duration_cast<std::chrono::microseconds>(end_time -
                                                              start_time)
            .count();

    {  // Lock mutex before modifying traceEvents
      std::lock_guard<std::mutex> lock(traceMutex);
      traceEvents.emplace_back("Callback Execution", "E", end_timestamp_us,
                               "Scheduler", "CallbackThread");
    }

    timestamps.push_back(timestamp_us);
    callbackCount++;
  };

  {
    // lets create and destoy in frame ...
    RequestScheduler<int> scheduler(fps, callback, true);

    // Push a large number of requests
    for (int i = 0; i < 100; ++i) {
      scheduler.queueRequest(i);
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(test_duration_ms));
    // the movement  we disable all q is called in disable call so do not do that
    // scheduler.enableJitter(false);
  }

  // Save trace for debugging
  std::ofstream traceFile("trace.json");  // trace file
  traceFile << generateTraceJson(traceEvents);
  traceFile.close();
  std::cout << "Trace file generated: trace.json (Open in chrome://tracing/)\n";

  // **Recalculate Expected Calls based on actual timestamps**
  int actual_calls       = callbackCount.load();
  double avg_interval_us = 0.0;

  if (timestamps.size() > 1) {
    std::vector<uint64_t> intervals;
    for (size_t i = 1; i < timestamps.size(); ++i) {
      intervals.push_back(timestamps[i] - timestamps[i - 1]);
    }

    avg_interval_us = std::accumulate(intervals.begin(), intervals.end(), 0.0) /
                      intervals.size();
    std::cout << "Average callback interval: " << avg_interval_us / 1000.0
              << " ms"
              << "fps::" << 1000000.0 / avg_interval_us << "\n";
  }

  // Adjust expected range dynamically
  int min_expected_calls =
      (fps * test_duration_ms * 0.8) / 1000;  // Allow 20% margin
  int max_expected_calls = (fps * test_duration_ms * 1.2) / 1000;
  if (max_expected_calls == 0)
    max_expected_calls = 1;
  std::cout << " min_expected_calls::" << min_expected_calls
            << " actual_calls::" << actual_calls
            << " max_expected_calls::" << max_expected_calls << std::endl;
  EXPECT_GE(actual_calls, min_expected_calls);
  EXPECT_LE(actual_calls, max_expected_calls);
}
