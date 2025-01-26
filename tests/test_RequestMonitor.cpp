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
#include "../include/RequestMonitor.h"
#include "Task.h"
#include <future>
#include <gtest/gtest.h>
#include <thread>

class RequestMonitorTest : public ::testing::Test {
protected:
  // This will be run before each test
  void SetUp() override {
    // Set up necessary test data, if needed
    for (int i = 0; i < 10; i++) {
      auto task = std::make_shared<Task_t>();
      task->request = std::make_shared<AlgoRequest>();
      task->request->mRequestId = 1;
      taskVector.push_back(task);
    }
  }

  // This will be run after each test
  void TearDown() override {
    // Clean up if needed
  }

public:
  bool callbackCalled = false;
  std::vector<std::shared_ptr<Task_t>> taskVector;
};
void mockCallback(void *pContext, std::shared_ptr<Task_t> task) {

  RequestMonitorTest *test = static_cast<RequestMonitorTest *>(pContext);
  test->callbackCalled = true;
}

// Test case for starting and stopping a request
TEST_F(RequestMonitorTest, StartStopRequest) {
  RequestMonitor monitor;
  monitor.SetCallback(mockCallback, this);
  // Start a request with ID 1 and a timeout of 3 seconds
  monitor.StartRequestMonitoring(taskVector[0], 3000);

  // Stop the request
  monitor.StopRequestMonitoring(taskVector[0]);

  // Check that the callback has not been triggered, because the request should
  // stop normally
  EXPECT_FALSE(callbackCalled);
}

// Test case for timeout exceeding tolerance and triggering the callback
TEST_F(RequestMonitorTest, TimeoutExceedsTolerance) {
  RequestMonitor monitor;
  monitor.SetCallback(mockCallback, this);
  // Start a request with ID 1 and a timeout of 2 seconds
  monitor.StartRequestMonitoring(taskVector[0], 1000);

  // Sleep for 3 seconds to simulate timeout
  std::this_thread::sleep_for(std::chrono::seconds(3));

  // Since 3 seconds exceeds the tolerance of 500ms, callback should be
  // triggered
  monitor.StopRequestMonitoring(taskVector[0]);

  // Check that the callback was triggered
  EXPECT_TRUE(callbackCalled);
}

// Test case for request completing within tolerance
TEST_F(RequestMonitorTest, RequestCompleteWithinTolerance) {
  RequestMonitor monitor;
  monitor.SetCallback(mockCallback, this);
  // Start a request with ID 1 and a timeout of 3 seconds
  monitor.StartRequestMonitoring(taskVector[0], 3000);

  // Sleep for 1 second to simulate a request completing within the tolerance
  std::this_thread::sleep_for(std::chrono::seconds(1));

  // Stop the request
  monitor.StopRequestMonitoring(taskVector[0]);

  // Ensure the callback is not called because the request completed within
  // tolerance
  EXPECT_FALSE(callbackCalled);
}

// Test case for a request that exceeds timeout (no stopRequest)
TEST_F(RequestMonitorTest, RequestExceedsTimeoutWithoutStop) {
  RequestMonitor monitor;
  monitor.SetCallback(mockCallback, this);
  // Start a request with ID 1 and a timeout of 2 seconds
  monitor.StartRequestMonitoring(taskVector[0], 2000);

  // Sleep for 3 seconds to simulate a timeout
  std::this_thread::sleep_for(std::chrono::seconds(3));

  // The monitor should call the callback after the timeout, even if stopRequest
  // was not called The callback is triggered by the monitor thread, not the
  // stopRequest method
  std::this_thread::sleep_for(
      std::chrono::seconds(1)); // Ensure callback has been processed

  EXPECT_TRUE(callbackCalled);
}

// Test case for multiple requests (check behavior for several requests at once)
TEST_F(RequestMonitorTest, MultipleRequests) {
  RequestMonitor monitor;
  monitor.SetCallback(mockCallback, this);
  // Start requests with different timeouts
  monitor.StartRequestMonitoring(taskVector[0], 3000);
  monitor.StartRequestMonitoring(taskVector[1], 4000);

  // Sleep for 5 seconds to allow both requests to be monitored
  std::this_thread::sleep_for(std::chrono::seconds(2));

  // Stop requests after the timeout
  monitor.StopRequestMonitoring(taskVector[0]);
  monitor.StopRequestMonitoring(taskVector[1]);

  // Ensure that both requests did not trigger the callback (they were completed
  // within tolerance)
  EXPECT_FALSE(callbackCalled);
}

// Test case for checking timeout on a request that has not been stopped
TEST_F(RequestMonitorTest, RequestTimeoutNoStop) {
  RequestMonitor monitor;

  monitor.SetCallback(mockCallback, this);

  // Start a request with ID 1 and a timeout of 3 seconds
  monitor.StartRequestMonitoring(taskVector[0], 3000);

  // Sleep for 4 seconds to simulate timeout
  std::this_thread::sleep_for(std::chrono::seconds(4));

  // The callback should be triggered because the request was not stopped
  std::this_thread::sleep_for(
      std::chrono::seconds(1)); // Allow callback processing

  EXPECT_TRUE(callbackCalled);
}

//-----------------------------------------------------------
int callbackCounter = 0;
void Callback(void *pContext, std::shared_ptr<Task_t> task) {
  callbackCounter++;
}

class RequestMonitorStressTest : public ::testing::Test {
protected:
  void SetUp() override {
    callbackCounter = 0;
    monitor = new RequestMonitor();
    monitor->SetCallback(Callback, this);

    for (int i = 0; i < 10; i++) {
      auto task = std::make_shared<Task_t>();
      task->request = std::make_shared<AlgoRequest>();
      task->request->mRequestId = 1;
      taskVector.push_back(task);
    }
  }

  void TearDown() override { delete monitor; }

  RequestMonitor *monitor = nullptr;
  std::vector<std::shared_ptr<Task_t>> taskVector;
};

TEST_F(RequestMonitorStressTest, NoCallbackForProcessingWithinTolerance) {
  const int timeout = 33;                  // 33ms timeout
  const int tolerance = 3;                 // 3ms tolerance
  const int minTime = timeout - tolerance; // 30ms
  const int maxTime = timeout + tolerance; // 36ms
  const int totalFrames = 10;

  // Vector to store futures for asynchronous tasks
  std::vector<std::future<void>> tasks;

  // Add 1000 frames with random processing times asynchronously
  for (int i = 0; i < totalFrames; ++i) {
    monitor->StartRequestMonitoring(taskVector[i], maxTime);
    tasks.emplace_back(std::async(std::launch::async, [i, this, minTime,
                                                       maxTime]() {
      // Simulate processing time between 30ms and 36ms
      int processingTime = minTime + (std::rand() % (maxTime - minTime + 1));
      std::this_thread::sleep_for(std::chrono::milliseconds(processingTime));
      monitor->StopRequestMonitoring(taskVector[i]);
    }));
  }

  // Wait for all tasks to complete
  for (auto &task : tasks) {
    task.get();
  }

  // Wait for monitor to stabilize
  std::this_thread::sleep_for(std::chrono::seconds(2));

  // Assert that no callback was triggered
  ASSERT_EQ(callbackCounter, 0) << "Monitor triggered callbacks even though "
                                   "processing was within tolerance!";
}