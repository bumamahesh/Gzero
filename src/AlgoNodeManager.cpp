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
#include "AlgoNodeManager.h"
#include <algorithm>
#include <cassert>
#include <filesystem>
#include <string>
#include <vector>
#include "AlgoLibraryLoader.h"
#include "Log.h"

namespace fs = std::filesystem;

AlgoNodeManager& AlgoNodeManager::Getinstance() {
  static AlgoNodeManager instance;
  return instance;
}
/**
@brief Construct a new Algo Node Manager:: Algo Node Manager object
 *
 * @param libraryPath
 */
AlgoNodeManager::AlgoNodeManager() {
  LOG(INFO, ALGOMANAGER, "AlgoNodeManager::AlgoNodeManager E");
  mLibraryPath =
      "/home/uma/workspace/Gzero/build/lib/";  // "@todo get from xml later
  /* Open the path and prepare a list of shared libraries with the format com.Algo.*.so */

  try {
    if (!fs::exists(mLibraryPath) || !fs::is_directory(mLibraryPath)) {
      throw std::runtime_error("Invalid library path: " + mLibraryPath);
    }

    for (const auto& entry : fs::directory_iterator(mLibraryPath)) {
      if (entry.is_regular_file()) {
        const auto& fileName = entry.path().filename().string();
        if (fileName.find("com.Algo.") == 0 &&
            fileName.find(".so") == fileName.size() - 3) {
          mSharedLibrariesPath.push_back(entry.path().string());
        }
      }
    }

    if (mSharedLibrariesPath.empty()) {
      LOG(WARNING, ALGOMANAGER, "No matching shared libraries found in %s",
          mLibraryPath.c_str());
    }
  } catch (const std::exception& e) {
    LOG(ERROR, ALGOMANAGER, "Error initializing AlgoNodeManager: %s", e.what());
  }

  mIdLoaderMap.clear();
  mIdToAlgoNameMap.clear();
  /* retrive algo id and name and save , algo objects are not created here */
  for (auto& lib : mSharedLibrariesPath) {
    LOG(INFO, ALGOMANAGER, "E Loading library: %s", lib.c_str());
    std::shared_ptr<AlgoLibraryLoader> pAlgoLoader = nullptr;
    try {
      pAlgoLoader = std::make_shared<AlgoLibraryLoader>(lib);
      //auto algo   = pAlgoLoader->GetAlgoMethod();
      mIdToAlgoNameMap[pAlgoLoader->GetAlgoId()] =
          pAlgoLoader->GetAlgorithmName();
      mIdLoaderMap[pAlgoLoader->GetAlgoId()] = pAlgoLoader;
    } catch (const std::bad_alloc& e) {
      LOG(ERROR, ALGOMANAGER,
          "Failed to allocate memory for AlgoLibraryLoader");
      assert(pAlgoLoader);
    }

    LOG(INFO, ALGOMANAGER, "X Loading library: %s", lib.c_str());
  }
  LOG(INFO, ALGOMANAGER, "AlgoNodeManager::AlgoNodeManager X");
}

/**
@brief Destroy the Algo Node Manager:: Algo Node Manager object
 *
 */
AlgoNodeManager::~AlgoNodeManager() {
  LOG(INFO, ALGOMANAGER, "~AlgoNodeManager E");

  // Clear other containers
  mAlgoMap.clear();
  mSharedLibrariesPath.clear();
  mIdToAlgoNameMap.clear();

  // Explicitly clear and release resources before destruction
  for (auto& entry : mIdLoaderMap) {
    entry.second.reset();  // Reset shared pointers to ensure proper cleanup
  }
  mIdLoaderMap.clear();  // Clear the map after releasing resources
  LOG(INFO, ALGOMANAGER, "~AlgoNodeManager X");
}

/**
@brief Check if the algorithm is available
 *
 * @param algoId
 * @return true
 * @return false
 */
bool AlgoNodeManager::IsAlgoAvailable(AlgoId algoId) const {
  return mIdLoaderMap.find(algoId) != mIdLoaderMap.end();
}

/**
@brief Check if the algorithm is available
 *
 * @param algoName
 * @return true
 * @return false
 */
bool AlgoNodeManager::IsAlgoAvailable(std::string& algoName) const {
  return std::any_of(
      mIdToAlgoNameMap.begin(), mIdToAlgoNameMap.end(),
      [&algoName](const auto& algo) { return algo.second == algoName; });
}

/**
@brief Create an Algo object
 *
 * @param algoId
 * @return std::shared_ptr<AlgoBase>
 */
std::shared_ptr<AlgoBase> AlgoNodeManager::CreateAlgo(AlgoId algoId) {
  std::lock_guard<std::mutex> lock(mAlgoCreationMutex);
  if (IsAlgoAvailable(algoId)) {
    return mIdLoaderMap[algoId]->GetAlgoMethod();
  }
  LOG(ERROR, ALGOMANAGER, "Algo not available");
  return nullptr;
}

/**
@brief Create an Algo object
 *
 * @param algoName
 * @return std::shared_ptr<AlgoBase>
 */
std::shared_ptr<AlgoBase> AlgoNodeManager::CreateAlgo(std::string& algoName) {
  std::lock_guard<std::mutex> lock(mAlgoCreationMutex);
  auto it = std::find_if(
      mIdToAlgoNameMap.begin(), mIdToAlgoNameMap.end(),
      [&algoName](const auto& algo) { return algo.second == algoName; });

  if (it != mIdToAlgoNameMap.end()) {
    return mIdLoaderMap[it->first]->GetAlgoMethod();
  }

  // If no match is found, log and return nullptr
  LOG(ERROR, ALGOMANAGER, "Algo not available");
  return nullptr;
}
/**
@brief Get the Loaded Algos Size object
 *
 * @return size_t
 */
size_t AlgoNodeManager::GetLoadedAlgosSize() const {
  return mIdLoaderMap.size();
}