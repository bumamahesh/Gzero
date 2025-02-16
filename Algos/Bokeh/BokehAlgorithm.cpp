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
  SupportedFormatsMap.push_back({ImageFormat::YUV420, ImageFormat::YUV420});
  ConfigParser parser;
  mConfigFile = CONFIGPATH;
  mConfigFile += AlgoBase::GetAlgorithmName();
  mConfigFile += ".config";
  parser.loadFile(mConfigFile.c_str());
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
  std::lock_guard<std::mutex> lock(mutex_);

  if (!req || req->GetImageCount() < 2) {
    SetStatus(AlgoStatus::FAILURE);
    return GetAlgoStatus();
  }

  auto inputImage0 = req->GetImage(0);
  auto inputImage1 = req->GetImage(1);
  if (!inputImage0 || !inputImage1) {
    SetStatus(AlgoStatus::FAILURE);
    return GetAlgoStatus();
  }

  const ImageFormat inputFormat0 = inputImage0->GetFormat();
  const ImageFormat inputFormat1 = inputImage1->GetFormat();
  const int width                = inputImage0->GetWidth();
  const int height               = inputImage0->GetHeight();

  if (CanProcessFormat(inputFormat0, inputFormat1)) {
    std::vector<unsigned char> outputData(width * height * 3 / 2,
                                          0);  // YUV output

    // Stereo Disparity Map Calculation
    cv::Mat img0 = cv::Mat(height, width, CV_8UC1, inputImage0->GetData()[0]);
    cv::Mat img1 = cv::Mat(height, width, CV_8UC1, inputImage1->GetData()[0]);

    // Convert to grayscale if they are not
    cv::Mat gray0, gray1;
    if (img0.channels() == 3) {
      cv::cvtColor(img0, gray0, cv::COLOR_BGR2GRAY);
      cv::cvtColor(img1, gray1, cv::COLOR_BGR2GRAY);
    } else {
      gray0 = img0;
      gray1 = img1;
    }

    // Compute disparity using StereoBM or StereoSGBM
    cv::Ptr<cv::StereoBM> stereoBM =
        cv::StereoBM::create(16, 9);  // Adjust parameters as needed
    cv::Mat disparity;
    stereoBM->compute(gray0, gray1, disparity);

    // Normalize disparity for visualization
    cv::Mat dispNorm;
    cv::normalize(disparity, dispNorm, 0, 255, cv::NORM_MINMAX, CV_8UC1);

    // Compute depth mask from disparity map (Assume baseline and focal length are known)
    const float baseline    = 0.1f;  // Example baseline (in meters)
    const float focalLength = 0.8f;  // Example focal length (in pixels)
    cv::Mat depthMap        = baseline * focalLength / disparity;

    // Apply depth threshold to create a mask (e.g., mask near objects)
    cv::Mat depthMask;
    cv::threshold(depthMap, depthMask, 0.5, 255, cv::THRESH_BINARY);

    // Extract process count (mProcessCnt) from the request
    int processCount =
        req->mProcessCnt;  // Assuming mProcessCnt is available in the request

    // Dump depth mask and disparity map for debugging with process count in filenames
    DumpDepthMask(depthMask, width, height, processCount);
    DumpDisparityMap(dispNorm, width, height, processCount);

    // Optionally, dump depth map
    DumpDepthMap(depthMap, width, height, processCount);

    // Replace input image with processed output
    req->ClearImages();
    if (req->AddImage(ImageFormat::YUV420, width, height,
                      std::move(outputData))) {
      LOG(ERROR, ALGOBASE, "Error Filling Output Data");
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

// Function to dump depth mask for debugging with process count
void BokehAlgorithm::DumpDepthMask(const cv::Mat& depthMask, int width,
                                   int height, int processCount) {
  std::string filename = "depth_mask_" + std::to_string(processCount) + ".png";
  cv::imwrite(filename, depthMask);  // Save as an image file
}

// Function to dump disparity map for debugging with process count
void BokehAlgorithm::DumpDisparityMap(const cv::Mat& disparityMap, int width,
                                      int height, int processCount) {
  std::string filename =
      "disparity_map_" + std::to_string(processCount) + ".png";
  cv::imwrite(filename, disparityMap);  // Save as an image file
}

// Optionally, you can also dump depth map with process count
void BokehAlgorithm::DumpDepthMap(const cv::Mat& depthMap, int width,
                                  int height, int processCount) {
  std::string filename = "depth_map_" + std::to_string(processCount) + ".png";
  cv::imwrite(filename, depthMap);  // Save as an image file
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
  return 90000;
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
