#ifndef LDC_ALGORITHM_H
#define LDC_ALGORITHM_H

#include "AlgoBase.h"
const char *LDC_NAME = "ldcAlgorithm";

struct CameraIntrinsics {
  std::pair<double, double> focalLength;    // fx, fy
  std::pair<double, double> principalPoint; // cx, cy
  std::vector<double> distortionCoeffs;     // k1, k2, p1, p2, k3
};

struct CameraExtrinsics {
  std::vector<std::vector<double>> rotationMatrix; // 3x3 rotation matrix
  std::vector<double> translationVector;           // tx, ty, tz
};

struct CalibrationData {
  CameraIntrinsics intrinsics; // Camera intrinsic parameters
  CameraExtrinsics extrinsics; // Camera extrinsic parameters

  bool IsValid() const {
    // Check if intrinsics and extrinsics have been initialized properly
    return !intrinsics.distortionCoeffs.empty() &&
           extrinsics.rotationMatrix.size() == 3 &&
           extrinsics.rotationMatrix[0].size() == 3;
  }
};

/**
 * @brief ldcAlgorithm class derived from AlgoBase to perform LDC-specific
 * operations.
 */
class ldcAlgorithm : public AlgoBase {
public:
  /**
   * @brief Constructor for ldcAlgorithm.
   *
   */
  ldcAlgorithm();

  /**
   * @brief Destructor for ldcAlgorithm.
   */
  ~ldcAlgorithm() override;

  /**
   * @brief Open the LDC algorithm, simulating resource checks.
   * @return Status of the operation.
   */
  AlgoStatus Open() override;

  /**
   * @brief Process the LDC algorithm, simulating LDC computation.
   * @return Status of the operation.
   */
  AlgoStatus Process(std::shared_ptr<AlgoRequest> req) override;

  /**
   * @brief Close the LDC algorithm, simulating cleanup.
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

  CameraIntrinsics ComputeCameraIntrinsics(
      const std::vector<std::shared_ptr<ImageData>> &yuvImages);

  CameraExtrinsics ComputeCameraExtrinsics(
      const std::vector<std::shared_ptr<ImageData>> &yuvImages);

  void CalculateCalibrationData(std::shared_ptr<AlgoRequest> req);

private:
  mutable std::mutex mutex_; // Mutex to protect the shared state

  bool CalculateDistortionCoefficients(
      const std::vector<std::vector<std::vector<uint8_t>>> &calibrationImages);

  double m_k1 = 0.0, m_k2 = 0.0, m_k3 = 0.0,
         m_k4 = 0.0; // Distortion coefficients
};

/**
 * @brief Factory function to expose ldcAlgorithm via shared library.
 * @return A pointer to the ldcAlgorithm instance.
 */
extern "C" AlgoBase *GetAlgoMethod();

#endif // LDC_ALGORITHM_H