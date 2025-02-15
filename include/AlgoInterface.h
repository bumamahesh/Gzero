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
#ifndef ALGO_INTERFACE_H
#define ALGO_INTERFACE_H

#include <atomic>
#include "AlgoSession.h"

#define MAX_HOLD_REQUESTS 20
const size_t MAX_MEMORY_USAGE_KB =
    500 * 1024 * 100;  // 500 *100 MB //let make high
class AlgoInterface {
 public:
  AlgoInterface();
  ~AlgoInterface();
  bool Process(std::shared_ptr<AlgoRequest> request,
               std::vector<AlgoId> algoList);
  int (*pIntfCallback)(std::shared_ptr<AlgoRequest> input) = nullptr;

  std::atomic<int> mRequestCnt{0};
  std::atomic<int> mResultCnt{0};
  mutable std::mutex mCallbackMutex;

 private:
  std::shared_ptr<AlgoSession> mSession;
  static void SessionCallbackHandler(void* pctx,
                                     std::shared_ptr<AlgoRequest> input);
};
#endif  // ALGO_INTERFACE_H
