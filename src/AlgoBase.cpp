#include "AlgoBase.h"
#include <cassert>
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
  // std::cout << "Destructor for AlgoBase E" << std::flush << std::endl;
  // mAlgoThread->WaitForQueueCompetion();
  // std::lock_guard<std::mutex> lock(thread_mutex_);
  // mAlgoThread->StopWorkerThread();
  // std::cout << "Destructor for AlgoBase X" << std::flush << std::endl;
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
    auto ctx = std::static_pointer_cast<AlgoBase>(task->ctx);
    /* a node as completed processing if  */
    assert(ctx != nullptr);
    if (ctx->NotifyEvent) {
      auto msg = std::make_shared<ALGOCALLBACKMSG>();
      assert(msg != nullptr);
      msg->request = task;
      msg->status = ctx->GetStatus();
      msg->type = ALGO_PROCESSING_COMPLETED;
      ctx->NotifyEvent(ctx, msg);
    } else {
      // std::cout << "ThreadCallback: AlgoBase::NotifyEvent Not Set" <<
      // std::endl;
    }
  }
}

/**
@brief  Enqueue Request object
 *
 * @param request
 */
void AlgoBase::EnqueueRequest(std::shared_ptr<Task_t> request) {
  if (!request) {
    return;
  }
  {
    // std::lock_guard<std::mutex> lock(thread_mutex_);
    mAlgoThread->Enqueue(request);
    queue_cond_.notify_all();
  }
}

/**
@brief Set Notify Event object
 *
 * @param NotifyEvent
 */
void AlgoBase::SetNotifyEvent(void (*EventHandler)(
    std::shared_ptr<void> ctx, std::shared_ptr<ALGOCALLBACKMSG> msg)) {
  this->NotifyEvent = EventHandler;
}

/**
@brief Wait For Queue Competion object
 *
 */
void AlgoBase::WaitForQueueCompetion() { mAlgoThread->WaitForQueueCompetion(); }

/**
@brief Set Next Algo object
 *
 */
void AlgoBase::SetNextAlgo(std::weak_ptr<AlgoBase> nextAlgo) {
  m_nextAlgo = nextAlgo;
}

/**
@brief Get Next Algo object
 *
 * @return std::weak_ptr<AlgoBase>
 */
std::weak_ptr<AlgoBase> AlgoBase::GetNextAlgo() { return m_nextAlgo; }