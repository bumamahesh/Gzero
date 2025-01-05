#include "AlgoBase.h"
#include "Log.h"
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
    msg->mRequest = task;
    msg->mStatus = pCtx->GetStatus();
    if (msg->mStatus == AlgoBase::AlgoStatus::SUCCESS) {
      msg->mType = AlgoBase::ALGO_PROCESSING_COMPLETED;
    } else {
      msg->mType = AlgoBase::ALGO_PROCESSING_FAILED;
    }
    if (pCtx->pNotifyEvent) {
      pCtx->pNotifyEvent(pCtx, msg);
    } else {
      LOG(ERROR, ALGOBASE, "pCtx->pNotifyEvent is nullptr");
    }
  } else {
    LOG(ERROR, ALGOBASE, "pCtx is nullptr");
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
    : mAlgoOperations{name, nullptr}, mCurrentStatus{AlgoStatus::SUCCESS} {
  mAlgoThread = std::make_shared<TaskQueue>(&AlgoBase::ThreadFunction,
                                            &AlgoBase::ThreadCallback, this);
  mAlgoThread->SetThread(name);
}

/**
@brief Destroy the Algo Base:: Algo Base object
 *
 */
AlgoBase::~AlgoBase() { mAlgoThread->WaitForQueueCompetion(); }

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
AlgoBase::AlgoStatus AlgoBase::GetStatus() const { return mCurrentStatus; }

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

  auto it = status_map.find(mCurrentStatus);
  return (it != status_map.end()) ? it->second : "UNKNOWN_STATUS";
}

/**
@brief Get the Algorithm Name object
 *
 * @return std::string
 */
std::string AlgoBase::GetAlgorithmName() const {
  return mAlgoOperations.mAlgoName;
}

/**
@brief Get the Algo Id object
 *
 * @return size_t
 */
AlgoId AlgoBase::GetAlgoId() const { return mAlgoId; }

/**
@brief Set the Status object
 *
 * @param status
 */
void AlgoBase::SetStatus(AlgoStatus status) { mCurrentStatus = status; }

/**
@brief  Enqueue Request object
 *
 * @param request
 */
void AlgoBase::EnqueueRequest(std::shared_ptr<Task_t> request) {
  if (!request) {
    LOG(ERROR, ALGOBASE, "request is nullptr");
    return;
  }
  mAlgoThread->Enqueue(request);
}

/**
@brief Set Notify Event object
 *
 * @param NotifyEvent
 */
void AlgoBase::SetNotifyEvent(
    void (*EventHandler)(void *ctx, std::shared_ptr<ALGOCALLBACKMSG> msg)) {
  this->pNotifyEvent = EventHandler;
  LOG(INFO, ALGOBASE, "pNotifyEvent Callback is set");
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
  mNextAlgo = nextAlgo;
}

/**
@brief Get Next Algo object
 *
 * @return std::weak_ptr<AlgoBase>
 */
std::weak_ptr<AlgoBase> AlgoBase::GetNextAlgo() { return mNextAlgo; }