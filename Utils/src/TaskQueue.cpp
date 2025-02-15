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
#include "../include/TaskQueue.h"
#include "../include/Log.h"

#include <unistd.h>
#include <cassert>
#include <stdexcept>
/***
 * @brief Worker thread function that processes tasks from the queue
 */
void* TaskQueue::WorkerThreadFuction(void* arg) {

  TaskQueue* pTaskQObj = static_cast<TaskQueue*>(arg);
  // LOG(VERBOSE, TASKQUEUE, "Worker thread Launched .....");
  assert(pTaskQObj != nullptr);
  assert(pTaskQObj->pExecute != nullptr);
  pTaskQObj->bIsRunning = true;
  bool bShouldMonitor   = false;
  while (true) {
    std::shared_ptr<Task_t> task = nullptr;
    // Lock the queue and wait until a task is available
    {
      std::unique_lock<std::mutex> lock(pTaskQObj->mTaskQMux);

      // Wait for a new task or for the thread to stop
      pTaskQObj->mConditionVar.wait(lock, [&]() {
        return !pTaskQObj->mTaskQueue.empty() || !pTaskQObj->bIsRunning.load();
      });

      if (!pTaskQObj->bIsRunning.load()) {
        // Exit the thread if the stop signal is received
        if (!pTaskQObj->mTaskQueue.empty()) {
          LOG(ERROR, TASKQUEUE,
              "TaskQueue has  %zu task pending But still Exiting ...",
              pTaskQObj->mTaskQueue.size());
        }
        break;
      }
      //  Get the next task from the queue

      if (!pTaskQObj->mTaskQueue.empty()) {
        task = pTaskQObj->mTaskQueue.front();
        pTaskQObj->mTaskQueue.pop();
      }
    }

    /*Process is here */
    if (pTaskQObj->pExecute) {
      bShouldMonitor = false;
      if ((pTaskQObj->monitor.get() != nullptr) && (task.get() != nullptr) &&
          (task->request.get() != nullptr)) {
        bShouldMonitor = true;
      } /*else {
        LOG(ERROR, TASKQUEUE, "task is not monitored");
      }*/

      if (bShouldMonitor) {
        pTaskQObj->monitor->StartRequestMonitoring(task, task->timeoutMs);
      }
      pTaskQObj->pExecute(pTaskQObj->pTaskCtx, task);

      if (bShouldMonitor) {
        pTaskQObj->monitor->StopRequestMonitoring(task);
      }
      pTaskQObj->mProcessSize++;
    } else {
      LOG(ERROR, TASKQUEUE, "pExecute is nullptr");
    }
    if (pTaskQObj->pCallback) {
      pTaskQObj->pCallback(pTaskQObj->pTaskCtx, task);
      pTaskQObj->mCallbackSize++;
    } else {
      LOG(ERROR, TASKQUEUE, "pCallback is nullptr");
    }
  }
  /*LOG(VERBOSE, TASKQUEUE, "Worker thread exiting %ld ",
      pTaskQObj->mTaskQueue.size());*/
  return nullptr;
}

/**
@brief Construct a new Task Queue::is Running object
 *
 */
TaskQueue::TaskQueue(TASKFUNC pExecute, TASKFUNC pCallback, void* pTaskCtx) {

  if (!pExecute || !pCallback || !pTaskCtx) {
    LOG(ERROR, TASKQUEUE, "Invalid function pointer or context");
    std::abort();
  }

  this->pExecute  = pExecute;
  this->pCallback = pCallback;
  this->pTaskCtx  = pTaskCtx;

  bIsRunning = false;
  // Create a worker thread
  mWorkerThread =
      std::make_shared<ThreadWrapper>(&TaskQueue::WorkerThreadFuction, this);
  if (!mWorkerThread) {
    throw std::runtime_error("Failed to create worker thread");
  }
  /*wait for thread to be start executing*/
  while (!bIsRunning.load()) {
    usleep(100);
  };

  this->monitor = std::make_shared<RequestMonitor>();
}
/**
@brief Destroy the Task Queue:: Task Queue object
 *
 */
TaskQueue::~TaskQueue() {
  StopWorkerThread();
}

/**
@brief Set the Thread object
 *
 * @param name
 */
void TaskQueue::SetThread(const std::string& name) {
  mWorkerThread->ThreadSetname(name.c_str());
}

/**
 * @brief  Enqueue a payload
 *
 * @param payload
 */

void TaskQueue::Enqueue(std::shared_ptr<Task_t> payload) {
  if (!payload) {
    throw std::invalid_argument("Invalid payload");
  }
  {
    // Try to acquire the lock with a timeout
    std::unique_lock<std::mutex> lock(mTaskQMux);
    // Lock acquired, add the payload
    mTaskQueue.push(payload);
    mEnQRequestSize++;
  }

  // Notify the worker thread that a new task is available
  mConditionVar.notify_all();
}

/**
@brief Wait for queue competion
 *
 */
void TaskQueue::WaitForQueueCompetion() {

  std::unique_lock<std::mutex> lock(mTaskQMux);
  while (!mTaskQueue.empty() || bIsRunning.load()) {
    // Wait with a timeout
    if (mConditionVar.wait_for(lock, std::chrono::milliseconds(200), [&]() {
          // return mTaskQueue.empty() && bIsRunning.load();
          return (mEnQRequestSize == mProcessSize) && bIsRunning.load();
        })) {
      // If the condition is satisfied, exit the loop
      break;
    }
  }
  /*LOG(VERBOSE, TASKQUEUE, "mTaskQueue size ::%ld %ld %ld %ld %d",
      mTaskQueue.size(), mEnQRequestSize, mProcessSize, mCallbackSize,
      (int)bIsRunning.load());*/
}

/**
@brief Stop the worker thread
 *
 */
void TaskQueue::StopWorkerThread() {

  if (!bIsRunning.load()) {
    return;
  } else {
    // Set bIsRunning to false to stop the worker thread
    bIsRunning.store(false);
    // Notify the worker thread to wake up and exit
    mConditionVar.notify_all();
    mWorkerThread->join();
    // Discard all remaining tasks in the queue
    {
      std::lock_guard<std::mutex> lock(mTaskQMux);
      while (!mTaskQueue.empty()) {
        mTaskQueue.pop();
      }
    }
  }
}