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
  parser.loadFile("/home/uma/workspace/Gzero/Config/LdcAlgorithm.config");
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

/**
 * @brief Process the ldc algorithm, simulating input validation and ldc
 * computation.
 * @return Status of the operation.
 */
AlgoBase::AlgoStatus LdcAlgorithm::Process(std::shared_ptr<AlgoRequest> req) {

  // Perform calibration data calculation
  CalculateCalibrationData(req);

  // Perform image undistortion
  // UndistortImages(req);
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

CameraIntrinsics LdcAlgorithm::ComputeCameraIntrinsics(
    const std::vector<std::shared_ptr<ImageData>>& yuvImages) {
  (void)yuvImages;
  // Placeholder for actual camera intrinsics computation logic
  CameraIntrinsics intrinsics;
  intrinsics.focalLength      = {1000.0, 1000.0};
  intrinsics.principalPoint   = {640.0, 480.0};
  intrinsics.distortionCoeffs = {0.1, -0.05, 0.0, 0.0, 0.0};
  return intrinsics;
}

CameraExtrinsics LdcAlgorithm::ComputeCameraExtrinsics(
    const std::vector<std::shared_ptr<ImageData>>& yuvImages) {
  (void)yuvImages;
  // Placeholder for actual camera extrinsics computation logic
  CameraExtrinsics extrinsics;
  extrinsics.rotationMatrix = {
      {1.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {0.0, 0.0, 1.0}};
  extrinsics.translationVector = {0.0, 0.0, 0.0};
  return extrinsics;
}

void LdcAlgorithm::CalculateCalibrationData(std::shared_ptr<AlgoRequest> req) {
  if (!req) {
    return;
  }
  // Ensure that there are images to process
  if (req->GetImageCount() == 0) {
    SetStatus(AlgoStatus::FAILURE);
    throw std::runtime_error("No images available for calibration.");
  }

  // Example: Use only YUV images for calibration
  auto yuvImages = req->GetYUVImages();
  if (yuvImages.empty()) {
    SetStatus(AlgoStatus::FAILURE);
    throw std::runtime_error("No YUV images available for calibration.");
  }

  // Perform the calibration algorithm (e.g., camera intrinsics, extrinsics)
  CalibrationData calibrationData;
  calibrationData.intrinsics = ComputeCameraIntrinsics(yuvImages);
  calibrationData.extrinsics = ComputeCameraExtrinsics(yuvImages);

  // Store calibration data for later usage
  // req->SetCalibrationData(calibrationData);

  // Log progress
  // Log("Calibration data calculation completed successfully.");
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
