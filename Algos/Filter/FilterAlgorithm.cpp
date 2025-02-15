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
#include <cmath>
#include "ConfigParser.h"
#include "Log.h"

/**
 * @brief Constructor for FilterAlgorithm.
 * @param name Name of the Filter algorithm.
 */
FilterAlgorithm::FilterAlgorithm() : AlgoBase(FILTER_NAME) {
  mAlgoId = ALGO_FILTER;  // Unique ID for Filter algorithm
  SupportedFormatsMap.push_back({ImageFormat::RGB, ImageFormat::RGB});
  SupportedFormatsMap.push_back({ImageFormat::YUV420, ImageFormat::YUV420});
  ConfigParser parser;
  mConfigFile = CONFIGPATH;
  mConfigFile += AlgoBase::GetAlgorithmName();
  mConfigFile += ".config";
  parser.loadFile(mConfigFile.c_str());
  std::string Version = parser.getValue("Version");
  if (parser.getErrorCode() == 0) {
    LOG(VERBOSE, ALGOBASE, "Filter Algo Version: %s", Version.c_str());
  }
}

/**
 * @brief Destructor for FilterAlgorithm.
 */
FilterAlgorithm::~FilterAlgorithm() {
  StopAlgoThread();
  Close();
};

/**
 * @brief Open the Filter algorithm, simulating resource checks.
 * @return Status of the operation.
 */
