#include "AlgoNodeManager.h"
#include "AlgoLibraryLoader.h"
#include "Log.h"
#include <filesystem>
#include <regex>
#include <string>
#include <vector>

namespace fs = std::filesystem;

/**
@brief Construct a new Algo Node Manager:: Algo Node Manager object
 *
 * @param libraryPath
 */
AlgoNodeManager::AlgoNodeManager(std::string &libraryPath) {

  mLibraryPath = libraryPath;
  /*open path and prepeare a list of shared librbary of format com.Algo.*.so*/

  try {
    if (!fs::exists(mLibraryPath) || !fs::is_directory(mLibraryPath)) {
      throw std::runtime_error("Invalid library path: " + mLibraryPath);
    }

    std::regex sharedLibraryPattern(R"(com\.Algo\..*\.so)");
    for (const auto &entry : fs::directory_iterator(mLibraryPath)) {
      if (entry.is_regular_file()) {
        const auto &fileName = entry.path().filename().string();
        if (std::regex_match(fileName, sharedLibraryPattern)) {
          mSharedLibrariesPath.push_back(entry.path().string());
        }
      }
    }

    if (mSharedLibrariesPath.empty()) {
      LOG(WARNING, ALGOMANAGER, "No matching shared libraries found in %s",
          mLibraryPath.c_str());
    }
  } catch (const std::exception &e) {
    LOG(ERROR, ALGOMANAGER, "Error initializing AlgoNodeManager: %s", e.what());
  }

  /* retrive algo id and name and save , algo objects are not created here */
  for (auto &lib : mSharedLibrariesPath) {
    auto pAlgoLoader = std::make_shared<AlgoLibraryLoader>(lib);
    if (pAlgoLoader) {
      auto algo = pAlgoLoader->GetAlgoMethod();
      mIdToAlgoNameMap[pAlgoLoader->GetAlgoId()] =
          pAlgoLoader->GetAlgorithmName();
      mIdLoaderMap[pAlgoLoader->GetAlgoId()] = pAlgoLoader;
    }
  }
}

/**
@brief Destroy the Algo Node Manager:: Algo Node Manager object
 *
 */
AlgoNodeManager::~AlgoNodeManager() {
  mIdToAlgoNameMap.clear();
  mSharedLibrariesPath.clear();
  mIdLoaderMap.clear();
}

/**
@brief Check if the algorithm is available
 *
 * @param algoId
 * @return true
 * @return false
 */
bool AlgoNodeManager::IsAlgoAvailable(size_t algoId) const {
  return mIdLoaderMap.find(algoId) != mIdLoaderMap.end();
}

/**
@brief Check if the algorithm is available
 *
 * @param algoName
 * @return true
 * @return false
 */
bool AlgoNodeManager::IsAlgoAvailable(std::string &algoName) const {
  for (const auto &algo : mIdToAlgoNameMap) {
    if (algo.second == algoName) {
      return true;
    }
  }
  return false;
}

/**
@brief Create an Algo object
 *
 * @param algoId
 * @return std::shared_ptr<AlgoBase>
 */
std::shared_ptr<AlgoBase> AlgoNodeManager::CreateAlgo(size_t algoId) {
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
std::shared_ptr<AlgoBase> AlgoNodeManager::CreateAlgo(std::string &algoName) {
  for (const auto &algo : mIdToAlgoNameMap) {
    if (algo.second == algoName) {
      return mIdLoaderMap[algo.first]->GetAlgoMethod();
    }
  }
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