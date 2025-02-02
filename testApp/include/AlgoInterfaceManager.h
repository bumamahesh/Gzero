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

#ifndef ALGOINTERFACEMANAGER_H
#define ALGOINTERFACEMANAGER_H
#pragma once
#include "../../include/AlgoDecisionManager.h"
#include "../../include/AlgoDefs.h"
#include "../../include/AlgoRequest.h"
#include <fstream>
#include <memory>
#include <vector>

using InitAlgoInterfaceFunc    = int (*)(void **);
using DeInitAlgoInterfaceFunc  = int (*)(void **);
using AlgoInterfaceProcessFunc = int (*)(void **, std::shared_ptr<AlgoRequest>,
                                         std::vector<AlgoId>);
using RegisterCallbackFunc     = int (*)(void **,
                                     int (*)(std::shared_ptr<AlgoRequest>));

struct AlgoInterfaceptr {
public:
  InitAlgoInterfaceFunc initFunc            = nullptr;
  DeInitAlgoInterfaceFunc deinitFunc        = nullptr;
  AlgoInterfaceProcessFunc processFunc      = nullptr;
  RegisterCallbackFunc registerCallbackFunc = nullptr;
  void *libraryHandle                       = nullptr;
};

class DecisionManager : public AlgoDecisionManager {
public:
  DecisionManager()  = default;
  ~DecisionManager() = default;
  std::vector<AlgoId> ParseMetadata(std::shared_ptr<AlgoRequest> req) override {
    AlgoDecisionManager::SetAlgoFlag(ALGO_FILTER);
    // AlgoDecisionManager::SetAlgoFlag(ALGO_WATERMARK);
    // AlgoDecisionManager::SetAlgoFlag(ALGO_MANDELBROTSET);
    return AlgoDecisionManager::ParseMetadata(req);
  }
};

class AlgoInterfaceManager {
public:
  explicit AlgoInterfaceManager(const std::string inputFilePath, int width, int height);
  ~AlgoInterfaceManager();
  int SubmitRequest();

private:
  bool LoadLibraryFunctions();
  int InitAlgoInterface();
  void Cleanup();
  int mRequestId = 0;

  AlgoInterfaceptr handle;
  DecisionManager m_algoDecisionManager;

  std::vector<unsigned char> yuvBuffer;
  std::ifstream mInputFile;
  int mWidth;
  int mHeight;
};

#endif // ALGOINTERFACEMANAGER_H
