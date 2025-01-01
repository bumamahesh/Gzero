#include "../include/TaskQueue.h"
//#include <iostream>
#include <stdexcept>

/***
 * @brief Worker thread function that processes tasks from the queue
 */
void *TaskQueue::WorkerThreadFuction(void *arg) {
  TaskQueue *taskQueueObj = static_cast<TaskQueue *>(arg);

  // std::cout << "Worker thread started" << std::endl;
  while (taskQueueObj->isRunning.load()) {
    std::shared_ptr<Task_t> task = nullptr;

    // Lock the queue and wait until a task is available
    {
      std::unique_lock<std::mutex> lock(taskQueueObj->queueMutex);

      // Wait for a new task or for the thread to stop
      taskQueueObj->conditionVar.wait(lock, [&]() {
        return !taskQueueObj->taskQueue.empty() ||
               !taskQueueObj->isRunning.load();
      });

      if (!taskQueueObj->isRunning.load()) {
        // Exit the thread if the stop signal is received
        break;
      }

      // Get the next task from the queue
      task = taskQueueObj->DeQueue();
    }

    // Process the task
    taskQueueObj->ExecuteTask(task);
    if (taskQueueObj->pCallback != nullptr) {
      taskQueueObj->pCallback(task);
    }
    task = nullptr;
    // taskQueueObj->conditionVar.notify_all();
  }
  // std::cout << "Worker thread exiting" << std::endl;
  return nullptr;
}

/**
@brief Construct a new Task Queue::is Running object
 *
 */
TaskQueue::TaskQueue(void (*callbacks)(std::shared_ptr<Task_t> task)) {

  isRunning = true;
  pCallback = callbacks;
  // Create a worker thread
  int result = pthread_create(&workerThread, nullptr,
                              TaskQueue::WorkerThreadFuction, this);
  if (result != 0) {
    throw std::runtime_error("Failed to create worker thread");
  }
}
/**
@brief Destroy the Task Queue:: Task Queue object
 *
 */
TaskQueue::~TaskQueue() {
  // Set isRunning to false to stop the worker thread
  isRunning.store(false);

  // Notify the worker thread to wake up and exit
  conditionVar.notify_all();

  // Wait for the thread to join (i.e., complete its execution)
  pthread_join(workerThread, nullptr);

  // Discard all remaining tasks in the queue
  {
    std::lock_guard<std::mutex> lock(queueMutex);
    while (!taskQueue.empty()) {
      taskQueue.pop();
    }
  }
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
  // Lock the queue and add the payload
  {
    std::lock_guard<std::mutex> lock(queueMutex);
    taskQueue.push(payload);
  }

  // Notify the worker thread that a new task is available
  conditionVar.notify_one();
}

/**
@brief Dequeue the task
 *
 * @return Task_t*
 */
std::shared_ptr<Task_t> TaskQueue::DeQueue() {
  std::shared_ptr<Task_t> task = nullptr;
  {
    // std::lock_guard<std::mutex> lock(queueMutex);
    if (!taskQueue.empty()) {
      task = taskQueue.front();
      taskQueue.pop();
    }
  }
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

  std::unique_lock<std::mutex> lock(queueMutex);
  while (!taskQueue.empty() || isRunning.load()) {
    // Wait with a timeout
    if (conditionVar.wait_for(lock, std::chrono::milliseconds(100), [&]() {
          return taskQueue.empty() && isRunning.load();
        })) {
      // If the condition is satisfied, exit the loop
      break;
    }

    // You can perform additional checks or log information here if needed
  }
}
