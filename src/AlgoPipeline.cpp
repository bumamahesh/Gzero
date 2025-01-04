#include "AlgoPipeline.h"
#include "Log.h"
#include <assert.h>
/**
@brief Constructs a new AlgoPipeline object with a list of algorithm IDs
 *
 * @param algoList
 */
AlgoPipeline::AlgoPipeline(std::vector<size_t> algoList) {
  assert(algoList.size() != 0);
  m_algoListId = algoList;
  m_processedFrames = 0;
  m_algoNodeMgr = std::make_shared<AlgoNodeManager>(AlgosPath);
  assert(m_algoNodeMgr != nullptr);

  std::shared_ptr<AlgoBase> previousAlgo = nullptr;
  for (auto algoId : m_algoListId) {
    auto algo = m_algoNodeMgr->CreateAlgo(algoId);
    assert(algo != nullptr);
    algo->SetNotifyEvent(AlgoPipeline::NodeEventHandler);
    algo->m_pipCtx = (void *)this;
    m_algos.push_back(algo);
    m_algoListName.push_back(algo->GetAlgorithmName());
    if (previousAlgo) {
      previousAlgo->SetNextAlgo(algo);
    }
    previousAlgo = algo;
  }
}

/**
@brief Constructs a new AlgoPipeline object with a list of algorithm names
 *
 * @param algoList
 */
AlgoPipeline::AlgoPipeline(std::vector<std::string> algoList) {
  assert(algoList.size() != 0);
  m_algoListName = algoList;
  m_processedFrames = 0;
  m_algoNodeMgr = std::make_shared<AlgoNodeManager>(AlgosPath);
  assert(m_algoNodeMgr != nullptr);

  std::shared_ptr<AlgoBase> previousAlgo = nullptr;
  for (auto algoName : m_algoListName) {
    auto algo = m_algoNodeMgr->CreateAlgo(algoName);
    assert(algo != nullptr);
    algo->SetNotifyEvent(AlgoPipeline::NodeEventHandler);
    m_algos.push_back(algo);
    m_algoListId.push_back(algo->GetAlgoId());
    if (previousAlgo) {
      previousAlgo->SetNextAlgo(algo);
    }
    previousAlgo = algo;
  }
}

/**
@brief Destroy the Algo Pipeline:: Algo Pipeline object
 *
 */
AlgoPipeline::~AlgoPipeline() {
  for (auto &algo : m_algos) {
    algo->WaitForQueueCompetion();
  }
  m_algos.clear();
  m_algoListId.clear();
  m_algoListName.clear();
}

void AlgoPipeline::Process(std::string &input) {

  if (m_algos.size() == 0) {
    LOG(ERROR, ALGOPIPELINE, "No algos to process");
    return;
  }
  std::shared_ptr<Task_t> task = std::make_shared<Task_t>();
  task->args = new std::string(
      input); // for now request is just string
              // put on first request rest will be done by the first algo
  m_algos[0]->EnqueueRequest(task);
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
  assert(algo->m_pipCtx != nullptr);
  switch (msg->type) {
  case AlgoBase::ALGO_PROCESSING_COMPLETED: {
    LOG(VERBOSE, ALGOPIPELINE, "Processing Completed");
    std::shared_ptr<AlgoBase> NextAlgo = algo->GetNextAlgo().lock();
    if (NextAlgo) {
      NextAlgo->EnqueueRequest(msg->request);
    } else {
      /*last node so let free obj */
      std::string *input = reinterpret_cast<std::string *>(msg->request->args);
      delete input;
      auto plPipeline = reinterpret_cast<AlgoPipeline *>(algo->m_pipCtx);
      assert(plPipeline != nullptr);
      plPipeline->m_processedFrames++;
    }
  }
  /*kick next node */
  break;
  case AlgoBase::ALGO_PROCESSING_FAILED:
    LOG(ERROR, ALGOPIPELINE, "Processing Failed");
    break;
  case AlgoBase::ALGO_PROCESSING_TIMEOUT:
    LOG(ERROR, ALGOPIPELINE, "Processing Timeout");
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

  for (auto &algo : m_algos) {
    algo->WaitForQueueCompetion();
  }
}

/**
 * @brief Get Processed Frames
 *
 * @return size_t
 */
size_t AlgoPipeline::GetProcessedFrames() const { return m_processedFrames; }