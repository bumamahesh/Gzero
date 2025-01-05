#ifndef BOKEH_ALGORITHM_H
#define BOKEH_ALGORITHM_H

#include "AlgoBase.h"

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
  AlgoStatus Process() override;

  /**
   * @brief Close the BOKEH algorithm, simulating cleanup.
   * @return Status of the operation.
   */
  AlgoStatus Close() override;
  // cppcheck-suppress virtualCallInConstructor

private:
  mutable std::mutex mutex_; // Mutex to protect the shared state
};

/**
 * @brief Factory function to expose BokehAlgorithm via shared library.
 * @return A pointer to the BokehAlgorithm instance.
 */
extern "C" AlgoBase *GetAlgoMethod();

#endif // BOKEH_ALGORITHM_H