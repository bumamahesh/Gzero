
#include "AlgoPipeline.h"
#include <assert.h>

/**
@brief Constructs a new AlgoPipeline object with a list of algorithm IDs
 *
 * @param algoList
 */
AlgoPipeline::AlgoPipeline(std::vector<size_t> algoList) {
  assert(algoList.size() != 0);
  _algoListId = algoList;

  _algoNodeMgr = std::make_shared<AlgoNodeManager>(AlgosPath);
  assert(_algoNodeMgr != nullptr);

  for (auto algoId : _algoListId) {
    auto algo = _algoNodeMgr->CreateAlgo(algoId);
    assert(algo != nullptr);
    algo->SetNotifyEvent(AlgoPipeline::NodeEventHandler);
    _algos.push_back(algo);
    _algoListName.push_back(algo->GetAlgorithmName());
  }
}

/**
@brief Constructs a new AlgoPipeline object with a list of algorithm names
 *
 * @param algoList
 */
AlgoPipeline::AlgoPipeline(std::vector<std::string> algoList) {
  assert(algoList.size() != 0);
  _algoListName = algoList;

  _algoNodeMgr = std::make_shared<AlgoNodeManager>(AlgosPath);
  assert(_algoNodeMgr != nullptr);

  for (auto algoName : _algoListName) {
    auto algo = _algoNodeMgr->CreateAlgo(algoName);
    assert(algo != nullptr);
    algo->SetNotifyEvent(AlgoPipeline::NodeEventHandler);
    _algos.push_back(algo);
    _algoListId.push_back(algo->GetAlgoId());
  }
}

/**
@brief Destroy the Algo Pipeline:: Algo Pipeline object
 *
 */
AlgoPipeline::~AlgoPipeline() {
  _algos.clear();
  _algoListId.clear();
  _algoListName.clear();
}

void AlgoPipeline::Process(std::string &input) {

  if (_algos.size() == 0) {
    std::cout << "No algos to process" << std::endl;
    return;
  }
  _algos[0]->EnqueueRequest(
      input); // put on first request rest will be done by the first algo
}

void AlgoPipeline::NodeEventHandler() {
  // @todo

  // std::cout << "NodeEventHandler" << std::endl;
}