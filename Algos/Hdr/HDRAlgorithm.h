#ifndef HDR_ALGORITHM_H
#define HDR_ALGORITHM_H

#include "AlgoBase.h"
#include <string>

/**
 * @brief HDRAlgorithm class derived from AlgoBase to perform HDR-specific
 * operations.
 */
class HDRAlgorithm : public AlgoBase {
public:
  /**
   * @brief Constructor for HDRAlgorithm.
   *
   */
  HDRAlgorithm();

  /**
   * @brief Destructor for HDRAlgorithm.
   */
  ~HDRAlgorithm() override;

  /**
   * @brief Open the HDR algorithm, simulating resource checks.
   * @return Status of the operation.
   */
  AlgoStatus Open() override;

  /**
   * @brief Process the HDR algorithm, simulating HDR computation.
   * @return Status of the operation.
   */
  AlgoStatus Process() override;

  /**
   * @brief Close the HDR algorithm, simulating cleanup.
   * @return Status of the operation.
   */
  AlgoStatus Close() override;

private:
  bool is_open_called_ = false;    ///< Tracks whether Open has been called
  bool is_process_called_ = false; ///< Tracks whether Process has been called
  bool is_close_called_ = false;   ///< Tracks whether Close has been called

  // Simulated conditions for demonstration purposes
  bool resources_available_ = true; ///< Simulate resource availability
  bool input_data_invalid_ = false; ///< Simulate input data validation
  mutable std::mutex mutex_;        // Mutex to protect the shared state
};

/**
 * @brief Factory function to expose HDRAlgorithm via shared library.
 * @return A pointer to the HDRAlgorithm instance.
 */
extern "C" AlgoBase *GetAlgoMethod();

#endif // HDR_ALGORITHM_H