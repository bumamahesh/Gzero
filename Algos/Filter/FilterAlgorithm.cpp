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
#include "FilterAlgorithm.h"
#include "Log.h"
#include <cmath>

/**
 * @brief Constructor for FilterAlgorithm.
 * @param name Name of the Nop algorithm.
 */
FilterAlgorithm::FilterAlgorithm() : AlgoBase(FILTER_NAME) {
  mAlgoId = ALGO_FILTER; // Unique ID for Nop algorithm
  SupportedFormatsMap.push_back({ImageFormat::RGB, ImageFormat::RGB});
}

/**
 * @brief Destructor for FilterAlgorithm.
 */
FilterAlgorithm::~FilterAlgorithm() {
  StopAlgoThread();
  Close();
};

/**
 * @brief Open the Nop algorithm, simulating resource checks.
 * @return Status of the operation.
 */
AlgoBase::AlgoStatus FilterAlgorithm::Open() {
  std::lock_guard<std::mutex> lock(mutex_); // Protect the shared state

  SetStatus(AlgoStatus::SUCCESS);
  return GetAlgoStatus();
}

/**
 * @brief Process the Nop algorithm, simulating input validation and Nop
 * computation.
 * @return Status of the operation.
 */
AlgoBase::AlgoStatus
FilterAlgorithm::Process(std::shared_ptr<AlgoRequest> req) {
  std::lock_guard<std::mutex> lock(mutex_);
  // KpiMonitor kpi("SobelFilter::Process");

  if (!req || req->GetImageCount() == 0) {
    SetStatus(AlgoStatus::FAILURE);
    return GetAlgoStatus();
  }

  auto inputImage = req->GetImage(0); // Assume the first image as input
  if (!inputImage) {
    SetStatus(AlgoStatus::FAILURE);
    return GetAlgoStatus();
  }

  const ImageFormat inputFormat = inputImage->GetFormat();
  const int width = inputImage->GetWidth();
  const int height = inputImage->GetHeight();
  const std::vector<unsigned char> &inputData =
      inputImage->GetData(); // RGB input assumed

  if (inputData.size() != static_cast<size_t>(width * height * 3)) {
    SetStatus(AlgoStatus::FAILURE);
    return GetAlgoStatus();
  }
  if (true == CanProcessFormat(inputFormat, ImageFormat::RGB)) {
    std::vector<unsigned char> outputData(width * height * 3, 0); // RGB output

    // Sobel kernels for X and Y gradients
    const int Gx[3][3] = {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}};

    const int Gy[3][3] = {{1, 2, 1}, {0, 0, 0}, {-1, -2, -1}};

    // Compute Sobel filter sequentially for each color channel
    for (int y = 1; y < height - 1; ++y) {
      for (int x = 1; x < width - 1; ++x) {
        int gradientX[3] = {0, 0, 0};
        int gradientY[3] = {0, 0, 0};

        // Apply Sobel kernels for each color channel
        for (int ky = -1; ky <= 1; ++ky) {
          for (int kx = -1; kx <= 1; ++kx) {
            int pixelIndex = ((y + ky) * width + (x + kx)) * 3;
            for (int c = 0; c < 3; ++c) { // Iterate over R, G, B channels
              int pixelValue = inputData[pixelIndex + c];
              gradientX[c] += pixelValue * Gx[ky + 1][kx + 1];
              gradientY[c] += pixelValue * Gy[ky + 1][kx + 1];
            }
          }
        }

        // Compute gradient magnitude for each channel and clamp to 255
        int outputIndex = (y * width + x) * 3;
        for (int c = 0; c < 3; ++c) {
          int magnitude = static_cast<int>(std::sqrt(
              gradientX[c] * gradientX[c] + gradientY[c] * gradientY[c]));
          outputData[outputIndex + c] =
              static_cast<unsigned char>(std::min(magnitude, 255));
        }
      }
    }

    // Replace input image with output image
    req->ClearImages();
    req->AddImage(ImageFormat::RGB, width, height, outputData);
  } else {
    // skip processing
  }
  SetStatus(AlgoStatus::SUCCESS);
  return GetAlgoStatus();
}

/**
 * @brief Close the Filter algorithm, simulating cleanup.
 * @return Status of the operation.
 */
AlgoBase::AlgoStatus FilterAlgorithm::Close() {
  std::lock_guard<std::mutex> lock(mutex_); // Protect the shared state

  SetStatus(AlgoStatus::SUCCESS);
  return GetAlgoStatus();
}
/**
 * @brief max time taken by algo to process a request
 *
 * @return int
 */
int FilterAlgorithm::GetTimeout() { return 5000; }

// Public Exposed API for Nop
/**
 * @brief Factory function to expose FilterAlgorithm via shared library.
 * @return A pointer to the FilterAlgorithm instance.
 */
extern "C" AlgoBase *GetAlgoMethod() {
  FilterAlgorithm *pInstance = new FilterAlgorithm();
  return pInstance;
}

/**
@brief Get the algorithm ID.
 *
 */
extern "C" AlgoId GetAlgoId() { return ALGO_FILTER; }
/**
@brief Get the algorithm name.
 *
 */
extern "C" const char *GetAlgorithmName() { return FILTER_NAME; }
