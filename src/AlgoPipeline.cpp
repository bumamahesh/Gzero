/*
 * Copyright (c) [2025] [Uma Mahesh B]
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#include "AlgoPipeline.h"
#include <assert.h>
#include "Log.h"
/**
@brief Constructs a new AlgoPipeline object with a list of algorithm IDs
 *
 * @param algoList
 */
AlgoPipeline::AlgoPipeline(SESSIONCALLBACK pSesionCallBackHandler, void* pCtx) {
  LOG(INFO, ALGOPIPELINE, "AlgoPipeline::AlgoPipeline E");
  mProcessedFrames             = 0;
  mState                       = AlgoPipelineState::Initialised;
  this->pSesionCallBackHandler = pSesionCallBackHandler;
  this->pSessionCtx            = pCtx;
  pEventHandlerThread =
      std::make_shared<EventHandlerThread<AlgoBase::AlgoCallbackMessage>>(
          AlgoPipeline::NodeEventHandler, this);
  LOG(INFO, ALGOPIPELINE, "AlgoPipeline::AlgoPipeline X");
}

/**
 * @brief Destroy the Algo Pipeline:: Algo Pipeline object
 *
 */
AlgoPipeline::~AlgoPipeline() {
  LOG(INFO, ALGOPIPELINE, "AlgoPipeline::~AlgoPipeline E");
  for (auto& algo : mAlgos) {
    algo->WaitForQueueCompetion();
  }
  pEventHandlerThread->stop();
  mAlgos.clear();
  mAlgoListId.clear();
  mAlgoListName.clear();
  LOG(INFO, ALGOPIPELINE, "AlgoPipeline::~AlgoPipeline X");
}

/**
 * @brief  Get state of Pipeline
 *
 * @return AlgoPipelineState
 */
AlgoPipelineState AlgoPipeline::GetState() const {
  return mState;
}

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
std::vector<AlgoId> AlgoPipeline::GetAlgoListId() const {
  return mAlgoListId;
}

/**
 * @brief  Configure Pipeline with Provided algo List
 *
 * @param algoList
 * @return AlgoPipelineState
 */
AlgoPipelineState AlgoPipeline::ConfigureAlgoPipeline(
    std::vector<AlgoId>& algoList) {

  LOG(INFO, ALGOPIPELINE, "Configuring AlgoPipeline :: %ld ", algoList.size());

  if (GetState() == AlgoPipelineState::Initialised) {
    if (algoList.size() == 0) {
      LOG(ERROR, ALGOPIPELINE, "AlgoList is empty");
      return SetState(AlgoPipelineState::FailedToConfigure);
    }
    mAlgoListId      = algoList;
    mProcessedFrames = 0;
    mAlgoNodeMgr     = &AlgoNodeManager::Getinstance();
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
      previousAlgo     = algo;
      mAlgoMap[algoId] = algo;
    }
    previousAlgo->bIslastNode = true;  // lets mark last  node
    SetState(AlgoPipelineState::ConfiguredWithId);
  } else {
    LOG(ERROR, ALGOPIPELINE,
        "AlgoPipeline is not Currect State to Configure ::%d", (int)GetState());
  }
  LOG(INFO, ALGOPIPELINE, "AlgoPipeline::ConfigureAlgoPipeline X");
  return GetState();
}

/**
 * @brief  Configure Pipeline with Provided algo List
 *
 * @param algoList
 * @return AlgoPipelineState
 */
AlgoPipelineState AlgoPipeline::ConfigureAlgoPipeline(
    std::vector<std::string>& algoList) {

  LOG(VERBOSE, ALGOPIPELINE, "Configuring AlgoPipeline :: %ld ",
      algoList.size());
  if (GetState() == AlgoPipelineState::Initialised) {
    if (algoList.size() == 0) {
      LOG(ERROR, ALGOPIPELINE, "AlgoList is empty");
      return SetState(AlgoPipelineState::FailedToConfigure);
    }
    mAlgoListName    = algoList;
    mProcessedFrames = 0;
    mAlgoNodeMgr     = &AlgoNodeManager::Getinstance();
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
      previousAlgo                = algo;
      mAlgoMap[algo->GetAlgoId()] = algo;
    }
    previousAlgo->bIslastNode = true;  // lets mark last  node
    SetState(AlgoPipelineState::ConfiguredWithName);
  } else {
    LOG(ERROR, ALGOPIPELINE, "AlgoPipeline is not Currect State to Configure");
  }
  LOG(VERBOSE, ALGOPIPELINE, "AlgoPipeline::ConfigureAlgoPipeline X");
  return GetState();
}

