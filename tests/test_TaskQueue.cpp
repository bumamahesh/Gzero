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
  std::cout << "starting task" << std::endl;
  usleep(30 * 1000); // wait for 30 ms
  std::cout << "Completed task" << std::endl;
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