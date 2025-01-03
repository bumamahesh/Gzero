// BokehAlgorithm.cpp

#include "BokehAlgorithm.h"
#include <iostream>
static const std::string BOKEH_ALGO_NAME = "BokehAlgorithm";
static const size_t BOKEH_ALGO_ID = 0XCAFEBABE + 1;
/**
 * @brief Constructor for BokehAlgorithm.
 * @param name Name of the BOKEH algorithm.
 */
BokehAlgorithm::BokehAlgorithm() : AlgoBase(BOKEH_ALGO_NAME) {
  algo_id_ = BOKEH_ALGO_ID; // Unique ID for BOKEH algorithm
}

/**
 * @brief Destructor for BokehAlgorithm.
 */
BokehAlgorithm::~BokehAlgorithm() = default;

/**
 * @brief Open the BOKEH algorithm, simulating resource checks.
 * @return Status of the operation.
 */
AlgoBase::AlgoStatus BokehAlgorithm::Open() {
  std::lock_guard<std::mutex> lock(mutex_); // Protect the shared state
  if (is_open_called_) {
    SetStatus(AlgoStatus::ALREADY_OPEN);
    return GetStatus();
  }
  if (!resources_available_) {
    SetStatus(AlgoStatus::RESOURCE_UNAVAILABLE);
    return GetStatus();
  }
  is_open_called_ = true;
  SetStatus(AlgoStatus::SUCCESS);
  return GetStatus();
}

// static int i = 0;
/**
 * @brief Process the BOKEH algorithm, simulating input validation and BOKEH
 * computation.
 * @return Status of the operation.
 */
AlgoBase::AlgoStatus BokehAlgorithm::Process() {
  if (!is_open_called_) {
    SetStatus(AlgoStatus::NOT_INITIALIZED);
    return GetStatus();
  }
  if (input_data_invalid_) {
    SetStatus(AlgoStatus::INVALID_INPUT);
    return GetStatus();
  }

  // Simulate BOKEH computation logic here
  // std::cout << "Processing BOKEH Algorithm..." << i++ << std::endl;
  // is_process_called_ = true;
  SetStatus(AlgoStatus::SUCCESS);
  return GetStatus();
}

/**
 * @brief Close the BOKEH algorithm, simulating cleanup.
 * @return Status of the operation.
 */
AlgoBase::AlgoStatus BokehAlgorithm::Close() {
  std::lock_guard<std::mutex> lock(mutex_); // Protect the shared state
  if (!is_open_called_) {
    SetStatus(AlgoStatus::NOT_INITIALIZED);
    return GetStatus();
  }
  if (is_close_called_) {
    SetStatus(AlgoStatus::ALREADY_CLOSED);
    return GetStatus();
  }
  is_close_called_ = true;
  SetStatus(AlgoStatus::SUCCESS);
  return GetStatus();
}

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
extern "C" size_t GetAlgoId() { return BOKEH_ALGO_ID; }
/**
@brief Get the algorithm name.
 *
 */
extern "C" std::string GetAlgorithmName() { return BOKEH_ALGO_NAME; }
