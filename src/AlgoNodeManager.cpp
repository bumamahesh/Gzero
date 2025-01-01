#include "AlgoNodeManager.h"
#include "AlgoLibraryLoader.h"
#include <filesystem>
#include <iostream>
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

  _libraryPath = libraryPath;
  /*open path and prepeare a list of shared librbary of format com.Algo.*.so*/

  try {
    if (!fs::exists(_libraryPath) || !fs::is_directory(_libraryPath)) {
      throw std::runtime_error("Invalid library path: " + _libraryPath);
    }

    std::regex sharedLibraryPattern(R"(com\.Algo\..*\.so)");
    for (const auto &entry : fs::directory_iterator(_libraryPath)) {
      if (entry.is_regular_file()) {
        const auto &fileName = entry.path().filename().string();
        if (std::regex_match(fileName, sharedLibraryPattern)) {
          _sharedLibrariesPath.push_back(entry.path().string());
        }
      }
    }

    if (_sharedLibrariesPath.empty()) {
      std::cerr << "Warning: No matching shared libraries found in "
                << _libraryPath << std::endl;
    }
  } catch (const std::exception &e) {
    std::cerr << "Error initializing AlgoNodeManager: " << e.what()
              << std::endl;
  }

  /* retrive algo id and name and save , algo objects are not created here */
  for (auto &lib : _sharedLibrariesPath) {
    auto pAlgoLoader = std::make_shared<AlgoLibraryLoader>(lib);
    if (pAlgoLoader) {
      auto algo = pAlgoLoader->GetAlgoMethod();
      _IdAlgoNameMap[pAlgoLoader->GetAlgoId()] =
          pAlgoLoader->GetAlgorithmName();
      _IdLoaderMap[pAlgoLoader->GetAlgoId()] = pAlgoLoader;
    }
  }
}

/**
@brief Destroy the Algo Node Manager:: Algo Node Manager object
 *
 */
AlgoNodeManager::~AlgoNodeManager() {
  _IdAlgoNameMap.clear();
  _sharedLibrariesPath.clear();
  _IdLoaderMap.clear();
}

/**
@brief Check if the algorithm is available
 *
 * @param algoId
 * @return true
 * @return false
 */
bool AlgoNodeManager::IsAlgoAvailable(size_t algoId) const {
  return _IdLoaderMap.find(algoId) != _IdLoaderMap.end();
}

/**
@brief Check if the algorithm is available
 *
 * @param algoName
 * @return true
 * @return false
 */
bool AlgoNodeManager::IsAlgoAvailable(std::string &algoName) const {
  for (const auto &algo : _IdAlgoNameMap) {
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
    return _IdLoaderMap[algoId]->GetAlgoMethod();
  }
  std::cout << "Algo not available" << std::endl;
  return nullptr;
}

/**
@brief Create an Algo object
 *
 * @param algoName
 * @return std::shared_ptr<AlgoBase>
 */
std::shared_ptr<AlgoBase> AlgoNodeManager::CreateAlgo(std::string &algoName) {
  for (const auto &algo : _IdAlgoNameMap) {
    if (algo.second == algoName) {
      return _IdLoaderMap[algo.first]->GetAlgoMethod();
    }
  }
  std::cout << "Algo not available" << std::endl;
  return nullptr;
}
/**
@brief Get the Loaded Algos Size object
 *
 * @return size_t
 */
size_t AlgoNodeManager::GetLoadedAlgosSize() const {
  return _IdLoaderMap.size();
}