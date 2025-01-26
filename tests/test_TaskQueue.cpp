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
#include "TaskQueue.h"
#include <gtest/gtest.h>

#define STRESS_CNT 100
size_t gCallback = 0;

auto funcTask = [](void *ctx, std::shared_ptr<Task_t> task) { usleep(10000); };
auto funcCb = [](void *ctx, std::shared_ptr<Task_t> task) { gCallback++; };

TEST(TaskQueueTest, WaitForQueueCompletion) {
  gCallback = 0;
  TaskQueue queue(funcTask, funcCb, this);

  for (int i = 0; i < STRESS_CNT; i++) {
    auto task = std::make_shared<Task_t>();
    queue.Enqueue(task);
  }
  queue.WaitForQueueCompetion();
  EXPECT_EQ(gCallback, STRESS_CNT);
}

TEST(TaskQueueTest, StopWorkerThread) {
  gCallback = 0;
  TaskQueue queue(funcTask, funcCb, this);

  for (int i = 0; i < STRESS_CNT; i++) {
    auto task = std::make_shared<Task_t>();
    queue.Enqueue(task);
  }
  queue.StopWorkerThread();
}

/**test timout callback with long processing time and short timeout register */
auto longTask = [](void *ctx, std::shared_ptr<Task_t> task) {
  usleep(30 * 1000); // wait for 30 ms
};
bool bTimeoutCallback = false;
auto timeoutCallback = [](void *, std::shared_ptr<Task_t> task) {
  bTimeoutCallback = true;
};

TEST(TaskQueueTest, TestTimeoutCallback) {
  bTimeoutCallback = false;
  TaskQueue queue(longTask, funcCb, this);
  queue.monitor->SetCallback(timeoutCallback, this);
  // prepare a request
  auto task = std::make_shared<Task_t>();
  task->timeoutMs = 10;
  task->request = std::make_shared<AlgoRequest>();
  task->request->mRequestId = 1;
  queue.Enqueue(task);
  queue.WaitForQueueCompetion();
  EXPECT_EQ(bTimeoutCallback, true);
}