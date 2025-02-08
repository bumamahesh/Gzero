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
const char* SWJPEG_NAME = "SwJpegAlgorithm";

/**
 * @brief SwJpeg class derived from AlgoBase to perform
 * SWJPEG-specific operations.
 */
class SwJpeg : public AlgoBase {
 public:
  /**
   * @brief Constructor for SwJpeg.
   *
   */
  SwJpeg();

  /**
   * @brief Destructor for SwJpeg.
   */
  ~SwJpeg() override;

  /**
   * @brief Open the SWJPEG algorithm, simulating resource checks.
   * @return Status of the operation.
   */
  AlgoStatus Open() override;

  /**
   * @brief Process the SWJPEG algorithm, simulating SWJPEG
   * computation.
   * @return Status of the operation.
   */
  AlgoStatus Process(std::shared_ptr<AlgoRequest> req) override;

  /**
   * @brief Close the SWJPEG algorithm, simulating cleanup.
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
  mutable std::mutex mutex_;  // Mutex to protect the shared state
};

/**
 * @brief Factory function to expose SwJpeg via shared library.
 * @return A pointer to the SwJpeg instance.
 */
extern "C" AlgoBase* GetAlgoMethod();

#endif  // HDR_ALGORITHM_H