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

private:
  // Internal worker thread function
  static void *WorkerThreadFuction(void *arg);

  // Member variables
  // Queue to hold tasks
  std::queue<std::shared_ptr<Task_t>> taskQueue; // Queue to hold tasks
  std::mutex taskQMux;         // Mutex for accessing taskQueue
  pthread_t workerThread;      // Thread handle
  std::atomic<bool> isRunning; // Atomic flag to check if the thread should run
  std::condition_variable
      conditionVar; // Condition variable for synchronization
};

#endif // TASK_QUEUE_H
