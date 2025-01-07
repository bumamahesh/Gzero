#ifndef ALGO_SESSION_H
#define ALGO_SESSION_H

#include "AlgoPipeline.h"
#include <memory>
#include <vector>

typedef void (*INTERFACECALLBACK)(void *pctx,
                                  std::shared_ptr<AlgoRequest> input);
class AlgoSession {
public:
  AlgoSession(INTERFACECALLBACK pInterfaceCallBackHandler = nullptr,
              void *pCtx = nullptr);
  ~AlgoSession();
  bool SessionStop();
  bool SessionAddPipeline(std::shared_ptr<AlgoPipeline> &pipeline);
  bool SessionRemovePipeline(size_t pipelineId);
  bool SessionProcess(std::shared_ptr<AlgoRequest> input);
  bool SessionProcess(size_t pipelineId, std::shared_ptr<AlgoRequest> input);
  size_t SessionGetPipelineCount() const;
  std::vector<size_t> SessionGetPipelineIds() const;

  std::vector<AlgoId> SessionGetAlgoList();

  int SessionGetpipelineId(std::vector<AlgoId> algoList);
  std::shared_ptr<AlgoPipeline> SessionGetPipeline(size_t pipelineId);
  INTERFACECALLBACK pInterfaceCallBackHandler = nullptr;
  void *pInterfaceCtx = nullptr;

private:
  std::vector<std::shared_ptr<AlgoPipeline>> mPipelines;
  size_t mNextPipelineId = 0;
  std::unordered_map<size_t, std::shared_ptr<AlgoPipeline>> mPipelineMap;

  static void PiplineCallBackHandler(void *pctx,
                                     std::shared_ptr<AlgoRequest> input);
};

#endif // ALGO_SESSION_H
