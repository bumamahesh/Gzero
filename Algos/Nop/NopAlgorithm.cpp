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
#include "NopAlgorithm.h"
//#include <unistd.h>

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

  // usleep(33 * 1000);
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

/**
 * @brief max time taken by algo to process a request
 *
 * @return int
 */
int NopAlgorithm::GetTimeout() { return 1000; }

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
