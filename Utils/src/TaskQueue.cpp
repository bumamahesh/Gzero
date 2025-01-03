#include "../include/TaskQueue.h"
#include <iostream>
#include <ostream>
#include <stdexcept>
#include <unistd.h> //sleep
/***
 * @brief Worker thread function that processes tasks from the queue
 */
void *TaskQueue::WorkerThreadFuction(void *arg) {
  TaskQueue *taskQueueObj = static_cast<TaskQueue *>(arg);
  // std::cout << "Worker thread Launched ....." << std::endl;
  if (taskQueueObj == nullptr) {
    return nullptr;
  }
  taskQueueObj->isRunning = true;
  // std::cout << "Worker thread started" << std::endl;
  while (true) {
    std::shared_ptr<Task_t> task = nullptr;
    // std::cout << "Worker thread started Before Lock E" << std::endl;
    // Lock the queue and wait until a task is available
    {
      std::unique_lock<std::mutex> lock(taskQueueObj->taskQMux);

      // Wait for a new task or for the thread to stop
      taskQueueObj->conditionVar.wait(lock, [&]() {
        return !taskQueueObj->taskQueue.empty() ||
               !taskQueueObj->isRunning.load();
      });

      if (!taskQueueObj->isRunning.load()) {
        // Exit the thread if the stop signal is received
        break;
      }
      // std::cout << "Worker thread started Before Lock X" << std::endl;
      //  Get the next task from the queue

      if (!taskQueueObj->taskQueue.empty()) {
        task = taskQueueObj->taskQueue.front();
        taskQueueObj->taskQueue.pop();
      }
    }
    // Process the task
    if (task) {
      taskQueueObj->ExecuteTask(task);
      if (taskQueueObj->pCallback != nullptr) {
        taskQueueObj->pCallback(task);
      }
    } else {
      break;
    }
  }
  // std::cout << "Worker thread exiting" << std::endl;
  return nullptr;
}

/**
@brief Construct a new Task Queue::is Running object
 *
 */
TaskQueue::TaskQueue(void (*callbacks)(std::shared_ptr<Task_t> task)) {

  pCallback = callbacks;
  isRunning = false;
  // Create a worker thread
  int result = pthread_create(&workerThread, nullptr,
                              TaskQueue::WorkerThreadFuction, this);
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
@brief Dequeue the task
 *
 * @return Task_t*
 */
std::shared_ptr<Task_t> TaskQueue::DeQueue() {
  // std::cout << "DeQueue E" << std::endl;
  std::shared_ptr<Task_t> task = nullptr;
  {
    std::lock_guard<std::mutex> lock(taskQMux);
    if (!taskQueue.empty()) {
      task = taskQueue.front();
      taskQueue.pop();
    }
  }
  // std::cout << "DeQueue X" << std::endl;
  return task;
}

/**
@brief Execute the task
 *
 * @param task
 */
void TaskQueue::ExecuteTask(std::shared_ptr<Task_t> task) {
  // Execute the task (this is where the task would be processed)
  // std::cout << "Processing task: " << task << std::endl;
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
    std::cout << "Waiting for queue to complete Q size" << taskQueue.size()
              << std::endl;

    // You can perform additional checks or log information here if needed
  }
}
#include <iostream>
/**
@brief Stop the worker thread
 *
 */
void TaskQueue::StopWorkerThread() {

  if (!isRunning.load()) {
    std::cout << "StopWorkerThread  not running" << std::flush << std::endl;
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