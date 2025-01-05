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
  bool Stop();
  bool AddPipeline(std::shared_ptr<AlgoPipeline> &pipeline);
  bool RemovePipeline(size_t pipelineId);
  bool Process(std::shared_ptr<AlgoRequest> input);
  bool Process(size_t pipelineId, std::shared_ptr<AlgoRequest> input);
  size_t GetPipelineCount() const;
  std::vector<size_t> GetPipelineIds() const;

  std::vector<AlgoId> GetAlgoList();

  int GetpipelineId(std::vector<AlgoId> algoList);
  std::shared_ptr<AlgoPipeline> GetPipeline(size_t pipelineId);
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
