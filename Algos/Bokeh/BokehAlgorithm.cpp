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
#include "BokehAlgorithm.h"
#include "ConfigParser.h"
#include "Log.h"

/**
 * @brief Constructor for BokehAlgorithm.
 * @param name Name of the BOKEH algorithm.
 */
BokehAlgorithm::BokehAlgorithm() : AlgoBase(BOKEH_NAME) {
  mAlgoId = ALGO_BOKEH;  // Unique ID for BOKEH algorithm
  SupportedFormatsMap.push_back({ImageFormat::RGB, ImageFormat::RGB});
  ConfigParser parser;
  parser.loadFile("/home/uma/workspace/Gzero/Config/BokehAlgorithm.config");
  std::string Version = parser.getValue("Version");
  if (parser.getErrorCode() == 0) {
    LOG(VERBOSE, ALGOBASE, "BOKEH Algo Version: %s", Version.c_str());
  }
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
  std::lock_guard<std::mutex> lock(mutex_);  // Protect the shared state

  SetStatus(AlgoStatus::SUCCESS);
  return GetAlgoStatus();
}

/**
 * @brief Process the BOKEH algorithm, simulating input validation and BOKEH
 * computation.
 * @return Status of the operation.
 */
AlgoBase::AlgoStatus BokehAlgorithm::Process(std::shared_ptr<AlgoRequest> req) {

  int reqdone = 0x00;
  if (req && (0 == req->mMetadata.GetMetadata(ALGO_PROCESS_DONE, reqdone))) {
    reqdone |= ALGO_MASK(mAlgoId);
    req->mMetadata.SetMetadata(ALGO_PROCESS_DONE, reqdone);
  }
  SetStatus(AlgoStatus::SUCCESS);
  return GetAlgoStatus();
}

/**
 * @brief Close the BOKEH algorithm, simulating cleanup.
 * @return Status of the operation.
 */
AlgoBase::AlgoStatus BokehAlgorithm::Close() {
  std::lock_guard<std::mutex> lock(mutex_);  // Protect the shared state

  SetStatus(AlgoStatus::SUCCESS);
  return GetAlgoStatus();
}

/**
 * @brief max time taken by algo to process a request
 *
 * @return int
 */
int BokehAlgorithm::GetTimeout() {
  return 1000;
}

// Public Exposed API for BOKEH
/**
 * @brief Factory function to expose BokehAlgorithm via shared library.
 * @return A pointer to the BokehAlgorithm instance.
 */
extern "C" AlgoBase* GetAlgoMethod() {
  BokehAlgorithm* pInstance = new BokehAlgorithm();
  return pInstance;
}

/**
@brief Get the algorithm ID.
 *
 */
extern "C" AlgoId GetAlgoId() {
  return ALGO_BOKEH;
}
/**
@brief Get the algorithm name.
 *
 */
extern "C" const char* GetAlgorithmName() {
  return BOKEH_NAME;
}
