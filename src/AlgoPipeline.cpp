#include "AlgoPipeline.h"
#include "Log.h"
#include <assert.h>
/**
@brief Constructs a new AlgoPipeline object with a list of algorithm IDs
 *
 * @param algoList
 */
AlgoPipeline::AlgoPipeline(SESSIONCALLBACK pSesionCallBackHandler, void *pCtx) {
  mProcessedFrames = 0;
  mState = AlgoPipelineState::Initialised;
  this->pSesionCallBackHandler = pSesionCallBackHandler;
  this->pSessionCtx = pCtx;
  pEventHandlerThread =
      std::make_shared<EventHandlerThread<AlgoBase::AlgoCallbackMessage>>(
          AlgoPipeline::NodeEventHandler, this);
}

/**
 * @brief Destroy the Algo Pipeline:: Algo Pipeline object
 *
 */
AlgoPipeline::~AlgoPipeline() {
  for (auto &algo : mAlgos) {
    algo->WaitForQueueCompetion();
  }
  pEventHandlerThread->stop();
  mAlgos.clear();
  mAlgoListId.clear();
  mAlgoListName.clear();
}

/**
 * @brief  Get state of Pipeline
 *
 * @return AlgoPipelineState
 */
AlgoPipelineState AlgoPipeline::GetState() const { return mState; }

/**
 * @brief  Set state of pipeline
 *
 * @param state
 * @return AlgoPipelineState
 */
AlgoPipelineState AlgoPipeline::SetState(AlgoPipelineState state) {
  mState = state;
  return mState;
}

/**
 * @brief  Get Algo List Id
 *
 * @return std::vector<size_t>
 */
std::vector<AlgoId> AlgoPipeline::GetAlgoListId() const { return mAlgoListId; }

/**
 * @brief  Configure Pipeline with Provided algo List
 *
 * @param algoList
 * @return AlgoPipelineState
 */
AlgoPipelineState
AlgoPipeline::ConfigureAlgoPipeline(std::vector<AlgoId> &algoList) {

  // LOG(VERBOSE, ALGOPIPELINE, "Configuring AlgoPipeline :: %ld",
  //     algoList.size());

  if (GetState() == AlgoPipelineState::Initialised) {
    if (algoList.size() == 0) {
      LOG(ERROR, ALGOPIPELINE, "AlgoList is empty");
      return SetState(AlgoPipelineState::FailedToConfigure);
    }
    mAlgoListId = algoList;
    mProcessedFrames = 0;
    mAlgoNodeMgr = &AlgoNodeManager::Getinstance();
    assert(mAlgoNodeMgr != nullptr);

    std::shared_ptr<AlgoBase> previousAlgo = nullptr;
    for (auto algoId : mAlgoListId) {
      auto algo = mAlgoNodeMgr->CreateAlgo(algoId);
      if (algo == nullptr) {
        return SetState(AlgoPipelineState::FailedToConfigure);
      }
      algo->SetEventThread(pEventHandlerThread);
      mAlgos.push_back(algo);
      mAlgoListName.push_back(std::string(algo->GetAlgorithmName()));
      if (previousAlgo) {
        previousAlgo->SetNextAlgo(algo);
      }
      previousAlgo = algo;
      mAlgoMap[algoId] = algo;
    }
    previousAlgo->bIslastNode = true; // lets mark last  node
    SetState(AlgoPipelineState::ConfiguredWithId);
  } else {
    LOG(ERROR, ALGOPIPELINE,
        "AlgoPipeline is not Currect State to Configure ::%d", (int)GetState());
  }
  return GetState();
}

/**
 * @brief  Configure Pipeline with Provided algo List
 *
 * @param algoList
 * @return AlgoPipelineState
 */
