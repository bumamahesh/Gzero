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

  void Process(std::string &input);
  static void NodeEventHandler(void *,
                               std::shared_ptr<AlgoBase::ALGOCALLBACKMSG>);

private:
  std::shared_ptr<AlgoNodeManager> m_algoNodeMgr;
  std::vector<std::shared_ptr<AlgoBase>> m_algos;

  std::vector<size_t> m_algoListId;
  std::vector<std::string> m_algoListName;

  std::string AlgosPath = "/home/uma/workspace/Gzero/build/Algos/Hdr/";
  // "@todo get AlgosPath from xml later"
};
#endif // ALGO_PIPELINE_H
