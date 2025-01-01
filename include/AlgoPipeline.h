#ifndef ALGO_PIPELINE_H
#define ALGO_PIPELINE_H

#include "AlgoBase.h"
#include "AlgoNodeManager.h"
#include <memory>
#include <vector>

class AlgoPipeline {
public:
  AlgoPipeline(std::vector<size_t> algoList);
  AlgoPipeline(std::vector<std::string> algoList);
  ~AlgoPipeline();

private:
  std::shared_ptr<AlgoNodeManager> _algoNodeMgr;
  std::vector<std::shared_ptr<AlgoBase>> _algos;

  std::vector<size_t> _algoListId;
  std::vector<std::string> _algoListName;

  std::string AlgosPath = "/home/uma/workspace/Gzero/build/Algos/Hdr/";
  // "@todo get AlgosPath from xml later"
};
#endif // ALGO_PIPELINE_H
