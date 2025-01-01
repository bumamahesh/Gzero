#ifndef ALGO_LIBRARY_LOADER_H
#define ALGO_LIBRARY_LOADER_H

#include "AlgoBase.h"
#include <stdexcept>
#include <string>

class AlgoLibraryLoader {
public:
  // Constructor to load the shared library
  explicit AlgoLibraryLoader(const std::string &libraryPath);

  // Destructor to unload the shared library
  ~AlgoLibraryLoader();

  // Function to get the algorithm method from the shared library
  std::shared_ptr<AlgoBase> GetAlgoMethod();

private:
  void *libHandle; // Handle to the shared library
};

#endif // ALGO_LIBRARY_LOADER_H
