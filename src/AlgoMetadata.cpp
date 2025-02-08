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
 * @brief Construct a new Algo Metadata:: Algo Metadata object
 *
 */
AlgoMetadata::AlgoMetadata() {
  {
    std::lock_guard<std::mutex> lock(mMutex);
    intMetadata.clear();
    floatMetadata.clear();
    boolMetadata.clear();
  }
  // default metadata
  SetMetadata(MetaId::ALGO_PROCESS_DONE, 0x00);

  //for jpeg
  SetMetadata(MetaId::IMAGE_WIDTH, 1920);
  SetMetadata(MetaId::IMAGE_HEIGHT, 1080);
  SetMetadata(MetaId::IMAGE_ORIENTATION, 0);
  SetMetadata(MetaId::IMAGE_TIMESTAMP, 0);
  SetMetadata(MetaId::CAMERA_MAKE, 0);
  SetMetadata(MetaId::CAMERA_MODEL, 0);
  SetMetadata(MetaId::ISO_SPEED, 100);
  SetMetadata(MetaId::EXPOSURE_TIME, 1000);
  SetMetadata(MetaId::F_NUMBER, 2.8f);
  SetMetadata(MetaId::FOCAL_LENGTH, 35.0f);
  SetMetadata(MetaId::WHITE_BALANCE, 1);
  SetMetadata(MetaId::FLASH_STATE, false);
  SetMetadata(MetaId::GPS_LATITUDE, 0.0f);
  SetMetadata(MetaId::GPS_LONGITUDE, 0.0f);
  SetMetadata(MetaId::GPS_ALTITUDE, 0.0f);
  SetMetadata(MetaId::GPS_TIMESTAMP, 0);
  SetMetadata(MetaId::IMAGE_COMPRESSION, 90);
  SetMetadata(MetaId::COLOR_SPACE, 1);
  SetMetadata(MetaId::SENSOR_SENSITIVITY, 100);
  SetMetadata(MetaId::LENS_APERTURE, 2.8f);
  SetMetadata(MetaId::SHUTTER_SPEED, 1 / 100.0f);
  SetMetadata(MetaId::EXPOSURE_BIAS, 0.0f);
  SetMetadata(MetaId::METERING_MODE, 1);
  SetMetadata(MetaId::FOCUS_DISTANCE, 1.0f);
  SetMetadata(MetaId::IMAGE_BRIGHTNESS, 0);
  SetMetadata(MetaId::IMAGE_CONTRAST, 0);
  SetMetadata(MetaId::IMAGE_SATURATION, 0);
  SetMetadata(MetaId::IMAGE_SHARPNESS, 0);
  SetMetadata(MetaId::ALGO_HDR_ENABLED, false);
  SetMetadata(MetaId::ALGO_WATERMARK_ENABLED, false);
  SetMetadata(MetaId::ALGO_MANDELBROTSET_ENABLED, false);
  SetMetadata(MetaId::ALGO_FILTER_ENABLED, false);
  SetMetadata(MetaId::ALGO_JPEG_ENABLE, 1);
  SetMetadata(MetaId::ALGO_PROCESS_DONE, true);
  SetMetadata(MetaId::ALGO_REQUSET_NUMBER, 0);
}

AlgoMetadata::~AlgoMetadata() {
  {
    std::lock_guard<std::mutex> lock(mMutex);
    intMetadata.clear();
    floatMetadata.clear();
    boolMetadata.clear();
  }
}

/**
 * @brief Get the Metadata object
 *
 * @param id
 * @param value
 * @return int
 */
int AlgoMetadata::GetMetadata(MetaId id, int& value) {
  std::lock_guard<std::mutex> lock(mMutex);
  auto it = intMetadata.find(id);
  if (it != intMetadata.end()) {
    value = it->second;
    return 0;  // Success
  }
  return -1;  // Metadata not found
}

/**
 * @brief Get the Metadata object
 *
 * @param id
 * @param value
 * @return float
 */
int AlgoMetadata::GetMetadata(MetaId id, float& value) {
  std::lock_guard<std::mutex> lock(mMutex);
  auto it = floatMetadata.find(id);
  if (it != floatMetadata.end()) {
    value = it->second;
    return 0;  // Success
  }
  return -1;  // Metadata not found
}

/**
 * @brief Get the Metadata object
 *
 * @param id
 * @param value
 * @return bool
 */
int AlgoMetadata::GetMetadata(MetaId id, bool& value) {
  std::lock_guard<std::mutex> lock(mMutex);
  auto it = boolMetadata.find(id);
  if (it != boolMetadata.end()) {
    value = it->second;
    return 0;  // Success
  }
  return -1;  // Metadata not found
}

/**
 * @brief Set the Metadata object
 *
 * @param id
 * @param value
 * @return int
 */
int AlgoMetadata::SetMetadata(MetaId id, int value) {
  std::lock_guard<std::mutex> lock(mMutex);
  intMetadata[id] = value;
  return 0;  // Success
}

/**
 * @brief Set the Metadata object
 *
 * @param id
 * @param value
 * @return int
 */
int AlgoMetadata::SetMetadata(MetaId id, float value) {
  std::lock_guard<std::mutex> lock(mMutex);
  floatMetadata[id] = value;
  return 0;  // Success
}

/**
 * @brief Set the Metadata object
 *
 * @param id
 * @param value
 * @return bool
 */
int AlgoMetadata::SetMetadata(MetaId id, bool value) {
  std::lock_guard<std::mutex> lock(mMutex);
  boolMetadata[id] = value;
  return 0;  // Success
}