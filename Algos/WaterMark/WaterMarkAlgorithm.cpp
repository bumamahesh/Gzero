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
#include "WaterMarkAlgorithm.h"
#include "ConfigParser.h"
#include "Log.h"
//#include <unistd.h>

/**
 * @brief Constructor for WaterMarkAlgorithm.
 * @param name Name of the WaterMark algorithm.
 */
WaterMarkAlgorithm::WaterMarkAlgorithm() : AlgoBase(WATERMARK_NAME) {
  mAlgoId = ALGO_WATERMARK;  // Unique ID for WaterMark algorithm
  SupportedFormatsMap.push_back({ImageFormat::RGB, ImageFormat::RGB});
  SupportedFormatsMap.push_back({ImageFormat::YUV420, ImageFormat::YUV420});
  ConfigParser parser;
  parser.loadFile("/home/uma/workspace/Gzero/Config/WaterMarkAlgorithm.config");
  std::string watermark_ = parser.getValue("Watermark");
  if (parser.getErrorCode() == 0) {
    watermarkText = watermark_;
  } else {
    watermarkText = watermark_.size() ? watermark_ : "Uma Mahesh B";
  }
  std::string Version = parser.getValue("Version");
  if (parser.getErrorCode() == 0) {
    LOG(VERBOSE, ALGOBASE, "WaterMark Algo Version: %s", Version.c_str());
  }

  std::string watermarkLogoPath_ = parser.getValue("watermarkLogoPath");
  if (parser.getErrorCode() == 0) {
    watermarkLogoPath = watermarkLogoPath_;
    LOG(VERBOSE, ALGOBASE, "watermarkLogoPath: %s", watermarkLogoPath.c_str());
  } else {
    watermarkLogoPath = "/home/uma/workspace/Gzero/Algos/res/Logo.png";
    LOG(VERBOSE, ALGOBASE, "watermarkLogoPath: %s errcode %d",
        watermarkLogoPath.c_str(), parser.getErrorCode());
  }
}

/**
 * @brief Destructor for WaterMarkAlgorithm.
 */
WaterMarkAlgorithm::~WaterMarkAlgorithm() {
  StopAlgoThread();
  Close();
};

/**
 * @brief Open the WaterMark algorithm, simulating resource checks.
 * @return Status of the operation.
 */
AlgoBase::AlgoStatus WaterMarkAlgorithm::Open() {
  std::lock_guard<std::mutex> lock(mutex_);  // Protect the shared state

  SetStatus(AlgoStatus::SUCCESS);
  return GetAlgoStatus();
}

cv::Point GetWatermarkPosition(WatermarkPosition position, int imageWidth,
                               int imageHeight, int logoWidth, int logoHeight) {
  int padding = 40;  // Padding from the edges
  switch (position) {
    case WatermarkPosition::BOTTOM_LEFT:
      return cv::Point(padding, imageHeight - logoHeight - padding);
    case WatermarkPosition::BOTTOM_RIGHT:
      return cv::Point(imageWidth - logoWidth - padding,
                       imageHeight - logoHeight - padding);
    case WatermarkPosition::TOP_LEFT:
      return cv::Point(padding, padding);
    case WatermarkPosition::TOP_RIGHT:
      return cv::Point(imageWidth - logoWidth - padding, padding);
    default:
      return cv::Point(padding, imageHeight - logoHeight -
                                    padding);  // Default to bottom-left
  }
}

