// BokehAlgorithm.cpp

#include "BokehAlgorithm.h"
/**
 * @brief Constructor for BokehAlgorithm.
 * @param name Name of the BOKEH algorithm.
 */
BokehAlgorithm::BokehAlgorithm() : AlgoBase(BOKEH_NAME) {
  mAlgoId = ALGO_BOKEH; // Unique ID for BOKEH algorithm
}

/**
 * @brief Destructor for BokehAlgorithm.
 */
BokehAlgorithm::~BokehAlgorithm() {
  StopAlgoThread();
  Close();
};

/**
 * @brief Open the BOKEH algorithm, simulating resource checks.
 * @return Status of the operation.
 */
AlgoBase::AlgoStatus BokehAlgorithm::Open() {
  std::lock_guard<std::mutex> lock(mutex_); // Protect the shared state

  SetStatus(AlgoStatus::SUCCESS);
  return GetAlgoStatus();
}

/**
 * @brief Process the BOKEH algorithm, simulating input validation and BOKEH
 * computation.
 * @return Status of the operation.
 */
AlgoBase::AlgoStatus BokehAlgorithm::Process(std::shared_ptr<AlgoRequest> req) {

  SetStatus(AlgoStatus::SUCCESS);
  return GetAlgoStatus();
}

/**
 * @brief Close the BOKEH algorithm, simulating cleanup.
 * @return Status of the operation.
 */
AlgoBase::AlgoStatus BokehAlgorithm::Close() {
  std::lock_guard<std::mutex> lock(mutex_); // Protect the shared state

  SetStatus(AlgoStatus::SUCCESS);
  return GetAlgoStatus();
}

/**
 * @brief max time taken by algo to process a request
 *
 * @return int
 */
int BokehAlgorithm::GetTimeout() { return 1000; }

// Public Exposed API for BOKEH
/**
 * @brief Factory function to expose BokehAlgorithm via shared library.
 * @return A pointer to the BokehAlgorithm instance.
 */
extern "C" AlgoBase *GetAlgoMethod() {
  BokehAlgorithm *pInstance = new BokehAlgorithm();
  return pInstance;
}

/**
@brief Get the algorithm ID.
 *
 */
extern "C" AlgoId GetAlgoId() { return ALGO_BOKEH; }
/**
@brief Get the algorithm name.
 *
 */
extern "C" const char *GetAlgorithmName() { return BOKEH_NAME; }
