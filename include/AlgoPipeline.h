
#if 0
#ifndef ALGO_PIPELINE_H
#define ALGO_PIPELINE_H

#include "AlgoBase.h"
#include "AlgoNodeManager.h"
#include <memory>
#include <vector>

class AlgoPipeline {
public:
  AlgoPipeline(const std::string &libraryPath);
  ~AlgoPipeline();

  bool AddAlgo(size_t algoId);
  bool RemoveAlgo(size_t algoId);
  bool Process();

private:
  AlgoNodeManager algoNodeMgr_;
  std::vector<std::shared_ptr<AlgoBase>> algos_;
};
#endif // ALGO_PIPELINE_H
#endif