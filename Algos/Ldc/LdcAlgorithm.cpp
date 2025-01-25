// ldcAlgorithm.cpp

#include "LdcAlgorithm.h"
/**
 * @brief Constructor for ldcAlgorithm.
 * @param name Name of the ldc algorithm.
 */
ldcAlgorithm::ldcAlgorithm() : AlgoBase(LDC_NAME) {
  mAlgoId = ALGO_LDC; // Unique ID for ldc algorithm
}

/**
 * @brief Destructor for ldcAlgorithm.
 */
ldcAlgorithm::~ldcAlgorithm() {
  StopAlgoThread();
  Close();
};

/**
 * @brief Open the ldc algorithm, simulating resource checks.
 * @return Status of the operation.
 */
AlgoBase::AlgoStatus ldcAlgorithm::Open() {
  std::lock_guard<std::mutex> lock(mutex_); // Protect the shared state

  SetStatus(AlgoStatus::SUCCESS);
  return GetAlgoStatus();
}

/**
 * @brief Process the ldc algorithm, simulating input validation and ldc
 * computation.
 * @return Status of the operation.
 */
AlgoBase::AlgoStatus ldcAlgorithm::Process(std::shared_ptr<AlgoRequest> req) {

  // Perform calibration data calculation
  CalculateCalibrationData(req);

  // Perform image undistortion
  // UndistortImages(req);

  SetStatus(AlgoStatus::SUCCESS);
  return GetAlgoStatus();
}

/**
 * @brief Close the ldc algorithm, simulating cleanup.
 * @return Status of the operation.
 */
AlgoBase::AlgoStatus ldcAlgorithm::Close() {
  std::lock_guard<std::mutex> lock(mutex_); // Protect the shared state

  SetStatus(AlgoStatus::SUCCESS);
  return GetAlgoStatus();
}

/**
 * @brief max time taken by algo to process a request
 *
 * @return int
 */
int ldcAlgorithm::GetTimeout() { return 1000; }

CameraIntrinsics ldcAlgorithm::ComputeCameraIntrinsics(
    const std::vector<std::shared_ptr<ImageData>> &yuvImages) {
  // Placeholder for actual camera intrinsics computation logic
  CameraIntrinsics intrinsics;
  intrinsics.focalLength = {1000.0, 1000.0};
  intrinsics.principalPoint = {640.0, 480.0};
  intrinsics.distortionCoeffs = {0.1, -0.05, 0.0, 0.0, 0.0};
  return intrinsics;
}

CameraExtrinsics ldcAlgorithm::ComputeCameraExtrinsics(
    const std::vector<std::shared_ptr<ImageData>> &yuvImages) {
  // Placeholder for actual camera extrinsics computation logic
  CameraExtrinsics extrinsics;
  extrinsics.rotationMatrix = {
      {1.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {0.0, 0.0, 1.0}};
  extrinsics.translationVector = {0.0, 0.0, 0.0};
  return extrinsics;
}

void ldcAlgorithm::CalculateCalibrationData(std::shared_ptr<AlgoRequest> req) {
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
 * @brief Factory function to expose ldcAlgorithm via shared library.
 * @return A pointer to the ldcAlgorithm instance.
 */
extern "C" AlgoBase *GetAlgoMethod() {
  ldcAlgorithm *pInstance = new ldcAlgorithm();
  return pInstance;
}

/**
@brief Get the algorithm ID.
 *
 */
extern "C" AlgoId GetAlgoId() { return ALGO_LDC; }
/**
@brief Get the algorithm name.
 *
 */
extern "C" const char *GetAlgorithmName() { return LDC_NAME; }
