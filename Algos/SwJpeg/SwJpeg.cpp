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
#include "SwJpeg.h"
#include <jpeglib.h>
#include <algorithm>
#include <map>
#include <string>
#include "ConfigParser.h"
#include "Log.h"

/**
 * @brief Constructor for SwJpeg.
 */
SwJpeg::SwJpeg() : AlgoBase(SWJPEG_NAME) {
  mAlgoId = ALGO_SWJPEG;  // Unique ID for SWJPEG algorithm
  SupportedFormatsMap.push_back({ImageFormat::YUV420, ImageFormat::JPEG});
  SupportedFormatsMap.push_back({ImageFormat::RGB, ImageFormat::JPEG});
  ConfigParser parser;
  parser.loadFile("/home/uma/workspace/Gzero/Config/SwJpegAlgorithm.config");
  std::string Version = parser.getValue("Version");
  if (parser.getErrorCode() == 0) {
    LOG(VERBOSE, ALGOBASE, "SwJpeg Algo Version: %s", Version.c_str());
  }
}

/**
 * @brief Destructor for SwJpeg.
 */
SwJpeg::~SwJpeg() {
  StopAlgoThread();
  Close();
}

/**
 * @brief Open the SWJPEG algorithm, simulating resource checks.
 * @return Status of the operation.
 */
AlgoBase::AlgoStatus SwJpeg::Open() {
  std::lock_guard<std::mutex> lock(mutex_);
  SetStatus(AlgoStatus::SUCCESS);
  return GetAlgoStatus();
}

/**
@brief  utlity to conver yuv to RGB unoptimised version fix me
 * 
 * @param yuvData 
 * @param rgbData 
 * @param width 
 * @param height 
 * @param format 
 */
void ConvertYUVToRGB(const unsigned char* yuvData, unsigned char* rgbData,
                     int width, int height, ImageFormat format) {
  int frameSize               = width * height;
  const unsigned char* yPlane = yuvData;
  const unsigned char* uPlane = yuvData + frameSize;
  const unsigned char* vPlane = yuvData + frameSize + (frameSize / 4);

  for (int j = 0; j < height; j++) {
    for (int i = 0; i < width; i++) {
      int y = yPlane[j * width + i];
      int u = uPlane[(j / 2) * (width / 2) + (i / 2)];
      int v = vPlane[(j / 2) * (width / 2) + (i / 2)];

      int c = y - 16;
      int d = u - 128;
      int e = v - 128;

      int r = (298 * c + 409 * e + 128) >> 8;
      int g = (298 * c - 100 * d - 208 * e + 128) >> 8;
      int b = (298 * c + 516 * d + 128) >> 8;

      rgbData[(j * width + i) * 3 + 0] = std::clamp(r, 0, 255);
      rgbData[(j * width + i) * 3 + 1] = std::clamp(g, 0, 255);
      rgbData[(j * width + i) * 3 + 2] = std::clamp(b, 0, 255);
    }
  }
}

/**
@brief  Compose metadata for Jpeg
 * 
 * @param req 
 * @param cinfo 
 * @return int  status
 */
static int ComposeMetadata(std::shared_ptr<AlgoRequest> req,
                           struct jpeg_compress_struct& cinfo) {
  std::map<std::string, std::string> metadata;
  // Example of extracting some metadata
  int imageWidth, imageHeight;
  if (req->mMetadata.GetMetadata(MetaId::IMAGE_WIDTH, imageWidth) == 0) {
    metadata["Image Width"] = std::to_string(imageWidth);
  }

  if (req->mMetadata.GetMetadata(MetaId::IMAGE_HEIGHT, imageHeight) == 0) {
    metadata["Image Height"] = std::to_string(imageHeight);
  }

  // Extract other metadata similarly...
  float exposureTime;
  if (req->mMetadata.GetMetadata(MetaId::EXPOSURE_TIME, exposureTime) == 0) {
    metadata["Exposure Time"] = std::to_string(exposureTime);
  }

  bool flashState;
  if (req->mMetadata.GetMetadata(MetaId::FLASH_STATE, flashState) == 0) {
    metadata["Flash State"] = flashState ? "Used" : "Not Used";
  }

  // Set metadata in JPEG header (e.g., EXIF data)
  for (const auto& entry : metadata) {
    // Add custom metadata to EXIF or JFIF header
    jpeg_write_marker(&cinfo, JPEG_COM, (unsigned char*)entry.first.c_str(),
                      entry.first.length());
    jpeg_write_marker(&cinfo, JPEG_COM, (unsigned char*)entry.second.c_str(),
                      entry.second.length());
  }
  return 0;
}
/**
 * @brief Process the SWJPEG algorithm, simulating input validation and
 * ShJpeg computation.
 * @param req A shared pointer to the AlgoRequest object.
 * @return Status of the operation.
 */
