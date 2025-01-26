/*
 * Copyright (c) [2025] [Uma Mahesh B]
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
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

/**
 * @brief Process the Nop algorithm, simulating input validation and Nop
 * computation.
 * @return Status of the operation.
 */
AlgoBase::AlgoStatus
FilterAlgorithm::Process(std::shared_ptr<AlgoRequest> req) {

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

/**
 * @brief max time taken by algo to process a request
 *
 * @return int
 */
int FilterAlgorithm::GetTimeout() { return 1000; }

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
extern "C" const char *GetAlgorithmName() { return FILTER_NAME; }
