#ifndef ALGO_SESSION_H
#define ALGO_SESSION_H

#include "AlgoPipeline.h"
#include <memory>
#include <string>
#include <vector>

class AlgoSession {
public:
  AlgoSession();
  ~AlgoSession();

  bool Initialize(std::string &config);
  bool Start();
  bool Stop();
  bool AddPipeline(std::shared_ptr<AlgoPipeline> &pipeline);
  bool RemovePipeline(size_t pipelineId);
  bool Process(std::string &input);
  bool Process(size_t pipelineId, std::string &input);
  size_t GetPipelineCount() const;
  std::vector<size_t> GetPipelineIds() const;

  virtual std::vector<size_t> GetAlgoList();

  int GetpipelineId(std::vector<size_t> algoList);
  std::shared_ptr<AlgoPipeline> GetPipeline(size_t pipelineId);

private:
  std::vector<std::shared_ptr<AlgoPipeline>> mPipelines;
  size_t mNextPipelineId = 0;
  std::unordered_map<size_t, std::shared_ptr<AlgoPipeline>> mPipelineMap;
};

#endif // ALGO_SESSION_H
