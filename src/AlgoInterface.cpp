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
#include "AlgoInterface.h"
#include "Log.h"
#include "Utils.h"
#include <cassert>
/**
 * @brief Construct a new Algo Interface:: Algo Interface object
 *
 */
AlgoInterface::AlgoInterface() {
  Log::SetLevel(LogLevel::L_VERBOSE);
  LOG(INFO, ALGOINTERFACE, "AlgoInterface::AlgoInterface");
  mSession = std::make_shared<AlgoSession>(
      AlgoInterface::SessionCallbackHandler, this);
}

/**
 * @brief Destroy the Algo Interface:: Algo Interface object
 *
 */
AlgoInterface::~AlgoInterface() {
  LOG(INFO, ALGOINTERFACE, "AlgoInterface::~AlgoInterface");
}

/**
 * @brief
 *
 * @param request
 * @return true
 * @return false
 */
bool AlgoInterface::Process(std::shared_ptr<AlgoRequest> request,
                            std::vector<AlgoId> algoList) {

  LOG(INFO, ALGOINTERFACE, "AlgoInterface::Process");

  if (GetMemoryUsage() > 30000) {
    while (GetMemoryUsage() > 30000) {
      usleep(33 * 1000); // 33 ms assume 1 frame delay
      LOG(VERBOSE, ALGOINTERFACE,
          "AlgoInterface:: BLOCKED GetMemoryUsage() ::%ld KB",
          GetMemoryUsage());
    }
  }
  LOG(ERROR, ALGOINTERFACE, "AlgoInterface::GetMemoryUsage() ::%ld KB",
      GetMemoryUsage());

  if ((mRequestCnt - mResultCnt) > MAX_HOLD_REQUESTS) {
    usleep(33 * 1000); // 33 ms assume 1 frame delay)
    LOG(VERBOSE, ALGOINTERFACE, "[Req::%d Res::%d] processed[%d]",
        mRequestCnt.load(), mResultCnt.load(),
        (mRequestCnt.load() - mResultCnt.load()));
  }
  if (mSession) {
    mSession->SessionProcess(request, algoList);
    // mSession->Dump();
  } else {
    LOG(ERROR, ALGOINTERFACE, "mSession is nullptr");
  }
  mRequestCnt.fetch_add(1, std::memory_order_relaxed);
  return true;
}

/**
 * @brief Session Callback Handler
 *
 * @param pctx
 * @param input
 */
void AlgoInterface::SessionCallbackHandler(void *pctx,
                                           std::shared_ptr<AlgoRequest> input) {
  assert(pctx != nullptr);
  AlgoInterface *algoInterface = static_cast<AlgoInterface *>(pctx);
  if (algoInterface->pIntfCallback) {
    algoInterface->pIntfCallback(input);
    algoInterface->mResultCnt.fetch_add(1, std::memory_order_relaxed);
  }
}
