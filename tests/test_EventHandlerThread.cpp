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
#include "EventHandlerThread.h"

typedef struct AlgoEvent {
  void* arg = nullptr;
  AlgoEvent(void* arg = nullptr) : arg(arg) {}
} AlgoEvent;

TEST(EventHandlerThreadTest, BasicTest) {
  std::atomic<int> counter(0);

  // Use a condition variable to wait for all events to be processed
  std::mutex mtx;
  std::condition_variable cv;
  bool done = false;

  auto handler = [&](void* context, std::shared_ptr<AlgoEvent>) {
    (void)context;
    counter++;
    {
      std::lock_guard<std::mutex> lock(mtx);
      if (counter == 10) {  // If we processed 10 events, notify the main thread
        done = true;
        cv.notify_one();
      }
    }
  };

  EventHandlerThread<AlgoEvent> thread(handler, nullptr);

  // Push 10 events into the queue
  for (int i = 0; i < 10; ++i) {
    std::shared_ptr<AlgoEvent> event = std::make_shared<AlgoEvent>(nullptr);
    thread.SetEvent(event);
  }

  // Wait for the condition variable to notify that all events have been
  // processed
  {
    std::unique_lock<std::mutex> lock(mtx);
    cv.wait(lock, [&done] { return done; });
  }

  // Stop the thread and assert the counter
  thread.stop();
  ASSERT_EQ(counter.load(), 10);
}