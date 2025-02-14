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
#ifndef WATERMARK_ALGORITHM_H
#define WATERMARK_ALGORITHM_H

#include "AlgoBase.h"
#ifdef _CV_ENABLED_
#include <opencv2/opencv.hpp>
#endif
#include <string>
const char *WATERMARK_NAME = "WaterMarkAlgorithm";

enum class WatermarkPosition { TOP_LEFT,
                               TOP_RIGHT,
                               BOTTOM_LEFT,
                               BOTTOM_RIGHT };

/**
 * @brief WaterMarkAlgorithm class derived from AlgoBase to perform
 * WATERMARK-specific operations.
 */
class WaterMarkAlgorithm : public AlgoBase {
public:
  /**
   * @brief Constructor for WaterMarkAlgorithm.
   *
   */
  WaterMarkAlgorithm();

  /**
   * @brief Destructor for WaterMarkAlgorithm.
   */
  ~WaterMarkAlgorithm() override;

  /**
   * @brief Open the WATERMARK algorithm, simulating resource checks.
   * @return Status of the operation.
   */
  AlgoStatus Open() override;

  /**
   * @brief Process the WATERMARK algorithm, simulating WATERMARK computation.
   * @return Status of the operation.
   */
  AlgoStatus Process(std::shared_ptr<AlgoRequest> req) override;

  /**
   * @brief Close the WATERMARK algorithm, simulating cleanup.
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
  mutable std::mutex mutex_;     // Mutex to protect the shared state
#ifdef _CV_ENABLED_
  cv::Mat image_;                // Original image
  cv::Mat watermark_;            // Watermark (text or image)
#endif
  std::string watermarkText;     // If you want to apply text watermark
  std::string watermarkLogoPath; // If you want to apply logo

  AlgoStatus ProcessRGB(std::shared_ptr<AlgoRequest> req);
  AlgoStatus ProcessYUV(std::shared_ptr<AlgoRequest> req);
};

/**
 * @brief Factory function to expose WaterMarkAlgorithm via shared library.
 * @return A pointer to the WaterMarkAlgorithm instance.
 */
extern "C" AlgoBase *GetAlgoMethod();

#endif // WATERMARK_ALGORITHM_H