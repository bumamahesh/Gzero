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
#include "HdrAlgorithm.h"
#include "ConfigParser.h"
#include "Log.h"

/**
 * @brief Constructor for HdrAlgorithm.
 * @param name Name of the Hdr algorithm.
 */
HdrAlgorithm::HdrAlgorithm() : AlgoBase(HDR_NAME) {
  mAlgoId = ALGO_HDR;  // Unique ID for Hdr algorithm
  SupportedFormatsMap.push_back({ImageFormat::RGB, ImageFormat::RGB});
  ConfigParser parser;
  mConfigFile = CONFIGPATH;
  mConfigFile += AlgoBase::GetAlgorithmName();
  mConfigFile += ".config";
  parser.loadFile(mConfigFile.c_str());
  std::string Version = parser.getValue("Version");
  if (parser.getErrorCode() == 0) {
    LOG(VERBOSE, ALGOBASE, "Hdr Algo Version: %s", Version.c_str());
  }
}

/**
 * @brief Destructor for HdrAlgorithm.
 */
HdrAlgorithm::~HdrAlgorithm() {
  StopAlgoThread();
  Close();
};

/**
 * @brief Open the Hdr algorithm, simulating resource checks.
 * @return Status of the operation.
 */
AlgoBase::AlgoStatus HdrAlgorithm::Open() {
  std::lock_guard<std::mutex> lock(mutex_);  // Protect the shared state
  // LOG(DEBUG, ALGOBASE, "OPEN In Hdr Algo");
  SetStatus(AlgoStatus::SUCCESS);
  return GetAlgoStatus();
}

/**
 * @brief Process the Hdr algorithm, simulating input validation and Hdr
 * computation.
 * @return Status of the operation.
 */
AlgoBase::AlgoStatus HdrAlgorithm::Process(std::shared_ptr<AlgoRequest> req) {
  std::lock_guard<std::mutex> lock(mutex_);

  int reqdone = 0x00;
  if (req &&
      (0 == req->mMetadata.GetMetadata(MetaId::ALGO_PROCESS_DONE, reqdone))) {
    reqdone |= ALGO_MASK(mAlgoId);
    req->mMetadata.SetMetadata(MetaId::ALGO_PROCESS_DONE, reqdone);
  }
  SetStatus(AlgoStatus::SUCCESS);
  return GetAlgoStatus();
}

/**
 * @brief Close the Hdr algorithm, simulating cleanup.
 * @return Status of the operation.
 */
AlgoBase::AlgoStatus HdrAlgorithm::Close() {
  std::lock_guard<std::mutex> lock(mutex_);  // Protect the shared state
  // LOG(DEBUG, ALGOBASE, "Close In Hdr Algo");
  SetStatus(AlgoStatus::SUCCESS);
  return GetAlgoStatus();
}

/**
 * @brief max time taken by algo to process a request
 *
 * @return int
 */
int HdrAlgorithm::GetTimeout() {
  return 1000;
}

// Public Exposed API for Hdr
/**
 * @brief Factory function to expose HdrAlgorithm via shared library.
 * @return A pointer to the HdrAlgorithm instance.
 */
extern "C" AlgoBase* GetAlgoMethod() {
  HdrAlgorithm* pInstance = new HdrAlgorithm();
  return pInstance;
}

/**
@brief Get the algorithm ID.
 *
 */
extern "C" AlgoId GetAlgoId() {
  return ALGO_HDR;
}
/**
@brief Get the algorithm name.
 *
 */
extern "C" const char* GetAlgorithmName() {
  return HDR_NAME;
}
