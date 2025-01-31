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
#include "WaterMarkAlgorithm.h"
#include "Log.h"
//#include <unistd.h>

/**
 * @brief Constructor for WaterMarkAlgorithm.
 * @param name Name of the WaterMark algorithm.
 */
WaterMarkAlgorithm::WaterMarkAlgorithm() : AlgoBase(WATERMARK_NAME) {
  mAlgoId = ALGO_WATERMARK; // Unique ID for WaterMark algorithm
  watermarkText_ = "Sample Watermark";
  SupportedFormatsMap.push_back({ImageFormat::RGB, ImageFormat::RGB});
}

/**
 * @brief Destructor for WaterMarkAlgorithm.
 */
WaterMarkAlgorithm::~WaterMarkAlgorithm() {
  StopAlgoThread();
  Close();
};

/**
 * @brief Open the WaterMark algorithm, simulating resource checks.
 * @return Status of the operation.
 */
AlgoBase::AlgoStatus WaterMarkAlgorithm::Open() {
  std::lock_guard<std::mutex> lock(mutex_); // Protect the shared state

  SetStatus(AlgoStatus::SUCCESS);
  return GetAlgoStatus();
}

cv::Point GetWatermarkPosition(WatermarkPosition position, int width,
                               int height) {
  switch (position) {
  case WatermarkPosition::TOP_LEFT:
    return cv::Point(width / 10, height / 10);
  case WatermarkPosition::TOP_RIGHT:
    return cv::Point(width - width / 4, height / 10);
  case WatermarkPosition::BOTTOM_LEFT:
    return cv::Point(width / 10, height - height / 10);
  case WatermarkPosition::BOTTOM_RIGHT:
    return cv::Point(width - width / 4, height - height / 10);
  default:
    return cv::Point(width / 10, height / 10);
  }
}
/**
 * @brief Process the WaterMark algorithm, simulating input validation and
 * WaterMark computation.
 * @return Status of the operation.
 */
AlgoBase::AlgoStatus
WaterMarkAlgorithm::Process(std::shared_ptr<AlgoRequest> req) {
  LOG(VERBOSE, ALGOBASE, "Entered WaterMarkAlgorithm::Process %ld",
      req->mProcessCnt);

  auto inputImage = req->GetImage(0); // Assume the first image as input
  if (!inputImage) {
    SetStatus(AlgoStatus::FAILURE);
    return GetAlgoStatus();
  }

  const ImageFormat inputFormat = inputImage->GetFormat();
  const int width = inputImage->GetWidth();
  const int height = inputImage->GetHeight();

  if (true == CanProcessFormat(inputFormat, ImageFormat::RGB)) {

    // Convert input data to OpenCV Mat
    cv::Mat rgbImage(height, width, CV_8UC3, inputImage->GetData().data());

    // Add watermark with configurable position
    std::string watermarkText = "Uma Mahesh B";
    int fontFace = cv::FONT_HERSHEY_SIMPLEX;
    double fontScale = 1.0;
    int thickness = 2;
    WatermarkPosition position =
        WatermarkPosition::BOTTOM_RIGHT; // Set desired position
    cv::Point textOrg = GetWatermarkPosition(position, width, height);
    cv::Scalar color(255, 255, 255); // White watermark
    cv::putText(rgbImage, watermarkText, textOrg, fontFace, fontScale, color,
                thickness);

    // Store the modified image data in a buffer
    std::vector<uint8_t> outputData(rgbImage.total() * rgbImage.elemSize());
    memcpy(outputData.data(), rgbImage.data, outputData.size());

    // Replace input image with output image in req
    req->ClearImages();
    req->AddImage(ImageFormat::RGB, width, height, outputData);
  } else {
    // skip processing
  }

  SetStatus(AlgoStatus::SUCCESS);
  return GetAlgoStatus();
}

/**
 * @brief Close the WaterMark algorithm, simulating cleanup.
 * @return Status of the operation.
 */
AlgoBase::AlgoStatus WaterMarkAlgorithm::Close() {
  std::lock_guard<std::mutex> lock(mutex_); // Protect the shared state

  SetStatus(AlgoStatus::SUCCESS);
  return GetAlgoStatus();
}

/**
 * @brief max time taken by algo to process a request
 *
 * @return int
 */
int WaterMarkAlgorithm::GetTimeout() { return 1000; }

// Public Exposed API for WaterMark
/**
 * @brief Factory function to expose WaterMarkAlgorithm via shared library.
 * @return A pointer to the WaterMarkAlgorithm instance.
 */
extern "C" AlgoBase *GetAlgoMethod() {
  WaterMarkAlgorithm *pInstance = new WaterMarkAlgorithm();
  return pInstance;
}

/**
@brief Get the algorithm ID.
 *
 */
extern "C" AlgoId GetAlgoId() { return ALGO_WATERMARK; }
/**
@brief Get the algorithm name.
 *
 */
extern "C" const char *GetAlgorithmName() { return WATERMARK_NAME; }
