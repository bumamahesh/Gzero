/*
 * Copyright (c) [2025] [Uma Mahesh B]
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#ifndef ALGO_SESSION_H
#define ALGO_SESSION_H

#include "AlgoPipeline.h"
#include <memory>
#include <mutex>
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
  mutable std::mutex mCallbackMutex;

  void Dump();

private:
  std::vector<std::shared_ptr<AlgoPipeline>> mPipelines;
  size_t mNextPipelineId = 0;
  std::unordered_map<size_t, std::shared_ptr<AlgoPipeline>> mPipelineMap;

  static void PiplineCallBackHandler(void *pctx,
                                     std::shared_ptr<AlgoRequest> input);
};

#endif // ALGO_SESSION_H
