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
#ifndef HDR_ALGORITHM_H
#define HDR_ALGORITHM_H

#include "AlgoBase.h"
const char *MANDELBROTSET_NAME = "MandelbrotSetAlgorithm";

// Constants for Mandelbrot calculation
constexpr int MAX_ITER = 100; // Maximum iterations for escape condition
constexpr double INITIAL_ZOOM = 1.05;
constexpr double ZOOM_FACTOR = 1.2;

const double CentreCordinates[3][2] = {{-0.74364388703, 0.13182590421},
                                       {-0.74700000000, 0.10000000000},
                                       {-0.75000000000, 0.00000000000}};

enum MandelbrotSetCentre {
  Seahorse_Valley = 0,
  Spiral_formation,
  Main,
};
/**
 * @brief MandelbrotSet class derived from AlgoBase to perform HDR-specific
 * operations.
 */
class MandelbrotSet : public AlgoBase {
public:
  /**
   * @brief Constructor for MandelbrotSet.
   *
   */
  MandelbrotSet();

  /**
   * @brief Destructor for MandelbrotSet.
   */
  ~MandelbrotSet() override;

  /**
   * @brief Open the HDR algorithm, simulating resource checks.
   * @return Status of the operation.
   */
  AlgoStatus Open() override;

  /**
   * @brief Process the HDR algorithm, simulating HDR computation.
   * @return Status of the operation.
   */
  AlgoStatus Process(std::shared_ptr<AlgoRequest> req) override;

  /**
   * @brief Close the HDR algorithm, simulating cleanup.
   * @return Status of the operation.
   */
  // cppcheck-suppress virtualCallInConstructor
  AlgoStatus Close() override;

  /**
   * @brief Get the Timeout object
   *
   * @return int
   */
  int GetTimeout() override;

private:
  mutable std::mutex mutex_; // Mutex to protect the shared state
  double zoomLevel;
  double offsetX;
  double offsetY;
  int modelIdx;
};

/**
 * @brief Factory function to expose MandelbrotSet via shared library.
 * @return A pointer to the MandelbrotSet instance.
 */
extern "C" AlgoBase *GetAlgoMethod();

#endif // HDR_ALGORITHM_H