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
#include "LdcAlgorithm.h"
#include "ConfigParser.h"
#include "Log.h"
#ifdef __OPENCV_ENABLE__
#include <cmath>
#include <opencv2/opencv.hpp>
#endif

/**
 * @brief Constructor for LdcAlgorithm.
 * @param name Name of the ldc algorithm.
 */
LdcAlgorithm::LdcAlgorithm() : AlgoBase(LDC_NAME) {
  mAlgoId = ALGO_LDC;  // Unique ID for ldc algorithm

  SupportedFormatsMap.push_back({ImageFormat::YUV420, ImageFormat::YUV420});
  SupportedFormatsMap.push_back({ImageFormat::RGB, ImageFormat::RGB});
  SupportedFormatsMap.push_back(
      {ImageFormat::GRAYSCALE, ImageFormat::GRAYSCALE});
  ConfigParser parser;
  mConfigFile = CONFIGPATH;
  mConfigFile += AlgoBase::GetAlgorithmName();
  mConfigFile += ".config";
  parser.loadFile(mConfigFile.c_str());
  std::string Version = parser.getValue("Version");
  if (parser.getErrorCode() == 0) {
    LOG(VERBOSE, ALGOBASE, "Ldc Algo Version: %s", Version.c_str());
  }
}

/**
 * @brief Destructor for LdcAlgorithm.
 */
LdcAlgorithm::~LdcAlgorithm() {
  StopAlgoThread();
  Close();
};

/**
 * @brief Open the ldc algorithm, simulating resource checks.
 * @return Status of the operation.
 */
AlgoBase::AlgoStatus LdcAlgorithm::Open() {
  std::lock_guard<std::mutex> lock(mutex_);  // Protect the shared state

  SetStatus(AlgoStatus::SUCCESS);
  return GetAlgoStatus();
}

#if __OPENCV_ENABLE__
/**
 * @brief Get the Transformation Matrix object
 * 
 * @param scale 
 * @param rotationDeg 
 * @param tx 
 * @param ty 
 * @return cv::Mat 
 */
cv::Mat GetTransformationMatrix(float scale = 1.0, float rotationDeg = 0.0,
                                float tx = 0.0, float ty = 0.0) {
  // Define source points (original image corners)
  cv::Point2f srcPoints[4] = {
      cv::Point2f(0, 0),  // Top-left
      cv::Point2f(1, 0),  // Top-right
      cv::Point2f(1, 1),  // Bottom-right
      cv::Point2f(0, 1)   // Bottom-left
  };

  // Convert rotation from degrees to radians
  float rotationRad = rotationDeg * (CV_PI / 180.0);

  // Compute transformed destination points
  cv::Point2f dstPoints[4];
  for (int i = 0; i < 4; ++i) {
    // Apply scaling and rotation
    float x = srcPoints[i].x - 0.5;  // Centering
    float y = srcPoints[i].y - 0.5;

    float xNew = scale * (x * cos(rotationRad) - y * sin(rotationRad));
    float yNew = scale * (x * sin(rotationRad) + y * cos(rotationRad));

    // Apply translation
    dstPoints[i] = cv::Point2f(xNew + 0.5 + tx, yNew + 0.5 + ty);
  }

  // Compute perspective transformation matrix
  return cv::getPerspectiveTransform(srcPoints, dstPoints);
}

#endif

/**
 * @brief Process the ldc algorithm, simulating input validation and ldc
 * computation.
 * @return Status of the operation.
 */
