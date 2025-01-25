#ifndef FILTER_ALGORITHM_H
#define FILTER_ALGORITHM_H

#include "AlgoBase.h"
const char *FILTER_NAME = "FilterAlgorithm";
/**
 * @brief FilterAlgorithm class derived from AlgoBase to perform Filter-specific
 * operations.
 */
class FilterAlgorithm : public AlgoBase {
public:
  /**
   * @brief Constructor for FilterAlgorithm.
   *
   */
  FilterAlgorithm();

  /**
   * @brief Destructor for FilterAlgorithm.
   */
  ~FilterAlgorithm() override;

  /**
   * @brief Open the Filter algorithm, simulating resource checks.
   * @return Status of the operation.
   */
  AlgoStatus Open() override;

  /**
   * @brief Process the Filter algorithm, simulating Filter computation.
   * @return Status of the operation.
   */
  AlgoStatus Process(std::shared_ptr<AlgoRequest> req) override;

  /**
   * @brief Close the Filter algorithm, simulating cleanup.
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
  mutable std::mutex mutex_; // Mutex to protect the shared state
};

/**
 * @brief Factory function to expose FilterAlgorithm via shared library.
 * @return A pointer to the FilterAlgorithm instance.
 */
extern "C" AlgoBase *GetAlgoMethod();

#endif // FILTER_ALGORITHM_H