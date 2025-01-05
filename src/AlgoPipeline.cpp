#include "AlgoPipeline.h"
#include "Log.h"
#include <assert.h>
/**
@brief Constructs a new AlgoPipeline object with a list of algorithm IDs
 *
 * @param algoList
 */
AlgoPipeline::AlgoPipeline() {
  mProcessedFrames = 0;
  mState = ALGOPIPELINESTATE::INITIALISED;
}

/**
 * @brief Destroy the Algo Pipeline:: Algo Pipeline object
 *
 */
AlgoPipeline::~AlgoPipeline() {
  for (auto &algo : mAlgos) {
    algo->WaitForQueueCompetion();
  }
  mAlgos.clear();
  mAlgoListId.clear();
  mAlgoListName.clear();
}

/**
 * @brief  Get state of Pipeline
 *
 * @return ALGOPIPELINESTATE
 */
ALGOPIPELINESTATE AlgoPipeline::GetState() const { return mState; }

/**
 * @brief  Set state of pipeline
 *
 * @param state
 * @return ALGOPIPELINESTATE
 */
ALGOPIPELINESTATE AlgoPipeline::SetState(ALGOPIPELINESTATE state) {
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
 * @return ALGOPIPELINESTATE
 */
ALGOPIPELINESTATE
AlgoPipeline::ConfigureAlgoPipeline(std::vector<AlgoId> &algoList) {

  LOG(VERBOSE, ALGOPIPELINE, "Configuring AlgoPipeline :: %ld",
      algoList.size());

  if (GetState() == ALGOPIPELINESTATE::INITIALISED) {
    if (algoList.size() == 0) {
      LOG(ERROR, ALGOPIPELINE, "AlgoList is empty");
      return SetState(FAILED_TO_CONFIGURE);
    }
    mAlgoListId = algoList;
    mProcessedFrames = 0;
    mAlgoNodeMgr = &AlgoNodeManager::Getinstance();
    assert(mAlgoNodeMgr != nullptr);

    std::shared_ptr<AlgoBase> previousAlgo = nullptr;
    for (auto algoId : mAlgoListId) {
      auto algo = mAlgoNodeMgr->CreateAlgo(algoId);
      if (algo == nullptr) {
        return SetState(FAILED_TO_CONFIGURE);
      }
      algo->SetNotifyEvent(AlgoPipeline::NodeEventHandler);
      algo->pPipelineCtx = (void *)this;
      mAlgos.push_back(algo);
      mAlgoListName.push_back(algo->GetAlgorithmName());
      if (previousAlgo) {
        previousAlgo->SetNextAlgo(algo);
      }
      previousAlgo = algo;
    }
    SetState(CONFIGURED_WITH_ID);
  } else {
    LOG(ERROR, ALGOPIPELINE, "AlgoPipeline is not Currect State to Configure");
  }
  return GetState();
}

/**
 * @brief  Configure Pipeline with Provided algo List
 *
 * @param algoList
 * @return ALGOPIPELINESTATE
 */
ALGOPIPELINESTATE
AlgoPipeline::ConfigureAlgoPipeline(std::vector<std::string> &algoList) {

  LOG(VERBOSE, ALGOPIPELINE, "Configuring AlgoPipeline :: %ld",
      algoList.size());
  if (GetState() == ALGOPIPELINESTATE::INITIALISED) {
    if (algoList.size() == 0) {
      LOG(ERROR, ALGOPIPELINE, "AlgoList is empty");
      return SetState(FAILED_TO_CONFIGURE);
    }
    mAlgoListName = algoList;
    mProcessedFrames = 0;
    mAlgoNodeMgr = &AlgoNodeManager::Getinstance();
    assert(mAlgoNodeMgr != nullptr);

    std::shared_ptr<AlgoBase> previousAlgo = nullptr;
    for (auto algoName : mAlgoListName) {
      auto algo = mAlgoNodeMgr->CreateAlgo(algoName);
      if (algo == nullptr) {
        return SetState(FAILED_TO_CONFIGURE);
      }
      algo->SetNotifyEvent(AlgoPipeline::NodeEventHandler);
      mAlgos.push_back(algo);
      mAlgoListId.push_back(algo->GetAlgoId());
      if (previousAlgo) {
        previousAlgo->SetNextAlgo(algo);
      }
      previousAlgo = algo;
    }
    SetState(CONFIGURED_WITH_NAME);
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
void AlgoPipeline::Process(std::string &input) {

  if (mAlgos.size() == 0) {
    // LOG(ERROR, ALGOPIPELINE, "No algos to process");
    return;
  }
  if (GetState() == CONFIGURED_WITH_NAME || GetState() == CONFIGURED_WITH_ID) {
    std::shared_ptr<Task_t> task = std::make_shared<Task_t>();
    task->args = new std::string(
        input); // for now request is just string
                // put on first request rest will be done by the first algo
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
    void *ctx, std::shared_ptr<AlgoBase::ALGOCALLBACKMSG> msg) {
  assert(msg != nullptr);
  assert(ctx != nullptr);
  auto algo = static_cast<AlgoBase *>(ctx);
  assert(algo != nullptr);
  assert(algo->pPipelineCtx != nullptr);
  auto plPipeline = reinterpret_cast<AlgoPipeline *>(algo->pPipelineCtx);
  assert(plPipeline != nullptr);
  switch (msg->mType) {
  case AlgoBase::ALGO_PROCESSING_COMPLETED: {
    LOG(VERBOSE, ALGOPIPELINE, "Processing Completed");
    std::shared_ptr<AlgoBase> NextAlgo = algo->GetNextAlgo().lock();
    if (NextAlgo) {
      NextAlgo->EnqueueRequest(msg->mRequest);
    } else {
      /*last node so let free obj */
      std::string *input = reinterpret_cast<std::string *>(msg->mRequest->args);
      delete input;
      /*we need to put the update on new thread for now this is last node thread
       which hasupdated this info do not prpogare furthure on this same thread ,
       lets stop here and put loadf on new thread which is of pipeline or
       session
       */
      plPipeline->mProcessedFrames++;
    }
  }
  /*kick next node */
  break;
  case AlgoBase::ALGO_PROCESSING_FAILED:
    LOG(ERROR, ALGOPIPELINE, "Processing Failed");
    plPipeline->mState = FAILED_TO_PROCESS;
    break;
  case AlgoBase::ALGO_PROCESSING_TIMEOUT:
    LOG(ERROR, ALGOPIPELINE, "Processing Timeout");
    plPipeline->mState = FAILED_TO_PROCESS;
    break;
  case AlgoBase::ALGO_PROCESSING_PARTIAL:
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

  for (auto &algo : mAlgos) {
    algo->WaitForQueueCompetion();
  }
}

/**
 * @brief Get Processed Frames
 *
 * @return size_t
 */
size_t AlgoPipeline::GetProcessedFrames() const { return mProcessedFrames; }