AlgoBase::AlgoStatus FilterAlgorithm::Open() {
  std::lock_guard<std::mutex> lock(mutex_);  // Protect the shared state

  SetStatus(AlgoStatus::SUCCESS);
  return GetAlgoStatus();
}
AlgoBase::AlgoStatus FilterAlgorithm::SobelRGB(
    std::shared_ptr<AlgoRequest> req) {
  auto inputImage = req->GetImage(0);  // Assume the first image as input
  if (!inputImage) {
    SetStatus(AlgoStatus::FAILURE);
    return GetAlgoStatus();
  }
  const int width                             = inputImage->GetWidth();
  const int height                            = inputImage->GetHeight();
  const std::vector<unsigned char>& inputData = inputImage->GetData();

  std::vector<unsigned char> outputData(width * height * 3, 0);  // RGB output

  // Compute Sobel filter sequentially for each color channel
  for (int y = 1; y < height - 1; ++y) {
    for (int x = 1; x < width - 1; ++x) {
      int gradientX[3] = {0, 0, 0};
      int gradientY[3] = {0, 0, 0};

      // Apply Sobel kernels for each color channel
      for (int ky = -1; ky <= 1; ++ky) {
        for (int kx = -1; kx <= 1; ++kx) {
          int pixelIndex = ((y + ky) * width + (x + kx)) * 3;
          for (int c = 0; c < 3; ++c) {  // Iterate over R, G, B channels
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
  if (req->AddImage(ImageFormat::RGB, width, height, std::move(outputData))) {
    LOG(ERROR, ALGOBASE, "Error Filling Output data");
    SetStatus(AlgoStatus::FAILURE);
  }

  return GetAlgoStatus();
}

AlgoBase::AlgoStatus FilterAlgorithm::SobelYuv(
    std::shared_ptr<AlgoRequest> req) {
  auto inputImage = req->GetImage(0);
  if (!inputImage) {
    SetStatus(AlgoStatus::FAILURE);
    return GetAlgoStatus();
  }

  const int width                             = inputImage->GetWidth();
  const int height                            = inputImage->GetHeight();
  const std::vector<unsigned char>& inputData = inputImage->GetData();

  // YUV420 - Y plane size = width * height
  // UV planes = (width * height) / 4 each
  std::vector<unsigned char> outputData(width * height * 3 / 2, 0);

  // Sobel on Y channel only
  for (int y = 1; y < height - 1; ++y) {
    for (int x = 1; x < width - 1; ++x) {
      int gradientX = 0, gradientY = 0;

      for (int ky = -1; ky <= 1; ++ky) {
        for (int kx = -1; kx <= 1; ++kx) {
          int pixelIndex = (y + ky) * width + (x + kx);  // Correct indexing
          int pixelValue = inputData[pixelIndex];        // Y channel
          gradientX += pixelValue * Gx[ky + 1][kx + 1];
          gradientY += pixelValue * Gy[ky + 1][kx + 1];
        }
      }

      int magnitude = static_cast<int>(
          std::sqrt(gradientX * gradientX + gradientY * gradientY));
      outputData[y * width + x] = static_cast<unsigned char>(
          std::min(magnitude, 255));  // Store in Y plane
    }
  }

  // Copy U and V channels from input
  int uvOffset = width * height;
  std::copy(inputData.begin() + uvOffset, inputData.end(),
            outputData.begin() + uvOffset);

  req->ClearImages();
  if (req->AddImage(ImageFormat::YUV420, width, height,
                    std::move(outputData))) {
    LOG(ERROR, ALGOBASE, "Error Filling Output data");
    SetStatus(AlgoStatus::FAILURE);
  }

  return GetAlgoStatus();
}

/**
 * @brief Process the Filter algorithm, simulating input validation and Filter
 * computation.
 * @return Status of the operation.
 */
AlgoBase::AlgoStatus FilterAlgorithm::Process(
    std::shared_ptr<AlgoRequest> req) {
  AlgoBase::AlgoStatus rc = AlgoStatus::FAILURE;
  std::lock_guard<std::mutex> lock(mutex_);
  // KpiMonitor kpi("SobelFilter::Process");

  if (!req || req->GetImageCount() == 0) {
    SetStatus(AlgoStatus::FAILURE);
    return GetAlgoStatus();
  }

  auto inputImage = req->GetImage(0);  // Assume the first image as input
  if (!inputImage) {
    SetStatus(AlgoStatus::FAILURE);
    return GetAlgoStatus();
  }

  const ImageFormat inputFormat = inputImage->GetFormat();

  switch (inputFormat) {
    case ImageFormat::RGB: {
      if (true == CanProcessFormat(inputFormat, ImageFormat::RGB)) {
        rc = SobelRGB(req);
      }
    } break;
    case ImageFormat::YUV420: {
      if (true == CanProcessFormat(inputFormat, ImageFormat::YUV420)) {
        rc = SobelYuv(req);
      }
    } break;
    default:
      break;
  }

  int reqdone = 0x00;
  if (req &&
      (0 == req->mMetadata.GetMetadata(MetaId::ALGO_PROCESS_DONE, reqdone))) {
    reqdone |= ALGO_MASK(mAlgoId);
    req->mMetadata.SetMetadata(MetaId::ALGO_PROCESS_DONE, reqdone);
  }
  return rc;
}

/**
 * @brief Close the Filter algorithm, simulating cleanup.
 * @return Status of the operation.
 */
AlgoBase::AlgoStatus FilterAlgorithm::Close() {
  std::lock_guard<std::mutex> lock(mutex_);  // Protect the shared state

  SetStatus(AlgoStatus::SUCCESS);
  return GetAlgoStatus();
}
/**
 * @brief max time taken by algo to process a request
 *
 * @return int
 */
int FilterAlgorithm::GetTimeout() {
  return 5000;
}

// Public Exposed API for Filter
/**
 * @brief Factory function to expose FilterAlgorithm via shared library.
 * @return A pointer to the FilterAlgorithm instance.
 */
extern "C" AlgoBase* GetAlgoMethod() {
  FilterAlgorithm* pInstance = new FilterAlgorithm();
  return pInstance;
}

/**
@brief Get the algorithm ID.
 *
 */
extern "C" AlgoId GetAlgoId() {
  return ALGO_FILTER;
}
/**
@brief Get the algorithm name.
 *
 */
extern "C" const char* GetAlgorithmName() {
  return FILTER_NAME;
}
