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
#include "AlgoMetadata.h"

/**
 * @brief Get the Metadata object
 *
 * @param id
 * @param value
 * @return int
 */
int AlgoMetadata::GetMetadata(Medatdata id, int &value) {
  std::lock_guard<std::mutex> lock(mMutex);
  auto it = intMetadata.find(id);
  if (it != intMetadata.end()) {
    value = it->second;
    return 0; // Success
  }
  return -1; // Metadata not found
}

/**
 * @brief Get the Metadata object
 *
 * @param id
 * @param value
 * @return float
 */
int AlgoMetadata::GetMetadata(Medatdata id, float &value) {
  std::lock_guard<std::mutex> lock(mMutex);
  auto it = floatMetadata.find(id);
  if (it != floatMetadata.end()) {
    value = it->second;
    return 0; // Success
  }
  return -1; // Metadata not found
}

/**
 * @brief Set the Metadata object
 *
 * @param id
 * @param value
 * @return int
 */
int AlgoMetadata::SetMetadata(Medatdata id, int value) {
  std::lock_guard<std::mutex> lock(mMutex);
  intMetadata[id] = value;
  return 0; // Success
}

/**
 * @brief Set the Metadata object
 *
 * @param id
 * @param value
 * @return int
 */
int AlgoMetadata::SetMetadata(Medatdata id, float value) {
  std::lock_guard<std::mutex> lock(mMutex);
  floatMetadata[id] = value;
  return 0; // Success
}