#include "TaskQueue.h"
#include <gtest/gtest.h>
#include <string>

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
