#include "AlgoLibraryLoader.h"
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
    throw std::runtime_error("Failed to load library: " +
                             std::string(dlerror()));
  }
}

/**
@brief Destroy the Algo Library Loader:: Algo Library Loader object
 *
 */
AlgoLibraryLoader::~AlgoLibraryLoader() {
  if (libHandle) {
    dlclose(libHandle);
  }
}

std::shared_ptr<AlgoBase> AlgoLibraryLoader::GetAlgoMethod() {
  // Try to get the function from the library
  typedef AlgoBase *(*GetAlgoMethodFunc)();
  GetAlgoMethodFunc getAlgoMethod =
      reinterpret_cast<GetAlgoMethodFunc>(dlsym(libHandle, "GetAlgoMethod"));

  if (!getAlgoMethod) {
    throw std::runtime_error("Failed to find GetAlgoMethod: " +
                             std::string(dlerror()));
  }
  std::shared_ptr<AlgoBase> pAlgoBase(getAlgoMethod());
  return pAlgoBase;
}

/**
@brief Get the Algorithm Name object
 *
 * @return std::string
 */
std::string AlgoLibraryLoader::GetAlgorithmName() const {
  // Try to get the function from the library
  typedef std::string (*GetAlgorithmNameFunc)();
  GetAlgorithmNameFunc getAlgoName = reinterpret_cast<GetAlgorithmNameFunc>(
      dlsym(libHandle, "GetAlgorithmName"));

  if (!getAlgoName) {
    throw std::runtime_error("Failed to find GetAlgorithmName: " +
                             std::string(dlerror()));
  }
  return getAlgoName();
}

/**
@brief Get the Algo Id object
 *
 * @return size_t
 */
size_t AlgoLibraryLoader::GetAlgoId() const {

  // Try to get the function from the library
  typedef size_t (*GetAlgoIdFunc)();
  GetAlgoIdFunc getAlgoId =
      reinterpret_cast<GetAlgoIdFunc>(dlsym(libHandle, "GetAlgoId"));

  if (!getAlgoId) {
    throw std::runtime_error("Failed to find GetAlgoId: " +
                             std::string(dlerror()));
  }
  return getAlgoId();
}