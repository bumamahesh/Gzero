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
#include <dlfcn.h>
#include <cassert>
#include <fstream>
#include <iostream>
#include <memory>
#include <vector>
#include "../../include/AlgoDecisionManager.h"
#include "../../include/AlgoDefs.h"
#include "../../include/AlgoRequest.h"

#define ALGOLIB_PATH "/home/uma/workspace/Gzero/build/lib/libAlgoLib.so"
#define LOAD_SYM(FUNC) reinterpret_cast<FUNC##Func>(getSymbol(#FUNC))

using InitAlgoInterfaceFunc    = int (*)(void**);
using DeInitAlgoInterfaceFunc  = int (*)(void**);
using AlgoInterfaceProcessFunc = int (*)(void**, std::shared_ptr<AlgoRequest>,
                                         std::vector<AlgoId>);
using RegisterCallbackFunc     = int (*)(void**,
                                     int (*)(std::shared_ptr<AlgoRequest>));

class AlgoInterfaceptr {
 public:
  AlgoInterfaceptr(const std::string& path);
  ~AlgoInterfaceptr();
  void* getSymbol(const char* symbolName);
  InitAlgoInterfaceFunc initFunc            = nullptr;
  DeInitAlgoInterfaceFunc deinitFunc        = nullptr;
  AlgoInterfaceProcessFunc processFunc      = nullptr;
  RegisterCallbackFunc registerCallbackFunc = nullptr;
  void* libraryHandle                       = nullptr;
};

struct AlgoMetadataList {
  MetaId metaId;
  AlgoId algoId;
};

extern std::vector<AlgoMetadataList> g_AlgoMetadataList;

class DecisionManager : public AlgoDecisionManager {
 public:
  DecisionManager()  = default;
  ~DecisionManager() = default;
  std::vector<AlgoId> ParseMetadata(std::shared_ptr<AlgoRequest> req) override {
    std::vector<AlgoId> ret;

    for (const auto& algo : g_AlgoMetadataList) {
      bool enabled = false;
      if (0 == req->mMetadata.GetMetadata(algo.metaId, enabled) && enabled) {
        AlgoDecisionManager::SetAlgoFlag(algo.algoId);
      }
    }
    ret = AlgoDecisionManager::ParseMetadata(req);
    return ret;
  }
};

class AlgoInterfaceManager {
 public:
  explicit AlgoInterfaceManager(std::vector<std::string> inputFilePath,
                                int width, int height, bool stereo = false);
  ~AlgoInterfaceManager();
  int SubmitRequest();
  int SubmitStereoRequest();
  DecisionManager m_algoDecisionManager;

 private:
  int InitAlgoInterface();
  void Cleanup();
  int mRequestId = 0;

  std::shared_ptr<AlgoInterfaceptr> phandle;
  std::vector<unsigned char> yuvBuffer;
  std::ifstream mInputFile;
  std::vector<unsigned char> yuvStereoBufferp[2];
  std::ifstream mStereoInputFile[2];
  int mWidth;
  int mHeight;
};

#endif  // ALGOINTERFACEMANAGER_H
