#include "../include/TaskQueue.h"
#include "../include/Log.h"

#include <cassert>
#include <stdexcept>
#include <unistd.h> //sleepv
/***
 * @brief Worker thread function that processes tasks from the queue
 */
void *TaskQueue::WorkerThreadFuction(void *arg) {

  TaskQueue *pTaskQObj = static_cast<TaskQueue *>(arg);
  // LOG(VERBOSE, TASKQUEUE, "Worker thread Launched .....");
  assert(pTaskQObj != nullptr);
  assert(pTaskQObj->pExecute != nullptr);
  pTaskQObj->bIsRunning = true;
  bool bShouldMonitor = false;
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
              "TaskQueue has  %ld task pending But still Exiting ...",
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
TaskQueue::TaskQueue(TASKFUNC pExecute, TASKFUNC pCallback, void *pTaskCtx) {

  if (!pExecute || !pCallback || !pTaskCtx) {
    LOG(ERROR, TASKQUEUE, "Invalid function pointer or context");
    std::abort();
  }

  this->pExecute = pExecute;
  this->pCallback = pCallback;
  this->pTaskCtx = pTaskCtx;

  bIsRunning = false;
  // Create a worker thread
  int result = pthread_create(&mWorkerThread, nullptr,
                              &TaskQueue::WorkerThreadFuction, this);
  if (result != 0) {
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
TaskQueue::~TaskQueue() { StopWorkerThread(); }

/**
@brief Set the Thread object
 *
 * @param name
 */
void TaskQueue::SetThread(const std::string &name) {
  pthread_setname_np(mWorkerThread, name.c_str());
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
    pthread_join(mWorkerThread, nullptr);
    mWorkerThread = 0;
    // Discard all remaining tasks in the queue
    {
      std::lock_guard<std::mutex> lock(mTaskQMux);
      while (!mTaskQueue.empty()) {
        mTaskQueue.pop();
      }
    }
  }
}