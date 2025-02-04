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
#ifndef ALGO_NODE_MANAGER_H
#define ALGO_NODE_MANAGER_H

#include "AlgoBase.h"
#include "AlgoLibraryLoader.h"
#include <string>

class AlgoNodeManager {
public:
  static AlgoNodeManager &Getinstance();
  ~AlgoNodeManager();

  bool IsAlgoAvailable(AlgoId algoId) const;
  bool IsAlgoAvailable(std::string &algoName) const;
  size_t GetLoadedAlgosSize() const;

  std::shared_ptr<AlgoBase> CreateAlgo(AlgoId algoId);
  std::shared_ptr<AlgoBase> CreateAlgo(std::string &algoName);
  std::mutex mAlgoCreationMutex;

private:
  AlgoNodeManager();
  std::unordered_map<size_t, std::shared_ptr<AlgoBase>> mAlgoMap;
  std::string mLibraryPath;
  std::vector<std::string> mSharedLibrariesPath;
  std::unordered_map<AlgoId, std::string> mIdToAlgoNameMap;
  std::unordered_map<AlgoId, std::shared_ptr<AlgoLibraryLoader>> mIdLoaderMap;
};

#endif // ALGO_NODE_MANAGER_H
