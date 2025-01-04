#include "../include/TaskQueue.h"
#include <cassert>
#include <iostream>
#include <ostream>
#include <stdexcept>
#include <unistd.h> //sleep
/***
 * @brief Worker thread function that processes tasks from the queue
 */
void *TaskQueue::WorkerThreadFuction(void *arg) {

  TaskQueue *pTaskQObj = static_cast<TaskQueue *>(arg);
  // std::cout << "Worker thread Launched ....." << std::endl;
  assert(pTaskQObj != nullptr);
  assert(pTaskQObj->pExecute != nullptr);
  pTaskQObj->isRunning = true;
  // std::cout << "Worker thread started" << std::endl;
  while (true) {
    std::shared_ptr<Task_t> task = nullptr;
    // std::cout << "Worker thread started Before Lock E" << std::endl;
    // Lock the queue and wait until a task is available
    {
      std::unique_lock<std::mutex> lock(pTaskQObj->taskQMux);

      // Wait for a new task or for the thread to stop
      pTaskQObj->conditionVar.wait(lock, [&]() {
        return !pTaskQObj->taskQueue.empty() || !pTaskQObj->isRunning.load();
      });

      if (!pTaskQObj->isRunning.load()) {
        // Exit the thread if the stop signal is received
        break;
      }
      // std::cout << "Worker thread started Before Lock X" << std::endl;
      //  Get the next task from the queue

      if (!pTaskQObj->taskQueue.empty()) {
        task = pTaskQObj->taskQueue.front();
        pTaskQObj->taskQueue.pop();
      }
    }

    /*Process is here */
    if (pTaskQObj->pExecute) {
      pTaskQObj->pExecute(pTaskQObj->pTaskCtx, task);
    } else {
      std::cout << "No Execute" << std::endl;
    }
    if (pTaskQObj->pCallback) {
      pTaskQObj->pCallback(pTaskQObj->pTaskCtx, task);
    } else {
      std::cout << "No Callback" << std::endl;
    }
  }
  // std::cout << "Worker thread exiting" << std::endl;
  return nullptr;
}

/**
@brief Construct a new Task Queue::is Running object
 *
 */
TaskQueue::TaskQueue(TASKFUNC pExecute, TASKFUNC pCallback, void *pTaskCtx) {

  if (!pExecute || !pCallback || !pTaskCtx) {
    // Log or handle the invalid inputs
    std::cerr << "Invalid function pointer or context!" << std::endl;
    std::abort();
  }

  this->pExecute = pExecute;
  this->pCallback = pCallback;
  this->pTaskCtx = pTaskCtx;

  isRunning = false;
  // Create a worker thread
  int result = pthread_create(&workerThread, nullptr,
                              &TaskQueue::WorkerThreadFuction, this);
  if (result != 0) {
    throw std::runtime_error("Failed to create worker thread");
  }
  /*wait for thread to be start executing*/
  while (!isRunning.load()) {
    usleep(100);
  };
}
/**
@brief Destroy the Task Queue:: Task Queue object
 *
 */
TaskQueue::~TaskQueue() {

  StopWorkerThread();
  // std::cout << "Task Queue destroyed" << std::endl;
}

/**
@brief Set the Thread object
 *
 * @param name
 */
void TaskQueue::SetThread(const std::string &name) {
  pthread_setname_np(workerThread, name.c_str());
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
    std::unique_lock<std::mutex> lock(taskQMux);
    // Lock acquired, add the payload
    taskQueue.push(payload);
  }

  // Notify the worker thread that a new task is available
  conditionVar.notify_all();
}

/**
@brief Wait for queue competion
 *
 */
void TaskQueue::WaitForQueueCompetion() {

  std::unique_lock<std::mutex> lock(taskQMux);
  while (!taskQueue.empty() || isRunning.load()) {
    // Wait with a timeout
    if (conditionVar.wait_for(lock, std::chrono::milliseconds(100), [&]() {
          return taskQueue.empty() && isRunning.load();
        })) {
      // If the condition is satisfied, exit the loop
      break;
    }
    /* std::cout << "Waiting for queue to complete Q size" << taskQueue.size()
               << std::endl;*/
  }
}
#include <iostream>
/**
@brief Stop the worker thread
 *
 */
void TaskQueue::StopWorkerThread() {

  if (!isRunning.load()) {
    // std::cout << "StopWorkerThread  not running" << std::flush << std::endl;
    return;
  } else {

    // std::cout << "StopWorkerThread  E" << std::flush << std::endl;

    // Set isRunning to false to stop the worker thread
    isRunning.store(false);
    // Notify the worker thread to wake up and exit
    conditionVar.notify_all();
    //  if (workerThread-joinable()) {
    pthread_join(workerThread, nullptr);
    workerThread = 0;
    // }
    // Discard all remaining tasks in the queue
    {
      std::lock_guard<std::mutex> lock(taskQMux);
      while (!taskQueue.empty()) {
        taskQueue.pop();
      }
    }

    // std::cout << "StopWorkerThread  X" << std::flush << std::endl;
    // Wait for the thread to join (i.e., complete its execution)
  }
}