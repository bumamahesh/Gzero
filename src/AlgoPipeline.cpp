#if 0
#include "AlgoPipeline.h"

AlgoPipeline::AlgoPipeline(const std::string &libraryPath)
    : algoNodeMgr_(libraryPath) {}

AlgoPipeline::~AlgoPipeline() {}

bool AlgoPipeline::AddAlgo(size_t algoId) {
  auto algo = algoNodeMgr_.CreateAlgo(algoId);
  if (algo == nullptr) {
    return false;
  }
  algos_.push_back(algo);
  return true;
}

bool AlgoPipeline::RemoveAlgo(size_t algoId) {
  for (auto it = algos_.begin(); it != algos_.end(); ++it) {
    if ((*it)->GetId() == algoId) {
      algos_.erase(it);
      return true;
    }
  }
  return false;
}

bool AlgoPipeline::Process() {
  for (auto &algo : algos_) {
    algo->Process();
  }
  return true;
}
#endif