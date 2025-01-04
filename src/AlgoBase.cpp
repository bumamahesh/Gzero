#include "AlgoBase.h"
#include <cassert>

/**
@brief Thread Function object
 *
 */
void AlgoBase::ThreadFunction(void *Ctx, std::shared_ptr<Task_t> task) {

  assert(task != nullptr);
  assert(Ctx != nullptr);
  auto pCtx = static_cast<AlgoBase *>(Ctx);
  AlgoBase::AlgoStatus rc = pCtx->Process();
  pCtx->SetStatus(rc);
  /* if (rc != AlgoBase::AlgoStatus::SUCCESS) {
     std::cout << pCtx->GetAlgorithmName() << "::" << pCtx->GetStatusString()
               << std::endl;
   }*/
}

/**
@brief Thread Callback object
 *  This will be called when the thread is done with the task
 * @param task
 */
void AlgoBase::ThreadCallback(void *Ctx, std::shared_ptr<Task_t> task) {

  assert(task != nullptr);
  assert(Ctx != nullptr);
  auto pCtx = static_cast<AlgoBase *>(Ctx);
  if (pCtx) {
    auto msg = std::make_shared<AlgoBase::ALGOCALLBACKMSG>();
    assert(msg != nullptr);
    msg->request = task;
    msg->status = pCtx->GetStatus();
    if (msg->status == AlgoBase::AlgoStatus::SUCCESS) {
      msg->type = AlgoBase::ALGO_PROCESSING_COMPLETED;
    } else {
      msg->type = AlgoBase::ALGO_PROCESSING_FAILED;
    }
    if (pCtx->NotifyEvent) {
      pCtx->NotifyEvent(pCtx, msg);
    } else {
      std::cout << "pCtx->NotifyEvent is nullptr" << std::endl;
    }
  } else {
    std::cout << "pCtx is nullptr" << std::endl;
  }
}

/**
@brief Construct a new Algo Base:: Algo Base object
 *
 */
AlgoBase::AlgoBase() {
  mAlgoThread = std::make_shared<TaskQueue>(&AlgoBase::ThreadFunction,
                                            &AlgoBase::ThreadCallback, this);
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
  mAlgoThread = std::make_shared<TaskQueue>(&AlgoBase::ThreadFunction,
                                            &AlgoBase::ThreadCallback, this);
  mAlgoThread->SetThread(name);
  // std::cout << "Constructor for AlgoBase X" << std::endl;
}

/**
@brief Destroy the Algo Base:: Algo Base object
 *
 */
AlgoBase::~AlgoBase() {
  // std::cout << "Destructor for AlgoBase E" << std::flush << std::endl;
  mAlgoThread->WaitForQueueCompetion();
  // std::lock_guard<std::mutex> lock(thread_mutex_);
  // mAlgoThread->StopWorkerThread();
  // std::cout << "Destructor for AlgoBase X" << std::flush << std::endl;
}

/**
 * @brief  Stop Worker Thread
 *
 */
void AlgoBase::StopAlgoThread() { mAlgoThread->StopWorkerThread(); }

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
void AlgoBase::SetNotifyEvent(
    void (*EventHandler)(void *ctx, std::shared_ptr<ALGOCALLBACKMSG> msg)) {
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