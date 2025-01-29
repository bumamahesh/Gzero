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

#ifndef ALGODECISIONMANAGER_H
#define ALGODECISIONMANAGER_H

#include "AlgoDefs.h"
#include "AlgoRequest.h"
#include <memory>
#include <vector>

#define ALGO_OFFSET(ALGO_IDX) (static_cast<int>(ALGO_IDX - ALGO_BASE_ID))

class AlgoDecisionManager {
public:
  AlgoDecisionManager();
  ~AlgoDecisionManager();
  bool IsAlgoEnabled(AlgoId algoId);
  uint32_t GetAlgoFlag() const;
  uint32_t SetAlgoFlag(AlgoId algoId);
  virtual std::vector<AlgoId> ParseMetadata(std::shared_ptr<AlgoRequest> req);

private:
  uint32_t mAlgoFlag = 0x00;
};

#endif // ALGODECISIONMANAGER_H
