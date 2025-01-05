// HDRAlgorithm.cpp

#include "HDRAlgorithm.h"
/**
 * @brief Constructor for HDRAlgorithm.
 * @param name Name of the HDR algorithm.
 */
HDRAlgorithm::HDRAlgorithm() : AlgoBase(HDR_NAME) {
  mAlgoId = ALGO_HDR; // Unique ID for HDR algorithm
}

/**
 * @brief Destructor for HDRAlgorithm.
 */
HDRAlgorithm::~HDRAlgorithm() {
  StopAlgoThread();
  Close();
};

/**
 * @brief Open the HDR algorithm, simulating resource checks.
 * @return Status of the operation.
 */
AlgoBase::AlgoStatus HDRAlgorithm::Open() {
  std::lock_guard<std::mutex> lock(mutex_); // Protect the shared state

  SetStatus(AlgoStatus::SUCCESS);
  return GetStatus();
}

// static int i = 0;
/**
 * @brief Process the HDR algorithm, simulating input validation and HDR
 * computation.
 * @return Status of the operation.
 */
AlgoBase::AlgoStatus HDRAlgorithm::Process() {
  std::lock_guard<std::mutex> lock(mutex_);

  SetStatus(AlgoStatus::SUCCESS);
  return GetStatus();
}

/**
 * @brief Close the HDR algorithm, simulating cleanup.
 * @return Status of the operation.
 */
AlgoBase::AlgoStatus HDRAlgorithm::Close() {
  std::lock_guard<std::mutex> lock(mutex_); // Protect the shared state

  SetStatus(AlgoStatus::SUCCESS);
  return GetStatus();
}

// Public Exposed API for HDR
/**
 * @brief Factory function to expose HDRAlgorithm via shared library.
 * @return A pointer to the HDRAlgorithm instance.
 */
extern "C" AlgoBase *GetAlgoMethod() {
  HDRAlgorithm *pInstance = new HDRAlgorithm();
  return pInstance;
}

/**
@brief Get the algorithm ID.
 *
 */
extern "C" AlgoId GetAlgoId() { return ALGO_HDR; }
/**
@brief Get the algorithm name.
 *
 */
extern "C" std::string GetAlgorithmName() { return HDR_NAME; }
