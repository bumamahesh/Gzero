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
#include "AlgoSession.h"
#include "Log.h"
#include <cassert>

/**
 * @brief Construct a new Algo Session:: Algo Session object
 *
 */
AlgoSession::AlgoSession(INTERFACECALLBACK pInterfaceCallBackHandler,
                         void *pCtx) {
  this->pInterfaceCallBackHandler = pInterfaceCallBackHandler;
  this->pInterfaceCtx = pCtx;
}

/**
 * @brief Destroy the Algo Session:: Algo Session object
 *
 */
AlgoSession::~AlgoSession() { SessionStop(); }

/**
 * @brief Stop Session
 *
 * @return true
 * @return false
 */
bool AlgoSession::SessionStop() {
  LOG(INFO, ALGOSESSION, "AlgoSession::SessionStop E");
  for (auto &pipeline : mPipelines) {
    pipeline->WaitForQueueCompetion();
  }
  mPipelines.clear();
  mPipelineMap.clear();
  mNextPipelineId = 0;
  LOG(INFO, ALGOSESSION, "AlgoSession::SessionStop X");
  return true;
}

/**
 * @brief Add Pipeline
 *
 * @param pipeline
 * @return true
 * @return false
 */
bool AlgoSession::SessionAddPipeline(std::shared_ptr<AlgoPipeline> &pipeline) {
  LOG(VERBOSE, ALGOSESSION, "AlgoSession::SessionAddPipeline E");
  if (pipeline == nullptr) {
    return false;
  }
  size_t pipelineId = mNextPipelineId++;
  mPipelines.push_back(pipeline);
  mPipelineMap[pipelineId] = pipeline;
  LOG(VERBOSE, ALGOSESSION, "AlgoSession::SessionAddPipeline X");
  return true;
}

/**
 * @brief Remove Pipeline
 *
 * @param pipelineId
 * @return true
 * @return false
 */
bool AlgoSession::SessionRemovePipeline(size_t pipelineId) {
  auto it = mPipelineMap.find(pipelineId);
  if (it == mPipelineMap.end()) {
    return false;
  }
  mPipelineMap.erase(it);
  for (size_t i = 0; i < mPipelines.size(); ++i) {
    if (mPipelines[i] == it->second) {
      mPipelines.erase(mPipelines.begin() + i);
      break;
    }
  }
  return true;
}

/**
 * @brief Processing Input Request
 *
 * @param input
 * @return true
 * @return false
 */
bool AlgoSession::SessionProcess(std::shared_ptr<AlgoRequest> input) {
  LOG(INFO, ALGOSESSION, "AlgoSession::SessionProcess E");
  std::vector<AlgoId> algoList = SessionGetAlgoList();
  int pipelineId = SessionGetpipelineId(algoList);
  if (pipelineId == -1) {
    auto lPipeline = std::make_shared<AlgoPipeline>(
        &AlgoSession::PiplineCallBackHandler, this);
    lPipeline->ConfigureAlgoPipeline(algoList);
    if (lPipeline->GetState() != AlgoPipelineState::ConfiguredWithId) {
      LOG(ERROR, ALGOSESSION, "Failed to Configure Pipeline");
      return false;
    }
    SessionAddPipeline(lPipeline);
    pipelineId = 0;
  }

  SessionProcess(pipelineId, input);
  LOG(INFO, ALGOSESSION, "AlgoSession::SessionProcess X");
  return true;
}

/**
 * @brief Process
 *
 * @param pipelineId
 * @param input
 * @return true
 * @return false
 */
bool AlgoSession::SessionProcess(size_t pipelineId,
                                 std::shared_ptr<AlgoRequest> input) {

  LOG(INFO, ALGOSESSION, "AlgoSession::SessionProcess E");
  auto it = mPipelineMap.find(pipelineId);
  if (it == mPipelineMap.end()) {
    return false;
  }
  it->second->Process(input);
  LOG(INFO, ALGOSESSION, "AlgoSession::SessionProcess X");
  return true;
}

/**
 * @brief   Get Pipeline Count
 *
 * @return size_t
 */
size_t AlgoSession::SessionGetPipelineCount() const {
  return mPipelines.size();
}

/**
 * @brief   Get PiplineIds
 *
 * @return std::vector<size_t>
 */
std::vector<size_t> AlgoSession::SessionGetPipelineIds() const {
  std::vector<size_t> ids;
  for (const auto &pair : mPipelineMap) {
    ids.push_back(pair.first);
  }
  return ids;
}

/**
 * @brief   Get Algo List form Decision list
 *
 * @return std::vector<size_t>
 */
std::vector<AlgoId> AlgoSession::SessionGetAlgoList() {
  // pAlgoInterface->GetAlgoList();
  /**get object of Decisionmanager and get a algo list  */
  static int count = 0;
  std::vector<AlgoId> algoList;
#if 0

  // algoList.push_back(ALGO_MANDELBROTSET);
  algoList.push_back(ALGO_FILTER);

#else
  if (count % 3 == 0) {
    algoList.push_back(ALGO_HDR);
    algoList.push_back(ALGO_BOKEH);
  } else if (count % 3 == 1) {
    algoList.push_back(ALGO_HDR);
  } else {
    algoList.push_back(ALGO_BOKEH);
  }
#endif
  count++;
  return algoList;
}

/**
 * @brief Get Pipeline id
 *
 * @return int
 */
int AlgoSession::SessionGetpipelineId(std::vector<AlgoId> algoList) {
  if (mPipelines.size() == 0) {
    return -1;
  }
  int pipelineId = 0;
  for (auto pipeline : mPipelines) {
    if (pipeline->GetAlgoListId() == algoList) {
      return pipelineId;
    }
    pipelineId++;
  }
  if (pipelineId == (int)mPipelines.size()) {
    return -1;
  }
  return pipelineId;
}

/**
 * @brief  Get pipline from pipline id
 *
 * @param pipelineId
 * @return std::shared_ptr<AlgoPipeline>
 */
std::shared_ptr<AlgoPipeline>
AlgoSession::SessionGetPipeline(size_t pipelineId) {
  auto it = mPipelineMap.find(pipelineId);
  if (it == mPipelineMap.end()) {
    return nullptr;
  }
  return it->second;
}

/**
 * @brief Pipline CallBack handler
 *
 * @param input
 */
void AlgoSession::PiplineCallBackHandler(void *pctx,
                                         std::shared_ptr<AlgoRequest> input) {
  AlgoSession *pSession = static_cast<AlgoSession *>(pctx);
  if (pSession) {
    if (pSession->pInterfaceCallBackHandler) {
      std::lock_guard<std::mutex> lock(pSession->mCallbackMutex);
      pSession->pInterfaceCallBackHandler(pSession->pInterfaceCtx, input);
    }
  }
}

/**
 * @brief Dump Session Info in logs
 *
 */
void AlgoSession::Dump() {
  LOG(VERBOSE, ALGOSESSION, "AlgoSession Dump");
  LOG(VERBOSE, ALGOSESSION, "Number of Pipelines: %ld", mPipelines.size());
  for (size_t i = 0; i < mPipelines.size(); ++i) {
    LOG(VERBOSE, ALGOSESSION, "Pipeline ID: %ld::%p", i, mPipelines[i].get());
    mPipelines[i]->Dump();
  }
}