AlgoBase::AlgoStatus WaterMarkAlgorithm::ProcessRGB(
    std::shared_ptr<AlgoRequest> req) {
  auto inputImage = req->GetImage(0);
  if (!inputImage) {
    LOG(ERROR, ALGOBASE, "Input image is null.");
    SetStatus(AlgoStatus::FAILURE);
    return GetAlgoStatus();
  }

  const ImageFormat inputFormat = inputImage->GetFormat();
  const int width               = inputImage->GetWidth();
  const int height              = inputImage->GetHeight();

  if (CanProcessFormat(inputFormat, ImageFormat::RGB)) {
    // Convert input RGB image to BGR for OpenCV processing
    cv::Mat rgbImage(height, width, CV_8UC3, inputImage->GetData().data());
    cv::Mat bgrImage;
    cv::cvtColor(rgbImage, bgrImage, cv::COLOR_RGB2BGR);

    // Load watermark logo with alpha channel
    cv::Mat logo = cv::imread(watermarkLogoPath.c_str(), cv::IMREAD_UNCHANGED);
    if (logo.empty()) {
      LOG(ERROR, ALGOBASE, "Failed to load the logo image.");
      SetStatus(AlgoStatus::FAILURE);
      return GetAlgoStatus();
    }

    // Resize logo
    const int logoWidth  = width / 5;
    const int logoHeight = logoWidth * logo.rows / logo.cols;
    cv::resize(logo, logo, cv::Size(logoWidth, logoHeight));

    // Determine logo position based on WatermarkPosition
    WatermarkPosition position =
        WatermarkPosition::BOTTOM_RIGHT;  // Example position
    cv::Point logoPos =
        GetWatermarkPosition(position, width, height, logoWidth, logoHeight);

    // Adjust position if out of bounds
    if (logoPos.x < 0 || logoPos.y < 0 || logoPos.x + logoWidth > width ||
        logoPos.y + logoHeight > height) {
      LOG(WARNING, ALGOBASE, "Adjusting logo position to fit within image.");
      logoPos.x = std::max(0, std::min(logoPos.x, width - logoWidth));
      logoPos.y = std::max(0, std::min(logoPos.y, height - logoHeight));
    }

    cv::Rect roi(logoPos.x, logoPos.y, logoWidth, logoHeight);
    if (roi.x < 0 || roi.y < 0 || roi.width + roi.x > bgrImage.cols ||
        roi.height + roi.y > bgrImage.rows) {
      LOG(ERROR, ALGOBASE, "Logo region out of image bounds.");
      SetStatus(AlgoStatus::FAILURE);
      return GetAlgoStatus();
    }

    cv::Mat region = bgrImage(roi);

    // Process logo with alpha blending
    if (logo.channels() == 4) {
      cv::Mat channels[4];
      cv::split(logo, channels);
      cv::Mat logoBGR;
      cv::merge(std::vector<cv::Mat>{channels[0], channels[1], channels[2]},
                logoBGR);
      cv::Mat alpha = channels[3];

      cv::Mat mask;
      alpha.convertTo(mask, CV_8UC1, 1.0 / 255.0);
      cv::threshold(mask, mask, 0.1, 1.0, cv::THRESH_BINARY);

      logoBGR.copyTo(region, mask);
    } else {
      cv::Mat logoBGR;
      if (logo.channels() == 3) {
        logoBGR = logo;
      } else {
        cv::cvtColor(logo, logoBGR, cv::COLOR_GRAY2BGR);
      }
      logoBGR.copyTo(region);
    }

    // Add watermark text below the logo
    int fontFace = cv::FONT_HERSHEY_SCRIPT_SIMPLEX;  // FONT_HERSHEY_SIMPLEX;
    double fontScale = 1.0;
    int thickness    = 2;
    cv::Scalar textColor(255, 255, 255);  // White in BGR

    // Calculate text position below the logo
    int textX = logoPos.x;
    int textY = logoPos.y + logoHeight + 30;  // 30 pixels below the logo

    cv::putText(bgrImage, watermarkText, cv::Point(textX, textY), fontFace,
                fontScale, textColor, thickness);

    // Convert BGR back to RGB for output
    cv::Mat outputRgbImage;
    cv::cvtColor(bgrImage, outputRgbImage, cv::COLOR_BGR2RGB);
    std::vector<uint8_t> outputData(outputRgbImage.total() *
                                    outputRgbImage.elemSize());
    memcpy(outputData.data(), outputRgbImage.data, outputData.size());

    req->ClearImages();
    if (req->AddImage(ImageFormat::RGB, width, height, std::move(outputData))) {
      LOG(ERROR, ALGOBASE, "Error Filling Output data");
      SetStatus(AlgoStatus::FAILURE);
    }

    // LOG(VERBOSE, ALGOBASE, "Watermark processing completed successfully.");
  } else {
    LOG(ERROR, ALGOBASE, "Unsupported image format for watermark processing.");
    SetStatus(AlgoStatus::FAILURE);
    return GetAlgoStatus();
  }
  return GetAlgoStatus();
}
AlgoBase::AlgoStatus WaterMarkAlgorithm::ProcessYUV(
    std::shared_ptr<AlgoRequest> req) {
  auto inputImage = req->GetImage(0);
  if (!inputImage) {
    LOG(ERROR, ALGOBASE, "Input image is null.");
    SetStatus(AlgoStatus::FAILURE);
    return GetAlgoStatus();
  }

  const ImageFormat inputFormat = inputImage->GetFormat();
  const int width               = inputImage->GetWidth();
  const int height              = inputImage->GetHeight();
  if (CanProcessFormat(inputFormat, ImageFormat::YUV420)) {
    cv::Mat bgrImage;

    if (inputFormat == ImageFormat::RGB) {
      // Convert input RGB image to BGR for OpenCV processing
      cv::Mat rgbImage(height, width, CV_8UC3, inputImage->GetData().data());
      cv::cvtColor(rgbImage, bgrImage, cv::COLOR_RGB2BGR);
    } else if (inputFormat == ImageFormat::YUV420) {
      // Convert YUV420 to BGR
      cv::Mat yuvImage(height + height / 2, width, CV_8UC1,
                       inputImage->GetData().data());
      cv::cvtColor(yuvImage, bgrImage, cv::COLOR_YUV2BGR_I420);
    } else {
      LOG(ERROR, ALGOBASE, "Unsupported input format.");
      SetStatus(AlgoStatus::FAILURE);
      return GetAlgoStatus();
    }

    // Load watermark logo with alpha channel
    cv::Mat logo = cv::imread(watermarkLogoPath.c_str(), cv::IMREAD_UNCHANGED);
    if (logo.empty()) {
      LOG(ERROR, ALGOBASE, "Failed to load the logo image.");
      SetStatus(AlgoStatus::FAILURE);
      return GetAlgoStatus();
    }

    // Resize logo
    const int logoWidth  = width / 5;
    const int logoHeight = logoWidth * logo.rows / logo.cols;
    cv::resize(logo, logo, cv::Size(logoWidth, logoHeight));

    // Determine logo position
    WatermarkPosition position = WatermarkPosition::BOTTOM_RIGHT;
    cv::Point logoPos =
        GetWatermarkPosition(position, width, height, logoWidth, logoHeight);

    // Adjust position if out of bounds
    logoPos.x = std::max(0, std::min(logoPos.x, width - logoWidth));
    logoPos.y = std::max(0, std::min(logoPos.y, height - logoHeight));

    cv::Rect roi(logoPos.x, logoPos.y, logoWidth, logoHeight);
    if (roi.x < 0 || roi.y < 0 || roi.width + roi.x > bgrImage.cols ||
        roi.height + roi.y > bgrImage.rows) {
      LOG(ERROR, ALGOBASE, "Logo region out of image bounds.");
      SetStatus(AlgoStatus::FAILURE);
      return GetAlgoStatus();
    }

    cv::Mat region = bgrImage(roi);

    // Process logo with alpha blending
    if (logo.channels() == 4) {
      cv::Mat channels[4];
      cv::split(logo, channels);
      cv::Mat logoBGR;
      cv::merge(std::vector<cv::Mat>{channels[0], channels[1], channels[2]},
                logoBGR);
      cv::Mat alpha = channels[3];

      cv::Mat mask;
      alpha.convertTo(mask, CV_8UC1, 1.0 / 255.0);
      cv::threshold(mask, mask, 0.1, 1.0, cv::THRESH_BINARY);

      logoBGR.copyTo(region, mask);
    } else {
      cv::Mat logoBGR;
      if (logo.channels() == 3) {
        logoBGR = logo;
      } else {
        cv::cvtColor(logo, logoBGR, cv::COLOR_GRAY2BGR);
      }
      logoBGR.copyTo(region);
    }

    // Add watermark text below the logo
    int fontFace     = cv::FONT_HERSHEY_SCRIPT_SIMPLEX;
    double fontScale = 1.0;
    int thickness    = 2;
    cv::Scalar textColor(255, 255, 255);  // White in BGR

    int textX = logoPos.x;
    int textY = logoPos.y + logoHeight + 30;

    cv::putText(bgrImage, watermarkText, cv::Point(textX, textY), fontFace,
                fontScale, textColor, thickness);

    // Convert output back to original format
    std::vector<uint8_t> outputData;

    if (inputFormat == ImageFormat::RGB) {
      cv::Mat outputRgbImage;
      cv::cvtColor(bgrImage, outputRgbImage, cv::COLOR_BGR2RGB);
      outputData.resize(outputRgbImage.total() * outputRgbImage.elemSize());
      memcpy(outputData.data(), outputRgbImage.data, outputData.size());
    } else if (inputFormat == ImageFormat::YUV420) {
      cv::Mat yuvImage;
      cv::cvtColor(bgrImage, yuvImage, cv::COLOR_BGR2YUV_I420);
      outputData.resize(yuvImage.total() * yuvImage.elemSize());
      memcpy(outputData.data(), yuvImage.data, outputData.size());
    }

    req->ClearImages();
    if (req->AddImage(inputFormat, width, height, std::move(outputData))) {
      LOG(ERROR, ALGOBASE, "Error Filling Output data");
      SetStatus(AlgoStatus::FAILURE);
    }
  }
  return GetAlgoStatus();
}
/**
 * @brief Process the WaterMark algorithm, simulating input validation and
 * WaterMark computation.
 * @return Status of the operation.
 */
