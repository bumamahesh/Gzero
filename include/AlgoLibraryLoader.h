#ifndef ALGO_LIBRARY_LOADER_H
#define ALGO_LIBRARY_LOADER_H

#include "AlgoBase.h"
#include <string>

typedef AlgoBase *(*GetAlgoMethodFunc)();
typedef std::string (*GetAlgorithmNameFunc)();
typedef size_t (*GetAlgoIdFunc)();
class AlgoLibraryLoader {
public:
  // Constructor to load the shared library
  explicit AlgoLibraryLoader(const std::string &libraryPath);

  // Destructor to unload the shared library
  ~AlgoLibraryLoader();

  // Function to get the algorithm method from the shared library
  std::shared_ptr<AlgoBase> GetAlgoMethod();

  // Function to get the algorithm name
  std::string GetAlgorithmName() const;

  // Function to get the algorithm ID
  size_t GetAlgoId() const;

private:
  void *plibHandle = nullptr; // Handle to the shared library
  std::mutex mlibMutex;       // Mutex to protect the shared library handle
  size_t mTotalAlgoInstances = 0;

  GetAlgoMethodFunc mGetAlgoMethod = nullptr;
  GetAlgorithmNameFunc mGetAlgoName = nullptr;
  GetAlgoIdFunc mGetAlgoId = nullptr;
};

#endif // ALGO_LIBRARY_LOADER_H
