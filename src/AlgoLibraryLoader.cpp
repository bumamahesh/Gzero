#include "AlgoLibraryLoader.h"
#include "Log.h"
#include <cassert>
#include <dlfcn.h>

/**
@brief Construct a new Algo Library Loader:: Algo Library Loader object
 *
 * @param libraryPath
 */
AlgoLibraryLoader::AlgoLibraryLoader(const std::string &libraryPath) {
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
  mTotalAlgoInstances = 0;
}

/**
@brief Destroy the Algo Library Loader:: Algo Library Loader object
 *
 */
AlgoLibraryLoader::~AlgoLibraryLoader() {
  /*LOG(VERBOSE, ALGOLIBLOADER,
      "%s::[%p]Total Algo Instances %ld plibHandle:: %p",
      GetAlgorithmName().c_str(), this, mTotalAlgoInstances, plibHandle);*/
  if (plibHandle) {
    mTotalAlgoInstances = 0;
    dlclose(plibHandle);
    plibHandle = nullptr;
  }
}

std::shared_ptr<AlgoBase> AlgoLibraryLoader::GetAlgoMethod() {

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
  return std::string(mGetAlgoName());
}

/**
@brief Get the Algo Id object
 *
 * @return size_t
 */
AlgoId AlgoLibraryLoader::GetAlgoId() const { return mGetAlgoId(); }