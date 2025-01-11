#ifndef TASK_QUEUE_H
#define TASK_QUEUE_H
#pragma once
#include "AlgoRequest.h"
#include <atomic>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <pthread.h>
#include <queue>

typedef struct Task_t {
  std::shared_ptr<AlgoRequest> request;
} Task_t;

typedef void (*TASKFUNC)(void *Ctx, std::shared_ptr<Task_t> task);
class TaskQueue {
public:
  // Constructor
  explicit TaskQueue(TASKFUNC pExecute, TASKFUNC pCallback, void *TaskCtx);

  // Destructor
  ~TaskQueue();

  // Set the thread name
  void SetThread(const std::string &name);

  // Enqueue a payload
  void Enqueue(std::shared_ptr<Task_t> payload);

  // Task and callback
  TASKFUNC pExecute = nullptr;
  TASKFUNC pCallback = nullptr;
  void *pTaskCtx = nullptr;

  // Wait for queue to complete
  void WaitForQueueCompetion();

  // Stop the worker thread
  void StopWorkerThread();

  /*for tracking/debug */
  size_t mEnQRequestSize = 0;
  size_t mProcessSize = 0;
  size_t mCallbackSize = 0;

private:
  // Internal worker thread function
  static void *WorkerThreadFuction(void *arg);

  // Member variables
  // Queue to hold tasks
  std::queue<std::shared_ptr<Task_t>> mTaskQueue; // Queue to hold tasks
  std::mutex mTaskQMux;         // Mutex for accessing taskQueue
  pthread_t mWorkerThread;      // Thread handle
  std::atomic<bool> bIsRunning; // Atomic flag to check if the thread should run
  std::condition_variable
      mConditionVar; // Condition variable for synchronization
};

#endif // TASK_QUEUE_H
