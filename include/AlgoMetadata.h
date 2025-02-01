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
#ifndef ALGO_METADATA_H
#define ALGO_METADATA_H
#include <mutex>
#pragma once
#include <memory>
#include <mutex>
#include <unordered_map>

class AlgoRequest;
typedef enum AlgoMedatdata {
  IMAGE_WIDTH,        // Image width in pixels
  IMAGE_HEIGHT,       // Image height in pixels
  IMAGE_FORMAT,       // Image format (e.g., JPEG, PNG)
  IMAGE_ORIENTATION,  // Orientation of the image (e.g., 0, 90, 180, 270)
  IMAGE_TIMESTAMP,    // Timestamp when the image was captured
  CAMERA_MAKE,        // Camera manufacturer
  CAMERA_MODEL,       // Camera model
  ISO_SPEED,          // ISO speed setting
  EXPOSURE_TIME,      // Exposure time in microseconds
  F_NUMBER,           // Aperture value (f-number)
  FOCAL_LENGTH,       // Focal length in mm
  WHITE_BALANCE,      // White balance mode
  FLASH_STATE,        // Whether the flash was used
  GPS_LATITUDE,       // GPS latitude if available
  GPS_LONGITUDE,      // GPS longitude if available
  GPS_ALTITUDE,       // GPS altitude if available
  GPS_TIMESTAMP,      // Timestamp of GPS data
  IMAGE_COMPRESSION,  // Compression level of the image
  COLOR_SPACE,        // Color space (e.g., sRGB, Adobe RGB)
  SENSOR_SENSITIVITY, // Sensitivity of the sensor
  LENS_APERTURE,      // Lens aperture value
  SHUTTER_SPEED,      // Shutter speed in seconds
  EXPOSURE_BIAS,      // Exposure bias value
  METERING_MODE,      // Metering mode used
  FOCUS_DISTANCE,     // Focus distance in meters
  IMAGE_BRIGHTNESS,   // Brightness of the image
  IMAGE_CONTRAST,     // Contrast level
  IMAGE_SATURATION,   // Saturation level
  IMAGE_SHARPNESS,    // Sharpness level
  ALGO_PROCESS_DONE   // algo has done processing
} Medatdata;

class AlgoMetadata {
public:
  AlgoMetadata() = default;
  ~AlgoMetadata() = default;
  int GetMetadata(Medatdata id, int &value);
  int GetMetadata(Medatdata id, float &value);
  int SetMetadata(Medatdata id, int value);
  int SetMetadata(Medatdata id, float value);

private:
  std::unordered_map<Medatdata, int> intMetadata;
  std::unordered_map<Medatdata, float> floatMetadata;
  std::mutex mMutex;
};

#endif // ALGO_METADATA_H
