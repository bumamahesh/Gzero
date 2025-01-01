
#include "../include/TaskQueue.h"
#include <gtest/gtest.h>
#include <memory>
#include <thread>

// Test TaskQueue Construction
TEST(TaskQueueTest, ConstructorInitializesCorrectly) {
  TaskQueue taskQueue;
  // Ensure the object is constructed without any issues
  ASSERT_TRUE(true); // Placeholder, no specific conditions to test here
}

// Test Enqueue and DeQueue functionality
TEST(TaskQueueTest, EnqueueIntTask) {

  auto callback = [](std::shared_ptr<Task_t> task) {
    int *args = reinterpret_cast<int *>(task->args);
    ASSERT_EQ(*args, 42);
    delete args;
  };
  TaskQueue taskQueue(callback);

  // Create a sample task
  auto task = std::make_shared<Task_t>();
  task->args = reinterpret_cast<void *>(new int(42));

  // Enqueue the task
  taskQueue.Enqueue(task);

  taskQueue.WaitForQueueCompetion();
}

// Test DeQueue on an empty queue (expect it to block or wait)
TEST(TaskQueueTest, DeQueueOnEmptyQueue) {
  TaskQueue taskQueue;

  // Start a separate thread to test DeQueue behavior
  std::thread dequeueThread([&taskQueue]() {
    auto task = taskQueue.DeQueue();
    ASSERT_EQ(task, nullptr); // Expect nullptr or other appropriate behavior
  });

  // Give some time for DeQueue to block
  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  // Join the thread
  dequeueThread.join();
}

// Test Destructor behavior (ensuring cleanup)
TEST(TaskQueueTest, DestructorCleansUp) {
  {
    TaskQueue taskQueue;

    auto task = std::make_shared<Task_t>();
    task->args = reinterpret_cast<void *>(42);

    taskQueue.Enqueue(task);
  }
  // After exiting the scope, the destructor should have been called.
  ASSERT_TRUE(true); // No actual verification; just ensuring no crashes.
}

// Stress test constants

int totalcallback = 0;
// Test Destructor behavior (ensuring cleanup)
TEST(TaskQueueTest, CallabckTest) {
  {

    auto callback = [](std::shared_ptr<Task_t> task) {
      int *args = reinterpret_cast<int *>(task->args);
      // std::cout << "Callback called with args: " << *args << std::endl;
      delete args;
      totalcallback++;
    };
    totalcallback = 0;
    TaskQueue taskQueue(callback);

    for (int i = 0; i < 100000; i++) {
      auto task = std::make_shared<Task_t>();
      task->args = reinterpret_cast<void *>(new int(i));

      taskQueue.Enqueue(task);
    }
    /* while (totalcallback < 100) {
       std::this_thread::sleep_for(std::chrono::milliseconds(100));
     }*/
    taskQueue.WaitForQueueCompetion();
    ASSERT_EQ(totalcallback, 100000);
  }
  // After exiting the scope, the destructor should have been called.
  ASSERT_TRUE(true); // No actual verification; just ensuring no crashes.
}

/**
 * @brief Main function to execute all tests.
 * @param argc Argument count.
 * @param argv Argument vector.
 * @return Test execution result.
 */
int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
