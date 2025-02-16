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
#ifndef BOKEH_ALGORITHM_H
#define BOKEH_ALGORITHM_H

#include <opencv2/opencv.hpp>
#include <opencv2/stereo.hpp>
#include <vector>
#include "AlgoBase.h"

const char* BOKEH_NAME = "BokehAlgorithm";

/**
 * @brief BokehAlgorithm class derived from AlgoBase to perform BOKEH-specific
 * operations.
 */
class BokehAlgorithm : public AlgoBase {
 public:
  /**
   * @brief Constructor for BokehAlgorithm.
   *
   */
  BokehAlgorithm();

  /**
   * @brief Destructor for BokehAlgorithm.
   */
  ~BokehAlgorithm() override;

  /**
   * @brief Open the BOKEH algorithm, simulating resource checks.
   * @return Status of the operation.
   */
  AlgoStatus Open() override;

  /**
   * @brief Process the BOKEH algorithm, simulating BOKEH computation.
   * @return Status of the operation.
   */
  AlgoStatus Process(std::shared_ptr<AlgoRequest> req) override;

  /**
   * @brief Close the BOKEH algorithm, simulating cleanup.
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
  void DumpDepthMask(const cv::Mat& depthMask, int width, int height,
                     int processCount);

  // Function to dump disparity map for debugging with process count
  void DumpDisparityMap(const cv::Mat& disparityMap, int width, int height,
                        int processCount);

  // Optionally, you can also dump depth map with process count
  void DumpDepthMap(const cv::Mat& depthMap, int width, int height,
                    int processCount);

 private:
  mutable std::mutex mutex_;  // Mutex to protect the shared state
};

/**
 * @brief Factory function to expose BokehAlgorithm via shared library.
 * @return A pointer to the BokehAlgorithm instance.
 */
extern "C" AlgoBase* GetAlgoMethod();

#endif  // BOKEH_ALGORITHM_H