AlgoBase::AlgoStatus WaterMarkAlgorithm::Process(
    std::shared_ptr<AlgoRequest> req) {
  // LOG(VERBOSE, ALGOBASE, "Entered WaterMarkAlgorithm::Process %ld",
  //     req->mProcessCnt);
  // KpiMonitor kpi("WaterMarkAlgorithm::Process");
  AlgoBase::AlgoStatus rc = AlgoStatus::FAILURE;
  auto inputImage         = req->GetImage(0);
  if (!inputImage) {
    LOG(ERROR, ALGOBASE, "Input image is null.");
    SetStatus(AlgoStatus::FAILURE);
    return GetAlgoStatus();
  }

  switch (inputImage->GetFormat()) {
    case ImageFormat::YUV420:
      rc = ProcessYUV(req);
      break;
    case ImageFormat::RGB:
      rc = ProcessRGB(req);
      break;
    default:
      LOG(ERROR, ALGOBASE, "Unsupported image format.");
      SetStatus(AlgoStatus::FAILURE);
      rc = GetAlgoStatus();
      break;
  }

  int reqdone = 0x00;
  if (req &&
      (0 == req->mMetadata.GetMetadata(MetaId::ALGO_PROCESS_DONE, reqdone))) {
    reqdone |= ALGO_MASK(mAlgoId);
    req->mMetadata.SetMetadata(MetaId::ALGO_PROCESS_DONE, reqdone);
  }
  return rc;
}
/**
 * @brief Close the WaterMark algorithm, simulating cleanup.
 * @return Status of the operation.
 */
AlgoBase::AlgoStatus WaterMarkAlgorithm::Close() {
  std::lock_guard<std::mutex> lock(mutex_);  // Protect the shared state

  SetStatus(AlgoStatus::SUCCESS);
  return GetAlgoStatus();
}

/**
 * @brief max time taken by algo to process a request
 *
 * @return int
 */
int WaterMarkAlgorithm::GetTimeout() {
  return 1000;
}

// Public Exposed API for WaterMark
/**
 * @brief Factory function to expose WaterMarkAlgorithm via shared library.
 * @return A pointer to the WaterMarkAlgorithm instance.
 */
extern "C" AlgoBase* GetAlgoMethod() {
  WaterMarkAlgorithm* pInstance = new WaterMarkAlgorithm();
  return pInstance;
}

/**
@brief Get the algorithm ID.
 *
 */
extern "C" AlgoId GetAlgoId() {
  return ALGO_WATERMARK;
}
/**
@brief Get the algorithm name.
 *
 */
extern "C" const char* GetAlgorithmName() {
  return WATERMARK_NAME;
}
