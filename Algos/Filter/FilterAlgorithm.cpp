// FilterAlgorithm.cpp

#include "FilterAlgorithm.h"
/**
 * @brief Constructor for FilterAlgorithm.
 * @param name Name of the Nop algorithm.
 */
FilterAlgorithm::FilterAlgorithm() : AlgoBase(FILTER_NAME) {
  mAlgoId = ALGO_FILTER; // Unique ID for Nop algorithm
}

/**
 * @brief Destructor for FilterAlgorithm.
 */
FilterAlgorithm::~FilterAlgorithm() {
  StopAlgoThread();
  Close();
};

/**
 * @brief Open the Nop algorithm, simulating resource checks.
 * @return Status of the operation.
 */
AlgoBase::AlgoStatus FilterAlgorithm::Open() {
  std::lock_guard<std::mutex> lock(mutex_); // Protect the shared state

  SetStatus(AlgoStatus::SUCCESS);
  return GetAlgoStatus();
}

// static int i = 0;
/**
 * @brief Process the Nop algorithm, simulating input validation and Nop
 * computation.
 * @return Status of the operation.
 */
AlgoBase::AlgoStatus FilterAlgorithm::Process() {

  SetStatus(AlgoStatus::SUCCESS);
  return GetAlgoStatus();
}

/**
 * @brief Close the Nop algorithm, simulating cleanup.
 * @return Status of the operation.
 */
AlgoBase::AlgoStatus FilterAlgorithm::Close() {
  std::lock_guard<std::mutex> lock(mutex_); // Protect the shared state

  SetStatus(AlgoStatus::SUCCESS);
  return GetAlgoStatus();
}

// Public Exposed API for Nop
/**
 * @brief Factory function to expose FilterAlgorithm via shared library.
 * @return A pointer to the FilterAlgorithm instance.
 */
extern "C" AlgoBase *GetAlgoMethod() {
  FilterAlgorithm *pInstance = new FilterAlgorithm();
  return pInstance;
}

/**
@brief Get the algorithm ID.
 *
 */
extern "C" AlgoId GetAlgoId() { return ALGO_FILTER; }
/**
@brief Get the algorithm name.
 *
 */
extern "C" std::string GetAlgorithmName() { return FILTER_NAME; }
