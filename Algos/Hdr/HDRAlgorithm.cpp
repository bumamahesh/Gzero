// HDRAlgorithm.cpp

#include "HDRAlgorithm.h"
#include <iostream>
/**
 * @brief Constructor for HDRAlgorithm.
 * @param name Name of the HDR algorithm.
 */
HDRAlgorithm::HDRAlgorithm(const std::string &name) : AlgoBase(name) {
  algo_id_ = 0xCAFEBABE; // Unique ID for HDR algorithm
}

/**
 * @brief Destructor for HDRAlgorithm.
 */
HDRAlgorithm::~HDRAlgorithm() {}

/**
 * @brief Open the HDR algorithm, simulating resource checks.
 * @return Status of the operation.
 */
AlgoBase::AlgoStatus HDRAlgorithm::Open() {
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
 * @brief Process the HDR algorithm, simulating input validation and HDR
 * computation.
 * @return Status of the operation.
 */
AlgoBase::AlgoStatus HDRAlgorithm::Process() {
  if (!is_open_called_) {
    SetStatus(AlgoStatus::NOT_INITIALIZED);
    return GetStatus();
  }
  if (input_data_invalid_) {
    SetStatus(AlgoStatus::INVALID_INPUT);
    return GetStatus();
  }

  // Simulate HDR computation logic here
  // std::cout << "Processing HDR Algorithm..." << i++ << std::endl;
  // is_process_called_ = true;
  SetStatus(AlgoStatus::SUCCESS);
  return GetStatus();
}

/**
 * @brief Close the HDR algorithm, simulating cleanup.
 * @return Status of the operation.
 */
AlgoBase::AlgoStatus HDRAlgorithm::Close() {
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

/**
 * @brief Factory function to expose HDRAlgorithm via shared library.
 * @return A pointer to the HDRAlgorithm instance.
 */
extern "C" AlgoBase *GetAlgoMethod() {
  HDRAlgorithm *pInstance = new HDRAlgorithm("HDRAlgorithm");
  return pInstance;
}
