#ifndef ALGO_PIPELINE_H
#define ALGO_PIPELINE_H

#include "AlgoBase.h"
#include "AlgoDefs.h"
#include "AlgoNodeManager.h"
#include "EventHandlerThread.h"
#include <memory>
#include <vector>

enum class AlgoPipelineState {
  NotInitialised = 0,
  Initialised,
  ConfiguredWithId,
  ConfiguredWithName,
  InvalidAlgoList,
  FailedToConfigure,
  Successful,
  FailedToProcess
};

typedef void (*SESSIONCALLBACK)(void *cntx, std::shared_ptr<AlgoRequest> input);

class AlgoPipeline {
public:
  AlgoPipeline(SESSIONCALLBACK pSesionCallBackHandler = nullptr,
               void *pCtx = nullptr);
  ~AlgoPipeline();

  AlgoPipelineState ConfigureAlgoPipeline(std::vector<AlgoId> &algoList);
  AlgoPipelineState ConfigureAlgoPipeline(std::vector<std::string> &algoList);

  void Process(std::shared_ptr<AlgoRequest> input);
  static void NodeEventHandler(void *,
                               std::shared_ptr<AlgoBase::AlgoCallbackMessage>);
  void WaitForQueueCompetion();
  size_t GetProcessedFrames() const;

  AlgoPipelineState GetState() const;
  AlgoPipelineState SetState(AlgoPipelineState state);

  std::vector<AlgoId> GetAlgoListId() const;

  SESSIONCALLBACK pSesionCallBackHandler = nullptr;
  void *pSessionCtx = nullptr;

  void Dump();

  size_t mProcessedFrames = 0;
  std::mutex mRequesteMapMutex;
  std::condition_variable mCondition;
  std::unordered_map<int, std::shared_ptr<AlgoRequest>> mRequesteMap;

private:
  AlgoNodeManager *mAlgoNodeMgr = nullptr;
  std::vector<std::shared_ptr<AlgoBase>> mAlgos;

  std::vector<AlgoId> mAlgoListId;
  std::vector<std::string> mAlgoListName;
  std::unordered_map<AlgoId, std::shared_ptr<AlgoBase>> mAlgoMap;

  AlgoPipelineState mState = AlgoPipelineState::NotInitialised;
  std::shared_ptr<EventHandlerThread<AlgoBase::AlgoCallbackMessage>>
      pEventHandlerThread;
};
#endif // ALGO_PIPELINE_H
