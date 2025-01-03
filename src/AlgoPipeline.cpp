
#include "AlgoPipeline.h"
#include <assert.h>

/**
@brief Constructs a new AlgoPipeline object with a list of algorithm IDs
 *
 * @param algoList
 */
AlgoPipeline::AlgoPipeline(std::vector<size_t> algoList) {
  assert(algoList.size() != 0);
  m_algoListId = algoList;

  m_algoNodeMgr = std::make_shared<AlgoNodeManager>(AlgosPath);
  assert(m_algoNodeMgr != nullptr);

  std::shared_ptr<AlgoBase> previousAlgo = nullptr;
  for (auto algoId : m_algoListId) {
    auto algo = m_algoNodeMgr->CreateAlgo(algoId);
    assert(algo != nullptr);
    algo->SetNotifyEvent(AlgoPipeline::NodeEventHandler);
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
    std::cout << "No algos to process" << std::endl;
    return;
  }
  std::shared_ptr<Task_t> task = std::make_shared<Task_t>();
  task->ctx = std::static_pointer_cast<void>(m_algos[0]);
  task->args = new std::string(
      input); // for now request is just string
              // put on first request rest will be done by the first algo
  m_algos[0]->EnqueueRequest(task);
}

void AlgoPipeline::NodeEventHandler(
    std::shared_ptr<void> ctx, std::shared_ptr<AlgoBase::ALGOCALLBACKMSG> msg) {
  assert(msg != nullptr);
  assert(ctx != nullptr);
  std::shared_ptr<AlgoBase> algo = std::static_pointer_cast<AlgoBase>(ctx);
  assert(algo != nullptr);

  switch (msg->type) {
  case AlgoBase::ALGO_PROCESSING_COMPLETED: {
    // std::cout << "AlgoPipeline::NodeEventHandler: Processing Completed"<<
    // std::endl;*/
    std::shared_ptr<AlgoBase> NextAlgo = algo->GetNextAlgo().lock();
    if (NextAlgo) {
      msg->request->ctx = std::static_pointer_cast<void>(NextAlgo);
      NextAlgo->EnqueueRequest(msg->request);
    } else {
      /*last node so let free obj */
      std::string *input = reinterpret_cast<std::string *>(msg->request->args);
      delete input;
      // std::cout << "AlgoPipeline::NodeEventHandler: No Next Algo" <<
      // std::endl;*/
    }
  }
  /*kick next node */
  break;
  case AlgoBase::ALGO_PROCESSING_FAILED:
    std::cout << "AlgoPipeline::NodeEventHandler: Processing Failed"
              << std::endl;
    break;
  case AlgoBase::ALGO_PROCESSING_TIMEOUT:
    std::cout << "AlgoPipeline::NodeEventHandler: Processing Timeout"
              << std::endl;
    break;
  case AlgoBase::ALGO_PROCESSING_PARTIAL:
    std::cout << "AlgoPipeline::NodeEventHandler: Partial Processing Completed"
              << std::endl;
    break;
  default:
    std::cout << "AlgoPipeline::NodeEventHandler: Unknown Message Type"
              << std::endl;
    break;
  }
}