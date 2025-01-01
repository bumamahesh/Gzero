#include "AlgoBase.h"

/**
@brief Construct a new Algo Base:: Algo Base object
 *
 */
AlgoBase::AlgoBase() {
  mAlgoThread = std::make_shared<TaskQueue>(&AlgoBase::ThreadFunction);
  mAlgoThread->pCallback = AlgoBase::ThreadCallback;
  mAlgoThread->SetThread("AlgoBaseDefaultThread");
}

/**
@brief Construct a new Algo Base:: Algo Base object
 *
 * @param name
 */
AlgoBase::AlgoBase(const std::string &name)
    : ops_{name, nullptr}, current_status_{AlgoStatus::SUCCESS} {
  // std::cout << "Constructor for AlgoBase E" << std::endl;
  mAlgoThread = std::make_shared<TaskQueue>(&AlgoBase::ThreadFunction);
  mAlgoThread->pCallback = AlgoBase::ThreadCallback;
  mAlgoThread->SetThread(name);
  // std::cout << "Constructor for AlgoBase X" << std::endl;
}

/**
@brief Destroy the Algo Base:: Algo Base object
 *
 */
AlgoBase::~AlgoBase() {
  // std::cout << "Destructor for AlgoBase E" << std::endl;
  mAlgoThread->WaitForQueueCompetion();
  // std::cout << "Destructor for AlgoBase X" << std::endl;
}

/**
@brief Get the Status object
 *
 * @return AlgoBase::AlgoStatus
 */
AlgoBase::AlgoStatus AlgoBase::GetStatus() const { return current_status_; }

/**
@brief Get the Status String object
 *
 * @return std::string
 */
std::string AlgoBase::GetStatusString() const {
  static const std::unordered_map<AlgoStatus, std::string> status_map = {
      {AlgoStatus::SUCCESS, "SUCCESS"},
      {AlgoStatus::NOT_INITIALIZED, "NOT_INITIALIZED"},
      {AlgoStatus::ALREADY_OPEN, "ALREADY_OPEN"},
      {AlgoStatus::ALREADY_CLOSED, "ALREADY_CLOSED"},
      {AlgoStatus::INVALID_INPUT, "INVALID_INPUT"},
      {AlgoStatus::RESOURCE_UNAVAILABLE, "RESOURCE_UNAVAILABLE"},
      {AlgoStatus::TIMEOUT, "TIMEOUT"},
      {AlgoStatus::OPERATION_FAILED, "OPERATION_FAILED"},
      {AlgoStatus::OUT_OF_MEMORY, "OUT_OF_MEMORY"},
      {AlgoStatus::PERMISSION_DENIED, "PERMISSION_DENIED"},
      {AlgoStatus::NOT_SUPPORTED, "NOT_SUPPORTED"},
      {AlgoStatus::INTERNAL_ERROR, "INTERNAL_ERROR"}};

  auto it = status_map.find(current_status_);
  return (it != status_map.end()) ? it->second : "UNKNOWN_STATUS";
}

/**
@brief Get the Algorithm Name object
 *
 * @return std::string
 */
std::string AlgoBase::GetAlgorithmName() const { return ops_.algorithm_name; }

/**
@brief Get the Algo Id object
 *
 * @return size_t
 */
size_t AlgoBase::GetAlgoId() const { return algo_id_; }

/**
@brief Set the Status object
 *
 * @param status
 */
void AlgoBase::SetStatus(AlgoStatus status) { current_status_ = status; }

/**
@brief Thread Function object
 *
 */
void AlgoBase::ThreadFunction(std::shared_ptr<Task_t>) {
  // Placeholder implementation. Extend as needed.
}

/**
@brief Thread Callback object
 *  This will be called when the thread is done with the task
 * @param task
 */
void AlgoBase::ThreadCallback(std::shared_ptr<Task_t> task) {
  // Placeholder implementation. Extend as needed.

  if (task) {
    if (task->args) {
      std::string *request = reinterpret_cast<std::string *>(task->args);
      // std::cout << "ThreadCallback: " << *request << std::endl;
      delete request;
    }
  }
}

/**
@brief  Enqueue Request object
 *
 * @param request
 */
void AlgoBase::EnqueueRequest(const std::string &request) {
  {
    std::lock_guard<std::mutex> lock(thread_mutex_);
    std::shared_ptr<Task_t> task_req = std::make_shared<Task_t>();
    if (mAlgoThread->pCallback) {
      /*callback is where we free obj so make sure a callnackexist first*/
      task_req->args = reinterpret_cast<void *>(new std::string(request));
    }
    mAlgoThread->Enqueue(task_req);
    queue_cond_.notify_all();
  }
}
