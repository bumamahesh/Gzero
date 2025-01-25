#ifndef HDR_ALGORITHM_H
#define HDR_ALGORITHM_H

#include "AlgoBase.h"
const char *MANDELBROTSET_NAME = "MandelbrotSetAlgorithm";

// Constants for Mandelbrot calculation
constexpr int MAX_ITER = 500; // Maximum iterations for escape condition
constexpr double INITIAL_ZOOM = 1.0;
constexpr double ZOOM_FACTOR = 1.2;
constexpr double CENTER_X = -0.75;
constexpr double CENTER_Y = 0.0;

/**
 * @brief MandelbrotSet class derived from AlgoBase to perform HDR-specific
 * operations.
 */
class MandelbrotSet : public AlgoBase {
public:
  /**
   * @brief Constructor for MandelbrotSet.
   *
   */
  MandelbrotSet();

  /**
   * @brief Destructor for MandelbrotSet.
   */
  ~MandelbrotSet() override;

  /**
   * @brief Open the HDR algorithm, simulating resource checks.
   * @return Status of the operation.
   */
  AlgoStatus Open() override;

  /**
   * @brief Process the HDR algorithm, simulating HDR computation.
   * @return Status of the operation.
   */
  AlgoStatus Process(std::shared_ptr<AlgoRequest> req) override;

  /**
   * @brief Close the HDR algorithm, simulating cleanup.
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
  mutable std::mutex mutex_; // Mutex to protect the shared state
  double zoomLevel;
  double offsetX;
  double offsetY;
};

/**
 * @brief Factory function to expose MandelbrotSet via shared library.
 * @return A pointer to the MandelbrotSet instance.
 */
extern "C" AlgoBase *GetAlgoMethod();

#endif // HDR_ALGORITHM_H