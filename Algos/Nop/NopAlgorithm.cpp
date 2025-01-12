// NopAlgorithm.cpp

#include "NopAlgorithm.h"
/**
 * @brief Constructor for NopAlgorithm.
 * @param name Name of the Nop algorithm.
 */
NopAlgorithm::NopAlgorithm() : AlgoBase(NOP_NAME) {
  mAlgoId = ALGO_NOP; // Unique ID for Nop algorithm
}

/**
 * @brief Destructor for NopAlgorithm.
 */
NopAlgorithm::~NopAlgorithm() {
  StopAlgoThread();
  Close();
};

/**
 * @brief Open the Nop algorithm, simulating resource checks.
 * @return Status of the operation.
 */
AlgoBase::AlgoStatus NopAlgorithm::Open() {
  std::lock_guard<std::mutex> lock(mutex_); // Protect the shared state

  SetStatus(AlgoStatus::SUCCESS);
  return GetAlgoStatus();
}

/**
 * @brief Process the Nop algorithm, simulating input validation and Nop
 * computation.
 * @return Status of the operation.
 */
AlgoBase::AlgoStatus NopAlgorithm::Process(std::shared_ptr<AlgoRequest> req) {

  SetStatus(AlgoStatus::SUCCESS);
  return GetAlgoStatus();
}

/**
 * @brief Close the Nop algorithm, simulating cleanup.
 * @return Status of the operation.
 */
AlgoBase::AlgoStatus NopAlgorithm::Close() {
  std::lock_guard<std::mutex> lock(mutex_); // Protect the shared state

  SetStatus(AlgoStatus::SUCCESS);
  return GetAlgoStatus();
}

// Public Exposed API for Nop
/**
 * @brief Factory function to expose NopAlgorithm via shared library.
 * @return A pointer to the NopAlgorithm instance.
 */
extern "C" AlgoBase *GetAlgoMethod() {
  NopAlgorithm *pInstance = new NopAlgorithm();
  return pInstance;
}

/**
@brief Get the algorithm ID.
 *
 */
extern "C" AlgoId GetAlgoId() { return ALGO_NOP; }
/**
@brief Get the algorithm name.
 *
 */
extern "C" const char *GetAlgorithmName() { return NOP_NAME; }