AlgoPipelineState
AlgoPipeline::ConfigureAlgoPipeline(std::vector<std::string> &algoList) {

  LOG(VERBOSE, ALGOPIPELINE, "Configuring AlgoPipeline :: %ld",
      algoList.size());
  if (GetState() == AlgoPipelineState::Initialised) {
    if (algoList.size() == 0) {
      LOG(ERROR, ALGOPIPELINE, "AlgoList is empty");
      return SetState(AlgoPipelineState::FailedToConfigure);
    }
    mAlgoListName = algoList;
    mProcessedFrames = 0;
    mAlgoNodeMgr = &AlgoNodeManager::Getinstance();
    assert(mAlgoNodeMgr != nullptr);

    std::shared_ptr<AlgoBase> previousAlgo = nullptr;
    for (auto algoName : mAlgoListName) {
      auto algo = mAlgoNodeMgr->CreateAlgo(algoName);
      if (algo == nullptr) {
        return SetState(AlgoPipelineState::FailedToConfigure);
      }
      algo->SetEventThread(pEventHandlerThread);
      mAlgos.push_back(algo);
      mAlgoListId.push_back(algo->GetAlgoId());
      if (previousAlgo) {
        previousAlgo->SetNextAlgo(algo);
      }
      previousAlgo = algo;
      mAlgoMap[algo->GetAlgoId()] = algo;
    }
    previousAlgo->bIslastNode = true; // lets mark last  node
    SetState(AlgoPipelineState::ConfiguredWithName);
  } else {
    LOG(ERROR, ALGOPIPELINE, "AlgoPipeline is not Currect State to Configure");
  }
  return GetState();
}

/**
 * @brief Process Request on Pipeline
 *
 * @param input
 */
void AlgoPipeline::Process(std::shared_ptr<AlgoRequest> input) {

  if (mAlgos.size() == 0) {
    // LOG(ERROR, ALGOPIPELINE, "No algos to process");
    return;
  }
  if (GetState() == AlgoPipelineState::ConfiguredWithName ||
      GetState() == AlgoPipelineState::ConfiguredWithId) {
    std::shared_ptr<Task_t> task = std::make_shared<Task_t>();
    task->request = input;
    {
      std::lock_guard<std::mutex> lock(mRequesteMapMutex);
      if (mRequesteMap.find(input->mRequestId) == mRequesteMap.end()) {
        /* LOG(VERBOSE, ALGOPIPELINE, "Added for Processing  Request ID:
           %d::%p", input->mRequestId, input.get());*/
        mRequesteMap.insert({input->mRequestId, input});
      } else {
        LOG(ERROR, ALGOPIPELINE, "Error Duplicate Request ID: %d::%p",
            input->mRequestId, input.get());
      }
    }
    mAlgos[0]->EnqueueRequest(task);

  } else {
    LOG(ERROR, ALGOPIPELINE, "AlgoPipeline is not Currect State to Process");
  }
}

/**
 * @brief  Node Event Handler
 *
 * @param ctx
 * @param msg
 */
