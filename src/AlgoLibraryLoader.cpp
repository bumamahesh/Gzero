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
#include "AlgoLibraryLoader.h"
#include <dlfcn.h>
#include <cassert>
#include "Log.h"

/**
@brief Construct a new Algo Library Loader:: Algo Library Loader object
 *
 * @param libraryPath
 */
AlgoLibraryLoader::AlgoLibraryLoader(const std::string& libraryPath) {
  LOG(INFO, ALGOLIBLOADER, "AlgoLibraryLoader::AlgoLibraryLoader E");
  // Attempt to load the shared library
  plibHandle = dlopen(libraryPath.c_str(), RTLD_NOW | RTLD_NODELETE);
  if (!plibHandle) {
    LOG(ERROR, ALGOLIBLOADER, "Failed to load library: %s", dlerror());
    assert(!plibHandle);
  } /*else {
    LOG(VERBOSE, ALGOLIBLOADER, "Loaded library: %s ::%p", libraryPath.c_str(),
        plibHandle);
  }*/

  mGetAlgoId = reinterpret_cast<GetAlgoIdFunc>(dlsym(plibHandle, "GetAlgoId"));
  if (!mGetAlgoId) {
    LOG(ERROR, ALGOLIBLOADER, "Failed to find GetAlgoId: %s", dlerror());
    assert(!mGetAlgoId);
  }
  mGetAlgoMethod =
      reinterpret_cast<GetAlgoMethodFunc>(dlsym(plibHandle, "GetAlgoMethod"));
  if (!mGetAlgoMethod) {
    LOG(ERROR, ALGOLIBLOADER, "Failed to find GetAlgoMethod: %s", dlerror());
    assert(!mGetAlgoMethod);
  }
  mGetAlgoName = reinterpret_cast<GetAlgorithmNameFunc>(
      dlsym(plibHandle, "GetAlgorithmName"));

  if (!mGetAlgoName) {
    LOG(ERROR, ALGOLIBLOADER, "Failed to find GetAlgorithmName: %s", dlerror());
    assert(!mGetAlgoName);
  }
  std::lock_guard<std::mutex> lock(mlibMutex);
  mTotalAlgoInstances = 0;
  LOG(INFO, ALGOLIBLOADER, "AlgoLibraryLoader::AlgoLibraryLoader X");
}

/**
@brief Destroy the Algo Library Loader:: Algo Library Loader object
 *
 */
AlgoLibraryLoader::~AlgoLibraryLoader() {
  LOG(INFO, ALGOLIBLOADER, "%s::[%p]Total Algo Instances %ld plibHandle:: %p",
      GetAlgorithmName().c_str(), (void*)this, mTotalAlgoInstances, plibHandle);
  if (plibHandle) {
    std::lock_guard<std::mutex> lock(mlibMutex);
    mTotalAlgoInstances = 0;
    dlclose(plibHandle);
    plibHandle = nullptr;
  }
}

std::shared_ptr<AlgoBase> AlgoLibraryLoader::GetAlgoMethod() {

  if (!mGetAlgoMethod) {
    LOG(ERROR, ALGOLIBLOADER, "mGetAlgoMethod is nullptr");
    return nullptr;
  }
  std::lock_guard<std::mutex> lock(mlibMutex);
  std::shared_ptr<AlgoBase> pAlgoBase(mGetAlgoMethod());
  pAlgoBase->Open();
  mTotalAlgoInstances++;
  return pAlgoBase;
}

/**
@brief Get the Algorithm Name object
 *
 * @return std::string
 */
std::string AlgoLibraryLoader::GetAlgorithmName() const {
  if (!mGetAlgoName) {
    LOG(ERROR, ALGOLIBLOADER, "mGetAlgoName is nullptr");
    return "";
  }
  return std::string(mGetAlgoName());
}

/**
@brief Get the Algo Id object
 *
 * @return size_t
 */
AlgoId AlgoLibraryLoader::GetAlgoId() const {
  if (!mGetAlgoId) {
    LOG(ERROR, ALGOLIBLOADER, "mGetAlgoId is nullptr");
    return AlgoId::ALGO_MAX;
  }
  return mGetAlgoId();
}