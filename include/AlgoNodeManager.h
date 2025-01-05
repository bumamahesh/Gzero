#ifndef ALGO_NODE_MANAGER_H
#define ALGO_NODE_MANAGER_H

#include "AlgoBase.h"
#include "AlgoLibraryLoader.h"
#include <string>

class AlgoNodeManager {
public:
  static AlgoNodeManager &Getinstance();
  ~AlgoNodeManager();

  bool IsAlgoAvailable(AlgoId algoId) const;
  bool IsAlgoAvailable(std::string &algoName) const;
  size_t GetLoadedAlgosSize() const;

  std::shared_ptr<AlgoBase> CreateAlgo(AlgoId algoId);
  std::shared_ptr<AlgoBase> CreateAlgo(std::string &algoName);

private:
  AlgoNodeManager();
  std::unordered_map<size_t, std::shared_ptr<AlgoBase>> mAlgoMap;
  std::string mLibraryPath;
  std::vector<std::string> mSharedLibrariesPath;
  std::unordered_map<AlgoId, std::string> mIdToAlgoNameMap;
  std::unordered_map<AlgoId, std::shared_ptr<AlgoLibraryLoader>> mIdLoaderMap;
};

#endif // ALGO_NODE_MANAGER_H
