#ifndef ALGO_BASE_H
#define ALGO_BASE_H

#include "TaskQueue.h"
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <iostream>
#include <memory>
#include <mutex>
#include <pthread.h>
#include <queue>
#include <string>
#include <unordered_map>

class AlgoBase {
public:
  enum class AlgoStatus {
    SUCCESS = 0,
    NOT_INITIALIZED = 1,
    ALREADY_OPEN = 2,
    ALREADY_CLOSED = 3,
    INVALID_INPUT = 4,
    RESOURCE_UNAVAILABLE = 5,
    TIMEOUT = 6,
    OPERATION_FAILED = 7,
    OUT_OF_MEMORY = 8,
    PERMISSION_DENIED = 9,
    NOT_SUPPORTED = 10,
    INTERNAL_ERROR = 11
  };

  struct AlgorithmOperations {
    std::string algorithm_name;
    void *context = nullptr;
  };
  // Constructors
  AlgoBase();
  // Constructor
  explicit AlgoBase(const std::string &name);
  // Destructor
  virtual ~AlgoBase();
  // Public member functions
  virtual AlgoStatus Open() = 0;
  virtual AlgoStatus Process() = 0;
  virtual AlgoStatus Close() = 0;
  AlgoStatus GetStatus() const;
  std::string GetStatusString() const;
  std::string GetAlgorithmName() const;
  size_t GetAlgoId() const;
  void EnqueueRequest(const std::string &request);

protected:
  AlgorithmOperations ops_;
  AlgoStatus current_status_ = AlgoStatus::SUCCESS;
  pthread_t thread_;
  bool thread_started_ = false;
  std::mutex thread_mutex_;
  std::condition_variable thread_cond_;
  std::condition_variable queue_cond_;
  std::atomic<bool> should_exit_ = false;
  std::shared_ptr<TaskQueue> mAlgoThread;
  size_t algo_id_ = 0XDEADBEEF;
  void SetStatus(AlgoStatus status);

private:
  static void ThreadFunction(std::shared_ptr<Task_t>);
  static void ThreadCallback(std::shared_ptr<Task_t>);
};

#endif // ALGO_BASE_H
