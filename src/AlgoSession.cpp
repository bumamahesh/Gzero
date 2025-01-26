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
  for (auto &pipeline : mPipelines) {
    pipeline->WaitForQueueCompetion();
  }
  mPipelines.clear();
  mPipelineMap.clear();
  mNextPipelineId = 0;
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
  if (pipeline == nullptr) {
    return false;
  }
  size_t pipelineId = mNextPipelineId++;
  mPipelines.push_back(pipeline);
  mPipelineMap[pipelineId] = pipeline;
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
  auto it = mPipelineMap.find(pipelineId);
  if (it == mPipelineMap.end()) {
    return false;
  }
  it->second->Process(input);
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
  if (count % 3 == 0) {
    algoList.push_back(ALGO_HDR);
    algoList.push_back(ALGO_BOKEH);
  } else if (count % 3 == 1) {
    algoList.push_back(ALGO_HDR);
  } else {
    algoList.push_back(ALGO_BOKEH);
  }
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
