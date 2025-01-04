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
  void WaitForQueueCompetion();
  size_t GetProcessedFrames() const;

private:
  std::shared_ptr<AlgoNodeManager> mAlgoNodeMgr;
  std::vector<std::shared_ptr<AlgoBase>> mAlgos;

  std::vector<size_t> mAlgoListId;
  std::vector<std::string> mAlgoListName;

  size_t mProcessedFrames = 0;

  std::string AlgosPath = "/home/uma/workspace/Gzero/cmake/lib/";
  // "@todo get AlgosPath from xml later"
};
#endif // ALGO_PIPELINE_H
