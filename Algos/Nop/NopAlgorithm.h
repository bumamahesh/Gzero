#ifndef NOP_ALGORITHM_H
#define NOP_ALGORITHM_H

#include "AlgoBase.h"
const char *NOP_NAME = "NopAlgorithm";

/**
 * @brief NopAlgorithm class derived from AlgoBase to perform NOP-specific
 * operations.
 */
class NopAlgorithm : public AlgoBase {
public:
  /**
   * @brief Constructor for NopAlgorithm.
   *
   */
  NopAlgorithm();

  /**
   * @brief Destructor for NopAlgorithm.
   */
  ~NopAlgorithm() override;

  /**
   * @brief Open the NOP algorithm, simulating resource checks.
   * @return Status of the operation.
   */
  AlgoStatus Open() override;

  /**
   * @brief Process the NOP algorithm, simulating NOP computation.
   * @return Status of the operation.
   */
  AlgoStatus Process(std::shared_ptr<AlgoRequest> req) override;

  /**
   * @brief Close the NOP algorithm, simulating cleanup.
   * @return Status of the operation.
   */
  AlgoStatus Close() override;
  // cppcheck-suppress virtualCallInConstructor

private:
  mutable std::mutex mutex_; // Mutex to protect the shared state
};

/**
 * @brief Factory function to expose NopAlgorithm via shared library.
 * @return A pointer to the NopAlgorithm instance.
 */
extern "C" AlgoBase *GetAlgoMethod();

#endif // NOP_ALGORITHM_H