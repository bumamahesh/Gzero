#include "AlgoLibraryLoader.h"
#include <dlfcn.h>

AlgoLibraryLoader::AlgoLibraryLoader(const std::string &libraryPath)
    : libHandle(nullptr) {
  // Attempt to load the shared library
  libHandle = dlopen(libraryPath.c_str(), RTLD_LAZY);
  if (!libHandle) {
    throw std::runtime_error("Failed to load library: " +
                             std::string(dlerror()));
  }
}

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