AlgoBase::AlgoStatus SwJpeg::Process(std::shared_ptr<AlgoRequest> req) {
  std::lock_guard<std::mutex> lock(mutex_);
  if (!req || req->GetImageCount() == 0) {
    SetStatus(AlgoStatus::FAILURE);
    return GetAlgoStatus();
  }
  auto inputImage = req->GetImage(0);
  if (!inputImage) {
    SetStatus(AlgoStatus::FAILURE);
    return GetAlgoStatus();
  }

  const ImageFormat inputFormat                  = inputImage->GetFormat();
  const int width                                = inputImage->GetWidth();
  const int height                               = inputImage->GetHeight();
  const std::vector<unsigned char>& inputDataVec = inputImage->GetData();
  const unsigned char* inputData                 = inputDataVec.data();
  std::vector<unsigned char> rgbData;

  if (inputFormat == ImageFormat::YUV420 ||
      inputFormat == ImageFormat::YUV422) {
    rgbData.resize(width * height * 3);
    ConvertYUVToRGB(inputData, rgbData.data(), width, height, inputFormat);
    inputData = rgbData.data();
  }
  if (CanProcessFormat(inputFormat, ImageFormat::JPEG)) {
    std::vector<unsigned char> jpegData;
    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;

    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_compress(&cinfo);

    unsigned char* outBuffer = nullptr;
    unsigned long outSize    = 0;
    jpeg_mem_dest(&cinfo, &outBuffer, &outSize);

    cinfo.image_width      = width;
    cinfo.image_height     = height;
    cinfo.input_components = 3;  // RGB
    cinfo.in_color_space   = JCS_RGB;

    jpeg_set_defaults(&cinfo);
    jpeg_set_quality(&cinfo, 75, TRUE);  // Set quality to 75%

    jpeg_start_compress(&cinfo, TRUE);

    // Add metadata first, before scanlines
    ComposeMetadata(req, cinfo);

    // Write image scanlines
    JSAMPROW row_pointer[1];
    int row_stride = width * 3;
    while (cinfo.next_scanline < cinfo.image_height) {
      row_pointer[0] = const_cast<unsigned char*>(
          &inputData[cinfo.next_scanline * row_stride]);
      jpeg_write_scanlines(&cinfo, row_pointer, 1);
    }

    jpeg_finish_compress(&cinfo);   // Finish compression
    jpeg_destroy_compress(&cinfo);  // Cleanup

    jpegData.assign(outBuffer, outBuffer + outSize);
    free(outBuffer);  // Free memory allocated for the buffer

    req->ClearImages();
    if (req->AddImage(ImageFormat::JPEG, width, height, std::move(jpegData))) {
      LOG(ERROR, ALGOBASE, "Error Filling Output data");
      SetStatus(AlgoStatus::FAILURE);
    }
  }

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
 * @brief Close the SWJPEG algorithm, simulating cleanup.
 * @return Status of the operation.
 */
AlgoBase::AlgoStatus SwJpeg::Close() {
  std::lock_guard<std::mutex> lock(mutex_);
  SetStatus(AlgoStatus::SUCCESS);
  return GetAlgoStatus();
}

/**
 * @brief max time taken by algo to process a request
 *
 * @return int
 */
int SwJpeg::GetTimeout() {
  return 10000;
}

/**
 * @brief Factory function to expose SwJpeg via shared library.
 * @return A pointer to the SwJpeg instance.
 */
extern "C" AlgoBase* GetAlgoMethod() {
  SwJpeg* pInstance = new SwJpeg();
  return pInstance;
}

/**
 * @brief Get the algorithm ID.
 */
extern "C" AlgoId GetAlgoId() {
  return ALGO_SWJPEG;
}

/**
 * @brief Get the algorithm name.
 */
extern "C" const char* GetAlgorithmName() {
  return SWJPEG_NAME;
}
