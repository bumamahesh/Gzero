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
  const int reqid                = req->mRequestId;

  if (CanProcessFormat(inputFormat0, inputFormat1)) {
    // Allocate output buffer for a YUV420 image (Y plane + UV planes)
    std::vector<unsigned char> outputData(width * height * 3 / 2, 0);
    LOG(ERROR, ALGOBASE, "Processing Bokeh request ::%d", reqid);

    // Stereo Disparity Map Calculation
    cv::Mat img0(height, width, CV_8UC1, &inputImage0->GetData()[0]);
    cv::Mat img1(height, width, CV_8UC1, &inputImage1->GetData()[0]);

    // Convert to grayscale if the images are not already in grayscale
    cv::Mat gray0, gray1;
    if (img0.channels() == 3) {
      cv::cvtColor(img0, gray0, cv::COLOR_BGR2GRAY);
      cv::cvtColor(img1, gray1, cv::COLOR_BGR2GRAY);
    } else {
      gray0 = img0;
      gray1 = img1;
    }

    // Compute disparity using StereoBM (adjust parameters as needed)
    cv::Ptr<cv::StereoBM> stereoBM = cv::StereoBM::create(64, 15);
    cv::Mat disparity;
    stereoBM->compute(gray0, gray1, disparity);

    // Normalize disparity for visualization
    cv::Mat dispNorm;
    cv::normalize(disparity, dispNorm, 0, 255, cv::NORM_MINMAX, CV_8UC1);
    // Normalize and convert to float for depth computation
    cv::normalize(disparity, disparity, 0, 255, cv::NORM_MINMAX);
    disparity.convertTo(disparity, CV_32F);

    // Compute depth map from disparity (avoid division by zero)
    const float baseline    = 0.1f;    // Example baseline in meters
    const float focalLength = 800.0f;  // Example focal length in pixels
    cv::Mat depthMap        = baseline * focalLength / (disparity + 1e-6);

    // Apply threshold to create a depth mask (adjust threshold value as needed)
    cv::Mat depthMask;
    cv::threshold(depthMap, depthMask, 0.1, 255, cv::THRESH_BINARY);
    depthMask.convertTo(depthMask,
                        CV_8U);  // Convert to 8-bit for display/storage

    // Fill outputData:
    // For a YUV420 image, the first width*height bytes are the Y channel.
    // We copy the depth mask into the Y plane.
    unsigned char* yPlane = outputData.data();
    std::memcpy(yPlane, depthMask.data, width * height);

    // The remaining bytes are for the U and V planes.
    // We'll fill them with 128 (neutral chroma) so that the image appears grayscale.
    unsigned char* uvPlane = outputData.data() + width * height;
    std::fill(uvPlane, uvPlane + (width * height / 2), 128);

    // Dump debug images using the request id for unique filenames
    //DumpDepthMask(depthMask, width, height, reqid);
    DumpDisparityMap(dispNorm, width, height, reqid);
    //DumpDepthMap(depthMap, width, height, reqid);
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
  (void)width;
  (void)height;
  std::string filename =
      "dump/depth_mask_" + std::to_string(processCount) + ".png";
  cv::imwrite(filename, depthMask);  // Save as an image file
}

// Function to dump disparity map for debugging with process count
void BokehAlgorithm::DumpDisparityMap(const cv::Mat& disparityMap, int width,
                                      int height, int processCount) {
  (void)width;
  (void)height;
  std::string filename =
      "dump/disparity_map_" + std::to_string(processCount) + ".png";
  cv::imwrite(filename, disparityMap);  // Save as an image file
}

// Optionally, you can also dump depth map with process count
void BokehAlgorithm::DumpDepthMap(const cv::Mat& depthMap, int width,
                                  int height, int processCount) {
  (void)width;
  (void)height;
  std::string filename =
      "dump/depth_map_" + std::to_string(processCount) + ".png";
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