AlgoBase::AlgoStatus LdcAlgorithm::Process(std::shared_ptr<AlgoRequest> req) {
  std::lock_guard<std::mutex> lock(mutex_);

  if (!req || req->GetImageCount() == 0) {
    SetStatus(AlgoStatus::FAILURE);
    return GetAlgoStatus();
  }
#ifdef __OPENCV_ENABLE__
  auto inputImage = req->GetImage(0);  // Assume first image as input
  if (!inputImage) {
    SetStatus(AlgoStatus::FAILURE);
    return GetAlgoStatus();
  }

  const ImageFormat inputFormat = inputImage->GetFormat();
  const int width               = inputImage->GetWidth();
  const int height              = inputImage->GetHeight();

  if (CanProcessFormat(inputFormat, inputFormat)) {
    // Extract YUV data from the input image
    std::vector<unsigned char> yuvData = inputImage->GetData();
    if (yuvData.empty()) {
      SetStatus(AlgoStatus::FAILURE);
      return GetAlgoStatus();
    }

    // Split Y, U, and V planes (Assuming YUV420 format)
    cv::Mat yPlane(height, width, CV_8UC1, yuvData.data());
    cv::Mat uPlane(height / 2, width / 2, CV_8UC1,
                   yuvData.data() + width * height);
    cv::Mat vPlane(
        height / 2, width / 2, CV_8UC1,
        yuvData.data() + width * height + (width / 2) * (height / 2));

    // Get transformation matrix (Assume it’s precomputed)
    static float scale     = 0.0f;
    constexpr float delta  = 0.01f;
    static bool increasing = true;

    if (req->mRequestId % 10 == 0) {
      if (scale >= 1.0f) {
        scale      = 1.0f;
        increasing = false;
      } else if (scale <= 0.0f) {
        scale      = 0.0f;
        increasing = true;
      }

      // Adjust scale based on direction
      scale += (increasing ? delta : -delta);
    }

    cv::Mat transformationMatrix = GetTransformationMatrix(scale, 0.0, 0, 0);

    // Apply LDC transformation on the Y channel
    cv::Mat yPlaneWarped;
    cv::warpPerspective(yPlane, yPlaneWarped, transformationMatrix,
                        yPlane.size());

    // Apply LDC transformation on UV channels using lower resolution
    cv::Mat uPlaneWarped, vPlaneWarped;
    cv::warpPerspective(uPlane, uPlaneWarped, transformationMatrix,
                        uPlane.size());
    cv::warpPerspective(vPlane, vPlaneWarped, transformationMatrix,
                        vPlane.size());

    // Merge back into YUV buffer
    std::vector<unsigned char> outputData(yuvData.size());
    std::memcpy(outputData.data(), yPlaneWarped.data, width * height);
    std::memcpy(outputData.data() + width * height, uPlaneWarped.data,
                (width / 2) * (height / 2));
    std::memcpy(outputData.data() + width * height + (width / 2) * (height / 2),
                vPlaneWarped.data, (width / 2) * (height / 2));

    // Replace input image with transformed YUV image
    req->ClearImages();
    if (req->AddImage(ImageFormat::YUV420, width, height,
                      std::move(outputData))) {
      LOG(ERROR, ALGOBASE, "Error Filling Output data");
      SetStatus(AlgoStatus::FAILURE);
    }
  }
#endif
  // Update metadata to mark process completion
  int reqdone = 0x00;
  if (req &&
      (0 == req->mMetadata.GetMetadata(MetaId::ALGO_PROCESS_DONE, reqdone))) {
    reqdone |= ALGO_MASK(mAlgoId);
    req->mMetadata.SetMetadata(MetaId::ALGO_PROCESS_DONE, reqdone);
  }

  SetStatus(AlgoStatus::SUCCESS);
  return GetAlgoStatus();
}

/**
 * @brief Close the ldc algorithm, simulating cleanup.
 * @return Status of the operation.
 */
AlgoBase::AlgoStatus LdcAlgorithm::Close() {
  std::lock_guard<std::mutex> lock(mutex_);  // Protect the shared state

  SetStatus(AlgoStatus::SUCCESS);
  return GetAlgoStatus();
}

/**
 * @brief max time taken by algo to process a request
 *
 * @return int
 */
int LdcAlgorithm::GetTimeout() {
  return 1000;
}

// Public Exposed API for ldc
/**
 * @brief Factory function to expose LdcAlgorithm via shared library.
 * @return A pointer to the LdcAlgorithm instance.
 */
extern "C" AlgoBase* GetAlgoMethod() {
  LdcAlgorithm* pInstance = new LdcAlgorithm();
  return pInstance;
}

/**
@brief Get the algorithm ID.
 *
 */
extern "C" AlgoId GetAlgoId() {
  return ALGO_LDC;
}
/**
@brief Get the algorithm name.
 *
 */
extern "C" const char* GetAlgorithmName() {
  return LDC_NAME;
}
