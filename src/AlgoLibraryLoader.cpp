#include "AlgoLibraryLoader.h"
#include "Log.h"
#include <cassert>
#include <dlfcn.h>

/**
@brief Construct a new Algo Library Loader:: Algo Library Loader object
 *
 * @param libraryPath
 */
AlgoLibraryLoader::AlgoLibraryLoader(const std::string &libraryPath)
    : libHandle(nullptr) {
  // Attempt to load the shared library
  libHandle = dlopen(libraryPath.c_str(), RTLD_LAZY);
  if (!libHandle) {
    LOG(ERROR, ALGOLIBLOADER, "Failed to load library: %s", dlerror());
    assert(!libHandle);
  }
  totalAlgoInstances = 0;
}

/**
@brief Destroy the Algo Library Loader:: Algo Library Loader object
 *
 */
AlgoLibraryLoader::~AlgoLibraryLoader() {
  LOG(VERBOSE, ALGOLIBLOADER, "%s::Total Algo Instances %ld",
      GetAlgorithmName().c_str(), totalAlgoInstances);
  if (libHandle) {
    dlclose(libHandle);
  }
}

std::shared_ptr<AlgoBase> AlgoLibraryLoader::GetAlgoMethod() {
  if (!libHandle) {
    LOG(ERROR, ALGOLIBLOADER, "Library handle is nullptr");
    return nullptr;
  }
  std::lock_guard<std::mutex> lock(libMutex);
  // Try to get the function from the library
  typedef AlgoBase *(*GetAlgoMethodFunc)();
  GetAlgoMethodFunc getAlgoMethod =
      reinterpret_cast<GetAlgoMethodFunc>(dlsym(libHandle, "GetAlgoMethod"));

  if (!getAlgoMethod) {
    LOG(ERROR, ALGOLIBLOADER, "Failed to find GetAlgoMethod: %s", dlerror());
    assert(!getAlgoMethod);
  }
  std::shared_ptr<AlgoBase> pAlgoBase(getAlgoMethod());
  totalAlgoInstances++;
  return pAlgoBase;
}

/**
@brief Get the Algorithm Name object
 *
 * @return std::string
 */
std::string AlgoLibraryLoader::GetAlgorithmName() const {
  if (!libHandle) {
    LOG(ERROR, ALGOLIBLOADER, "Library handle is nullptr");
    return "";
  }
  // Try to get the function from the library
  typedef std::string (*GetAlgorithmNameFunc)();
  GetAlgorithmNameFunc getAlgoName = reinterpret_cast<GetAlgorithmNameFunc>(
      dlsym(libHandle, "GetAlgorithmName"));

  if (!getAlgoName) {
    LOG(ERROR, ALGOLIBLOADER, "Failed to find GetAlgorithmName: %s", dlerror());
    assert(!getAlgoName);
  }
  return getAlgoName();
}

/**
@brief Get the Algo Id object
 *
 * @return size_t
 */
size_t AlgoLibraryLoader::GetAlgoId() const {

  if (!libHandle) {
    LOG(ERROR, ALGOLIBLOADER, "Library handle is nullptr");
    return 0XDEADBEAF;
  }
  // Try to get the function from the library
  typedef size_t (*GetAlgoIdFunc)();
  GetAlgoIdFunc getAlgoId =
      reinterpret_cast<GetAlgoIdFunc>(dlsym(libHandle, "GetAlgoId"));

  if (!getAlgoId) {
    LOG(ERROR, ALGOLIBLOADER, "Failed to find GetAlgoId: %s", dlerror());
    assert(!getAlgoId);
  }
  return getAlgoId();
}