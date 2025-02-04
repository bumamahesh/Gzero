/*
 * Copyright (c) [2025] [Uma Mahesh B]
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#ifndef ALGO_LIBRARY_LOADER_H
#define ALGO_LIBRARY_LOADER_H

#include "AlgoBase.h"
#include <string>

// All exposed api of libs
typedef AlgoBase *(*GetAlgoMethodFunc)();
typedef const char *(*GetAlgorithmNameFunc)();
typedef AlgoId (*GetAlgoIdFunc)();

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
  AlgoId GetAlgoId() const;

private:
  void *plibHandle = nullptr;     // Handle to the shared library
  std::mutex mlibMutex;           // Mutex to protect the shared library handle
  size_t mTotalAlgoInstances = 0; // Total Algo Instanced Opned

  GetAlgoMethodFunc mGetAlgoMethod  = nullptr;
  GetAlgorithmNameFunc mGetAlgoName = nullptr;
  GetAlgoIdFunc mGetAlgoId          = nullptr;
};

#endif // ALGO_LIBRARY_LOADER_H
