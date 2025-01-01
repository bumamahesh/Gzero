#ifndef ALGO_NODE_MANAGER_H
#define ALGO_NODE_MANAGER_H

#include "AlgoBase.h"
#include "AlgoLibraryLoader.h"
#include <stdexcept>
#include <string>

class AlgoNodeManager {
public:
  explicit AlgoNodeManager(std::string &libraryPath);
  ~AlgoNodeManager();

  bool IsAlgoAvailable(size_t algoId) const;
  bool IsAlgoAvailable(std::string &algoName) const;
  size_t GetLoadedAlgosSize() const;

  std::shared_ptr<AlgoBase> CreateAlgo(size_t algoId);
  std::shared_ptr<AlgoBase> CreateAlgo(std::string &algoName);

private:
  std::unordered_map<size_t, std::shared_ptr<AlgoBase>> algo_map_;
  std::string _libraryPath;
  std::vector<std::string> _sharedLibrariesPath;
  std::unordered_map<size_t, std::string> _IdAlgoNameMap;
  std::unordered_map<size_t, std::shared_ptr<AlgoLibraryLoader>> _IdLoaderMap;
};

#endif // ALGO_NODE_MANAGER_H