/**
 * @brief Process Request on Pipeline
 *
 * @param input
 */
void AlgoPipeline::Process(std::shared_ptr<AlgoRequest> input) {
  LOG(INFO, ALGOPIPELINE, "AlgoPipeline::Process E");
  if (mAlgos.size() == 0) {
    // LOG(ERROR, ALGOPIPELINE, "No algos to process");
    return;
  }
  if (GetState() == AlgoPipelineState::ConfiguredWithName ||
      GetState() == AlgoPipelineState::ConfiguredWithId) {
    std::shared_ptr<Task_t> task = std::make_shared<Task_t>();
    task->request                = input;
    {
      std::lock_guard<std::mutex> lock(mRequesteMapMutex);
      if (mRequesteMap.find(input->mRequestId) == mRequesteMap.end()) {
        /* LOG(VERBOSE, ALGOPIPELINE, "Added for Processing  Request ID:
           %d::%p", input->mRequestId, input.get());*/
        mRequesteMap.insert({input->mRequestId, input});
      } else {
        LOG(ERROR, ALGOPIPELINE, "Error Duplicate Request ID: %d::%p",
            input->mRequestId, (void*)input.get());
      }
    }
    task->timeoutMs = mAlgos[0]->GetTimeout();
    mAlgos[0]->EnqueueRequest(task);
    LOG(INFO, ALGOPIPELINE, "Request Enqueded on ::%s",
        mAlgos[0]->GetAlgorithmName().c_str());
  } else {
    LOG(ERROR, ALGOPIPELINE, "AlgoPipeline is not Currect State to Process");
  }
  LOG(INFO, ALGOPIPELINE, "AlgoPipeline::Process X");
}

/**
 * @brief  Node Event Handler
 *
 * @param ctx
 * @param msg
 */
void AlgoPipeline::NodeEventHandler(
    void* ctx, std::shared_ptr<AlgoBase::AlgoCallbackMessage> msg) {
  // static lockguard
  // static std::mutex mCallbackMutex;
  // std::lock_guard<std::mutex> lock(mCallbackMutex);
  assert(msg != nullptr);
  assert(ctx != nullptr);
  auto plPipeline = reinterpret_cast<AlgoPipeline*>(ctx);
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
        /**fecth and update timeout for processing this request on Next algo */
        msg->mRequest->timeoutMs = NextAlgo->GetTimeout();
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
        plPipeline->mCondition.notify_all();  // Notify waiting threads
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
  LOG(VERBOSE, ALGOPIPELINE, "AlgoPipeline::WaitForQueueCompetion E");
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
    LOG(ERROR, ALGOPIPELINE, "Waiting... %ld requests still pending.",
        mRequesteMap.size());
  }

  /*
    for (auto &algo : mAlgos) {

      algo->WaitForQueueCompetion();
    }
    */
  LOG(VERBOSE, ALGOPIPELINE, "AlgoPipeline::WaitForQueueCompetion X");
}

/**
 * @brief Get Processed Frames
 *
 * @return size_t
 */
size_t AlgoPipeline::GetProcessedFrames() const {
  return mProcessedFrames;
}

/**
 * @brief Dump pipline info in Log
 *
 */
void AlgoPipeline::Dump() {
  if (mAlgos.size() == 0) {
    LOG(VERBOSE, ALGOPIPELINE, "No algos to dump");
    return;
  }

  LOG(VERBOSE, ALGOPIPELINE, "--------AlgoPipeline Dump[%p]--------",
      (void*)this);
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
