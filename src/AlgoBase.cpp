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
  std::shared_ptr<AlgoRequest> req = task->request;
  AlgoBase::AlgoStatus rc = pCtx->Process(req);
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
  if (pCtx && pCtx->pEventHandlerThread) {
    AlgoMessageType msgType;
    AlgoStatus algoStatus = pCtx->GetAlgoStatus();
    try {
      if (task->request) {
        task->request->mProcessCnt++;
      }
    } catch (const std::exception &e) {
      LOG(ERROR, ALGOBASE, "Exception while accessing mProcessCnt: %s",
          e.what());
      return;
    }
    if (pCtx->GetAlgoStatus() == AlgoBase::AlgoStatus::SUCCESS) {
      msgType = AlgoMessageType::ProcessingCompleted;
      if (pCtx->bIslastNode) {
        msgType = AlgoMessageType::ProcessDone;
      }
    } else if (pCtx->GetAlgoStatus() == AlgoBase::AlgoStatus::TIMEOUT) {
      msgType = AlgoMessageType::ProcessingTimeout;
    } else {
      msgType = AlgoMessageType::ProcessingFailed;
    }

    /* if ((msgType != AlgoMessageType::ProcessingCompleted) &&
         (msgType != AlgoMessageType::ProcessDone)) {
       LOG(DEBUG, ALGOBASE, "Somerthing Worong in Node::%s",
           pCtx->GetStatusString().c_str());
     }*/

    pCtx->SetEvent(std::make_shared<AlgoBase::AlgoCallbackMessage>(
        msgType, algoStatus, task, pCtx->GetAlgoId()));
  } else {
    LOG(ERROR, ALGOBASE, "  pEventHandlerThread is nullptr");
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
AlgoBase::AlgoBase(const char *name)
    : mAlgoOperations{std::string(name), nullptr}, mCurrentStatus{
                                                       AlgoStatus::SUCCESS} {
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
AlgoBase::AlgoStatus AlgoBase::GetAlgoStatus() const { return mCurrentStatus; }

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
      {AlgoStatus::INTERNAL_ERROR, "INTERNAL_ERROR"},
      {AlgoStatus::FAILURE, "FAILURE"}};

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
@brief Set Event Thread object
 *
 * @param mEventCallbackThread
 */
void AlgoBase::SetEventThread(
    std::shared_ptr<EventHandlerThread<AlgoBase::AlgoCallbackMessage>>
        mEventCallbackThread) {
  pEventHandlerThread = mEventCallbackThread;
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

/**
 * @brief Set Event Message
 *
 * @param msg
 */
void AlgoBase::SetEvent(std::shared_ptr<AlgoCallbackMessage> msg) {
  if (pEventHandlerThread) {
    pEventHandlerThread->SetEvent(msg);
  } else {
    LOG(ERROR, ALGOBASE, "pEventHandlerThread is nullptr");
  }
}