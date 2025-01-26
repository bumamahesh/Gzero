#include "AlgoNodeManager.h"
#include "AlgoLibraryLoader.h"
#include "Log.h"
#include <algorithm>
#include <cassert>
#include <filesystem>
#include <regex>
#include <string>
#include <vector>

namespace fs = std::filesystem;

AlgoNodeManager &AlgoNodeManager::Getinstance() {
  static AlgoNodeManager instance;
  return instance;
}
/**
@brief Construct a new Algo Node Manager:: Algo Node Manager object
 *
 * @param libraryPath
 */
AlgoNodeManager::AlgoNodeManager() {

  mLibraryPath =
      "/home/uma/workspace/Gzero/cmake/lib/"; // "@todo get from xml later
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

  mIdLoaderMap.clear();
  mIdToAlgoNameMap.clear();
  /* retrive algo id and name and save , algo objects are not created here */
  for (auto &lib : mSharedLibrariesPath) {
    // LOG(VERBOSE, ALGOMANAGER, "E Loading library: %s", lib.c_str());
    std::shared_ptr<AlgoLibraryLoader> pAlgoLoader = nullptr;

    try {
      pAlgoLoader = std::make_shared<AlgoLibraryLoader>(lib);
      auto algo = pAlgoLoader->GetAlgoMethod();
      mIdToAlgoNameMap[pAlgoLoader->GetAlgoId()] =
          pAlgoLoader->GetAlgorithmName();
      mIdLoaderMap[pAlgoLoader->GetAlgoId()] = pAlgoLoader;
    } catch (const std::bad_alloc &e) {
      LOG(ERROR, ALGOMANAGER,
          "Failed to allocate memory for AlgoLibraryLoader");
      assert(pAlgoLoader);
    }

    // LOG(VERBOSE, ALGOMANAGER, "X Loading library: %s", lib.c_str());
  }
}

/**
@brief Destroy the Algo Node Manager:: Algo Node Manager object
 *
 */
AlgoNodeManager::~AlgoNodeManager() {
  // LOG(ERROR, ALGOMANAGER, "~AlgoNodeManager");

  // Clear other containers
  mAlgoMap.clear();
  mSharedLibrariesPath.clear();
  mIdToAlgoNameMap.clear();

  // Explicitly clear and release resources before destruction
  for (auto &entry : mIdLoaderMap) {
    entry.second.reset(); // Reset shared pointers to ensure proper cleanup
  }
  mIdLoaderMap.clear(); // Clear the map after releasing resources
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
bool AlgoNodeManager::IsAlgoAvailable(std::string &algoName) const {
  return std::any_of(
      mIdToAlgoNameMap.begin(), mIdToAlgoNameMap.end(),
      [&algoName](const auto &algo) { return algo.second == algoName; });
}

/**
@brief Create an Algo object
 *
 * @param algoId
 * @return std::shared_ptr<AlgoBase>
 */
std::shared_ptr<AlgoBase> AlgoNodeManager::CreateAlgo(AlgoId algoId) {
  if (IsAlgoAvailable(algoId)) {
    return mIdLoaderMap[algoId]->GetAlgoMethod();
  }
  LOG(DEBUG, ALGOMANAGER, "Algo not available");
  return nullptr;
}

/**
@brief Create an Algo object
 *
 * @param algoName
 * @return std::shared_ptr<AlgoBase>
 */
std::shared_ptr<AlgoBase> AlgoNodeManager::CreateAlgo(std::string &algoName) {
  auto it = std::find_if(
      mIdToAlgoNameMap.begin(), mIdToAlgoNameMap.end(),
      [&algoName](const auto &algo) { return algo.second == algoName; });

  if (it != mIdToAlgoNameMap.end()) {
    return mIdLoaderMap[it->first]->GetAlgoMethod();
  }

  // If no match is found, log and return nullptr
  LOG(DEBUG, ALGOMANAGER, "Algo not available");
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