void AlgoPipeline::NodeEventHandler(
    void *ctx, std::shared_ptr<AlgoBase::AlgoCallbackMessage> msg) {
  // static lockguard
  static std::mutex mCallbackMutex;
  std::lock_guard<std::mutex> lock(mCallbackMutex);
  assert(msg != nullptr);
  assert(ctx != nullptr);
  auto plPipeline = reinterpret_cast<AlgoPipeline *>(ctx);
  assert(plPipeline != nullptr);
  assert(plPipeline->mAlgoMap.find(msg->mAlgoId) != plPipeline->mAlgoMap.end());
  auto algo = plPipeline->mAlgoMap.at(msg->mAlgoId);
  assert(algo != nullptr);
  /*LOG(VERBOSE, ALGOPIPELINE, "NodeEventHandler:: [%d][%ld::%ld] Event:: %d",
      msg->mRequest->request->mRequestId, msg->mRequest->request->mProcessCnt,
      plPipeline->mAlgoMap.size(), (int)msg->mType);*/
  switch (msg->mType) {
  case AlgoBase::AlgoMessageType::ProcessingCompleted: {
    /*some node */
    // LOG(VERBOSE, ALGOPIPELINE, "Node Processing Completed:: %d by node %s",
    //    msg->mRequest->request->mRequestId, algo->GetAlgorithmName().c_str());
    std::shared_ptr<AlgoBase> NextAlgo = algo->GetNextAlgo().lock();
    if (NextAlgo) {
      NextAlgo->EnqueueRequest(msg->mRequest);
    }
  } break;
  case AlgoBase::AlgoMessageType::ProcessDone: {
    /**last node  */
    std::shared_ptr<AlgoRequest> Output = msg->mRequest->request;

    if (Output) {
      if (Output->mProcessCnt != plPipeline->mAlgos.size()) {
        LOG(ERROR, ALGOPIPELINE, "Output is not complete  %ld ,%ld",
            Output->mProcessCnt, plPipeline->mAlgos.size());
      }
    }

    {
      /*request is processed remove from request Quew*/
      std::lock_guard<std::mutex> lock(plPipeline->mRequesteMapMutex);
      auto callbackRequestId = msg->mRequest->request->mRequestId;
      if (plPipeline->mRequesteMap.find(callbackRequestId) !=
          plPipeline->mRequesteMap.end()) {
        /*LOG(VERBOSE, ALGOPIPELINE,
            "[%p][%6ld]Processing Completed for Request ID: %d::%p ",
            plPipeline, plPipeline->mProcessedFrames, callbackRequestId,
            Output.get());*/
        // Remove processed request
        plPipeline->mRequesteMap.erase(callbackRequestId);
      } else {
        LOG(ERROR, ALGOPIPELINE, "Request not present is Q Fatal ::%d",
            callbackRequestId);
      }
      plPipeline->mCondition.notify_all(); // Notify waiting threads
    }

    if (plPipeline->pSesionCallBackHandler) {
      plPipeline->pSesionCallBackHandler(plPipeline->pSessionCtx, Output);
    }
    plPipeline->mProcessedFrames++;
  } break;
  case AlgoBase::AlgoMessageType::ProcessingFailed:
    LOG(ERROR, ALGOPIPELINE, "Processing Failed");
    plPipeline->mState = AlgoPipelineState::FailedToProcess;
    break;
  case AlgoBase::AlgoMessageType::ProcessingTimeout:
    LOG(ERROR, ALGOPIPELINE, "Processing Timeout");
    plPipeline->mState = AlgoPipelineState::FailedToProcess;
    break;
  case AlgoBase::AlgoMessageType::ProcessingPartial:
    LOG(ERROR, ALGOPIPELINE, "Partial Processing");
    break;
  default:
    LOG(ERROR, ALGOPIPELINE, "Unknown Message Type");
    break;
  }
}

/**
 * @brief   Wait for the queue to complete
 *
 */
void AlgoPipeline::WaitForQueueCompetion() {

  const std::chrono::milliseconds timeoutDuration(500);
  int statsTimeout = 0;
  std::unique_lock<std::mutex> lock(mRequesteMapMutex);

  while (!mRequesteMap.empty()) {
    if (mCondition.wait_for(lock, timeoutDuration,
                            [this]() { return mRequesteMap.empty(); })) {
      break;
    }
    statsTimeout++;
    if (statsTimeout > 10) {
      for (auto req : mRequesteMap) {
        LOG(ERROR, ALGOPIPELINE,
            " [%ld]Pending Request ID::%d Node Completed::%ld", mAlgoMap.size(),
            req.first, req.second->mProcessCnt);
      }
      statsTimeout = 0;
    }
    std::cout << "Waiting... " << mRequesteMap.size()
              << " requests still pending.\n";
  }

  /*
    for (auto &algo : mAlgos) {

      algo->WaitForQueueCompetion();
    }
    */
}

/**
 * @brief Get Processed Frames
 *
 * @return size_t
 */
size_t AlgoPipeline::GetProcessedFrames() const { return mProcessedFrames; }

/**
 * @brief Dump pipline info in Log
 *
 */
void AlgoPipeline::Dump() {
  if (mAlgos.size() == 0) {
    LOG(VERBOSE, ALGOPIPELINE, "No algos to dump");
    return;
  }

  LOG(VERBOSE, ALGOPIPELINE, "--------AlgoPipeline Dump[%p]--------", this);
  LOG(VERBOSE, ALGOPIPELINE, "Number of Algos: %ld", mAlgos.size());
  for (auto algo : mAlgos) {
    LOG(VERBOSE, ALGOPIPELINE, "Algo ID: %x", (unsigned int)algo->GetAlgoId());
    LOG(VERBOSE, ALGOPIPELINE, "Algo Name: %s",
        algo->GetAlgorithmName().c_str());
    LOG(VERBOSE, ALGOPIPELINE, "Algo State: %s",
        algo->GetStatusString().c_str());
  }
  LOG(VERBOSE, ALGOPIPELINE, "Processed Frames: %ld", mProcessedFrames);
  LOG(VERBOSE, ALGOPIPELINE, "--------Pipeline State: %d--------",
      (int)GetState());
}
