#ifndef TASK_QUEUE_H
#define TASK_QUEUE_H
#pragma once
#include <atomic>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <pthread.h>
#include <queue>

typedef struct Task_t {
  void *args;
} Task_t;

class TaskQueue {
public:
  // Constructor
  TaskQueue(void (*callbacks)(std::shared_ptr<Task_t> task) = nullptr);

  // Destructor
  ~TaskQueue();

  // Set the thread name
  void SetThread(const std::string &name);

  // Enqueue a payload
  void Enqueue(std::shared_ptr<Task_t> payload);

  // Dequeue a payload
  std::shared_ptr<Task_t> DeQueue();

  // Callback function
  void (*pCallback)(std::shared_ptr<Task_t> task) = nullptr;

  // Wait for queue to complete
  void WaitForQueueCompetion();

private:
  // Internal worker thread function
  static void *WorkerThreadFuction(void *arg);

  // Function to execute tasks
  void ExecuteTask(std::shared_ptr<Task_t>);

  // Member variables
  std::queue<std::shared_ptr<Task_t>> taskQueue; // Queue to hold tasks
  pthread_t workerThread;                        // Thread handle
  std::atomic<bool> isRunning; // Atomic flag to check if the thread should run
  std::mutex queueMutex;       // Mutex for accessing taskQueue
  std::condition_variable
      conditionVar; // Condition variable for synchronization
};

#endif // TASK_QUEUE_H
