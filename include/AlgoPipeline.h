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
#ifndef ALGO_PIPELINE_H
#define ALGO_PIPELINE_H

#include <vector>
#include "AlgoBase.h"
#include "AlgoDefs.h"
#include "AlgoNodeManager.h"
#include "EventHandlerThread.h"

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

typedef void (*SESSIONCALLBACK)(void* cntx, std::shared_ptr<AlgoRequest> input);

class AlgoPipeline {
 public:
  AlgoPipeline(SESSIONCALLBACK pSesionCallBackHandler = nullptr,
               void* pCtx                             = nullptr);
  ~AlgoPipeline();

  AlgoPipelineState ConfigureAlgoPipeline(std::vector<AlgoId>& algoList);
  AlgoPipelineState ConfigureAlgoPipeline(std::vector<std::string>& algoList);

  void Process(std::shared_ptr<AlgoRequest> input);
  static void NodeEventHandler(void*,
                               std::shared_ptr<AlgoBase::AlgoCallbackMessage>);
  void WaitForQueueCompetion();
  size_t GetProcessedFrames() const;

  AlgoPipelineState GetState() const;
  AlgoPipelineState SetState(AlgoPipelineState state);

  std::vector<AlgoId> GetAlgoListId() const;

  SESSIONCALLBACK pSesionCallBackHandler = nullptr;
  void* pSessionCtx                      = nullptr;

  void Dump();

  size_t mProcessedFrames = 0;
  std::mutex mRequesteMapMutex;
  std::condition_variable mCondition;
  std::unordered_map<int, std::shared_ptr<AlgoRequest>> mRequesteMap;

 private:
  AlgoNodeManager* mAlgoNodeMgr = nullptr;
  std::vector<std::shared_ptr<AlgoBase>> mAlgos;

  std::vector<AlgoId> mAlgoListId;
  std::vector<std::string> mAlgoListName;
  std::unordered_map<AlgoId, std::shared_ptr<AlgoBase>> mAlgoMap;

  AlgoPipelineState mState = AlgoPipelineState::NotInitialised;
  std::shared_ptr<EventHandlerThread<AlgoBase::AlgoCallbackMessage>>
      pEventHandlerThread;
};
#endif  // ALGO_PIPELINE_H
