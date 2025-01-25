#ifndef BOKEH_ALGORITHM_H
#define BOKEH_ALGORITHM_H

#include "AlgoBase.h"

const char *BOKEH_NAME = "BokehAlgorithm";

/**
 * @brief BokehAlgorithm class derived from AlgoBase to perform BOKEH-specific
 * operations.
 */
class BokehAlgorithm : public AlgoBase {
public:
  /**
   * @brief Constructor for BokehAlgorithm.
   *
   */
  BokehAlgorithm();

  /**
   * @brief Destructor for BokehAlgorithm.
   */
  ~BokehAlgorithm() override;

  /**
   * @brief Open the BOKEH algorithm, simulating resource checks.
   * @return Status of the operation.
   */
  AlgoStatus Open() override;

  /**
   * @brief Process the BOKEH algorithm, simulating BOKEH computation.
   * @return Status of the operation.
   */
  AlgoStatus Process(std::shared_ptr<AlgoRequest> req) override;

  /**
   * @brief Close the BOKEH algorithm, simulating cleanup.
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
 * @brief Factory function to expose BokehAlgorithm via shared library.
 * @return A pointer to the BokehAlgorithm instance.
 */
extern "C" AlgoBase *GetAlgoMethod();

#endif // BOKEH_ALGORITHM_H