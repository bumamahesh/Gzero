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
#ifndef TASK_QUEUE_H
#define TASK_QUEUE_H
#pragma once
#include "AlgoRequest.h"
#include "RequestMonitor.h"
#include "Task.h"
#include <atomic>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <pthread.h>
#include <queue>

typedef void (*TASKFUNC)(void *Ctx, std::shared_ptr<Task_t> task);
class TaskQueue {
public:
  // Constructor
  explicit TaskQueue(TASKFUNC pExecute, TASKFUNC pCallback, void *pTaskCtx);

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
  std::shared_ptr<RequestMonitor> monitor;

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
