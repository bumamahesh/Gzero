#include "AlgoSession.h"
#include "Log.h"
#include <cassert>

/**
 * @brief Construct a new Algo Session:: Algo Session object
 *
 */
AlgoSession::AlgoSession() {}

/**
 * @brief Destroy the Algo Session:: Algo Session object
 *
 */
AlgoSession::~AlgoSession() { Stop(); }

/**
 * @brief  Initialize with Config
 *
 * @param config
 * @return true
 * @return false
 */
bool AlgoSession::Initialize(std::string &config) {
  LOG(INFO, ALGOSESSION, "AlgoSession Initialized");

  return true;
}

/**
 * @brief Start Session
 *
 * @return true
 * @return false
 */
bool AlgoSession::Start() { return true; }

/**
 * @brief Stop Session
 *
 * @return true
 * @return false
 */
bool AlgoSession::Stop() {
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
bool AlgoSession::AddPipeline(std::shared_ptr<AlgoPipeline> &pipeline) {
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
bool AlgoSession::RemovePipeline(size_t pipelineId) {
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
 * @brief
 *
 * @param input
 * @return true
 * @return false
 */
bool AlgoSession::Process(std::string &input) {
  std::vector<AlgoId> algoList = GetAlgoList();
  int pipelineId = GetpipelineId(algoList);
  if (pipelineId == -1) {
    auto lPipeline = std::make_shared<AlgoPipeline>();
    lPipeline->ConfigureAlgoPipeline(algoList);
    if (lPipeline->GetState() != ALGOPIPELINESTATE::CONFIGURED_WITH_ID) {
      LOG(ERROR, ALGOSESSION, "Failed to Configure Pipeline");
      return false;
    }
    AddPipeline(lPipeline);
    pipelineId = 0;
  }

  Process(pipelineId, input);

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
bool AlgoSession::Process(size_t pipelineId, std::string &input) {
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
size_t AlgoSession::GetPipelineCount() const { return mPipelines.size(); }

/**
 * @brief   Get PiplineIds
 *
 * @return std::vector<size_t>
 */
std::vector<size_t> AlgoSession::GetPipelineIds() const {
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
std::vector<AlgoId> AlgoSession::GetAlgoList() { return {}; }

/**
 * @brief Get Pipeline id
 *
 * @return int
 */
int AlgoSession::GetpipelineId(std::vector<AlgoId> algoList) {
  if (mPipelines.size() > 0) {
    return -1;
  }
  int pipelineId = 0;
  for (auto pipeline : mPipelines) {
    if (pipeline->GetState() == ALGOPIPELINESTATE::CONFIGURED_WITH_ID) {
      if (pipeline->GetAlgoListId() == algoList) {
        return pipelineId;
      }
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
std::shared_ptr<AlgoPipeline> AlgoSession::GetPipeline(size_t pipelineId) {
  auto it = mPipelineMap.find(pipelineId);
  if (it == mPipelineMap.end()) {
    return nullptr;
  }
  return it->second;
}