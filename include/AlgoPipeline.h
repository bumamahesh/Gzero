#ifndef ALGO_PIPELINE_H
#define ALGO_PIPELINE_H

#include "AlgoBase.h"
#include "AlgoDefs.h"
#include "AlgoNodeManager.h"
#include <memory>
#include <vector>

typedef enum AlgoPipelineState {
  NOT_INITIALISED = 0,
  INITIALISED,
  CONFIGURED_WITH_ID,
  CONFIGURED_WITH_NAME,
  INVALID_ALGO_LIST,
  FAILED_TO_CONFIGURE,
  SUCESSFULL,
  FAILED_TO_PROCESS
} ALGOPIPELINESTATE;

class AlgoPipeline {
public:
  AlgoPipeline();
  ~AlgoPipeline();

  ALGOPIPELINESTATE ConfigureAlgoPipeline(std::vector<AlgoId> &algoList);
  ALGOPIPELINESTATE ConfigureAlgoPipeline(std::vector<std::string> &algoList);

  void Process(std::string &input);
  static void NodeEventHandler(void *,
                               std::shared_ptr<AlgoBase::ALGOCALLBACKMSG>);
  void WaitForQueueCompetion();
  size_t GetProcessedFrames() const;

  ALGOPIPELINESTATE GetState() const;
  ALGOPIPELINESTATE SetState(ALGOPIPELINESTATE state);

  std::vector<AlgoId> GetAlgoListId() const;

private:
  AlgoNodeManager *mAlgoNodeMgr = nullptr;
  std::vector<std::shared_ptr<AlgoBase>> mAlgos;

  std::vector<AlgoId> mAlgoListId;
  std::vector<std::string> mAlgoListName;

  size_t mProcessedFrames = 0;
  ALGOPIPELINESTATE mState = NOT_INITIALISED;
};
#endif // ALGO_PIPELINE_H
