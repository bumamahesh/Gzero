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
#ifndef FILTER_ALGORITHM_H
#define FILTER_ALGORITHM_H

#include "AlgoBase.h"
const char *FILTER_NAME = "FilterAlgorithm";
/**
 * @brief FilterAlgorithm class derived from AlgoBase to perform Filter-specific
 * operations.
 */
class FilterAlgorithm : public AlgoBase {
public:
  /**
   * @brief Constructor for FilterAlgorithm.
   *
   */
  FilterAlgorithm();

  /**
   * @brief Destructor for FilterAlgorithm.
   */
  ~FilterAlgorithm() override;

  /**
   * @brief Open the Filter algorithm, simulating resource checks.
   * @return Status of the operation.
   */
  AlgoStatus Open() override;

  /**
   * @brief Process the Filter algorithm, simulating Filter computation.
   * @return Status of the operation.
   */
  AlgoStatus Process(std::shared_ptr<AlgoRequest> req) override;

  /**
   * @brief Close the Filter algorithm, simulating cleanup.
   * @return Status of the operation.
   */
  AlgoStatus Close() override;
  // cppcheck-suppress virtualCallInConstructor

  /**
   * @brief Get the Timeout object
   *
   * @return int
   */
  int GetTimeout() override;

private:
  mutable std::mutex mutex_; // Mutex to protect the shared state
                             /* Sobel kernels for X and Y gradients*/
  const int Gx[3][3] = {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}};
  const int Gy[3][3] = {{1, 2, 1}, {0, 0, 0}, {-1, -2, -1}};

  AlgoStatus SobelRGB(std::shared_ptr<AlgoRequest> req);
  AlgoStatus SobelYuv(std::shared_ptr<AlgoRequest> req);
};

/**
 * @brief Factory function to expose FilterAlgorithm via shared library.
 * @return A pointer to the FilterAlgorithm instance.
 */
extern "C" AlgoBase *GetAlgoMethod();

#endif // FILTER_ALGORITHM_H