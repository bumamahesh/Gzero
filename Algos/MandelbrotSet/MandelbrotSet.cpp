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
#include "MandelbrotSet.h"
#include "Log.h"
#include <cmath>
#include <complex>

// Function to compute the Mandelbrot set for a single pixel
int ComputeMandelbrot(double x, double y) {
  std::complex<double> c(x, y);
  std::complex<double> z = 0;
  int iter = 0;

  while (std::norm(z) <= 4.0 && iter < MAX_ITER) {
    z = z * z + c;
    ++iter;
  }
  return iter;
}

// Function to map pixel coordinates to complex plane
std::pair<double, double> MapToComplexPlane(int px, int py, int width,
                                            int height, double zoom,
                                            double offsetX, double offsetY) {
  double scaledX = (static_cast<double>(px) / width - 0.5) / zoom + offsetX;
  double scaledY = (static_cast<double>(py) / height - 0.5) / zoom + offsetY;
  return {scaledX, scaledY};
}

/**
 * @brief Constructor for MandelbrotSet.
 */
MandelbrotSet::MandelbrotSet()
    : AlgoBase(MANDELBROTSET_NAME), zoomLevel(INITIAL_ZOOM) {
  modelIdx = (int)MandelbrotSetCentre::Seahorse_Valley;
  offsetX = CentreCordinates[modelIdx][0];
  offsetY = CentreCordinates[modelIdx][1];
  mAlgoId = ALGO_MANDELBROTSET; // Unique ID for HDR algorithm
}

/**
 * @brief Destructor for MandelbrotSet.
 */
MandelbrotSet::~MandelbrotSet() {
  StopAlgoThread();
  Close();
}

/**
 * @brief Open the HDR algorithm, simulating resource checks.
 * @return Status of the operation.
 */
AlgoBase::AlgoStatus MandelbrotSet::Open() {
  std::lock_guard<std::mutex> lock(mutex_);
  SetStatus(AlgoStatus::SUCCESS);
  return GetAlgoStatus();
}

/**
 * @brief Process the HDR algorithm, simulating input validation and Mandelbrot
 * computation.
 * @param req A shared pointer to the AlgoRequest object.
 * @return Status of the operation.
 */
AlgoBase::AlgoStatus MandelbrotSet::Process(std::shared_ptr<AlgoRequest> req) {
  std::lock_guard<std::mutex> lock(mutex_);
  KpiMonitor kpi("MandelbrotSet::Process");

  if (!req || req->GetImageCount() == 0) {
    SetStatus(AlgoStatus::FAILURE);
    return GetAlgoStatus();
  }
  if (req->mRequestId % 30 == 0) {
    /* Reset offset and zoom level smoothly */
    modelIdx++;
    modelIdx = modelIdx % 3;
    offsetX += (CentreCordinates[modelIdx][0] - offsetX) * 0.1;
    offsetY += (CentreCordinates[modelIdx][1] - offsetY) * 0.1;
    zoomLevel = INITIAL_ZOOM;
  }

  auto inputImage = req->GetImage(0); // Assume the first image as input
  if (!inputImage) {
    SetStatus(AlgoStatus::FAILURE);
    return GetAlgoStatus();
  }

  const int width = inputImage->width;
  const int height = inputImage->height;

  // Preallocate combined YUV420 buffer for output

  std::vector<unsigned char> outputData(width * height * 3, 0); // RGB format

  // Parallelized computation using OpenMP
  //#pragma omp parallel for schedule(dynamic)
  for (int py = 0; py < height; ++py) {
    for (int px = 0; px < width; ++px) {
      // Map pixel to the complex plane
      auto [cx, cy] =
          MapToComplexPlane(px, py, width, height, zoomLevel, offsetX, offsetY);

      // Compute Mandelbrot iterations
      int iterations = ComputeMandelbrot(cx, cy);

      // Normalize iterations for color gradient
      float normalized = static_cast<float>(iterations) / MAX_ITER;

      // RGB color calculation (optimized math)
      unsigned char r = static_cast<unsigned char>(
          9 * (1 - normalized) * normalized * normalized * normalized * 255);
      unsigned char g =
          static_cast<unsigned char>(15 * (1 - normalized) * (1 - normalized) *
                                     normalized * normalized * 255);
      unsigned char b =
          static_cast<unsigned char>(8.5 * (1 - normalized) * (1 - normalized) *
                                     (1 - normalized) * normalized * 255);

      int pixelIndex = (py * width + px) * 3;

      outputData[pixelIndex + 0] = r;
      outputData[pixelIndex + 1] = g;
      outputData[pixelIndex + 2] = b;
    }
    // LOG(VERBOSE, ALGOBASE, "Processed Frames::%d", req->mRequestId);
  }

  // Replace input image with output image
  req->ClearImages();
  req->AddImage(ImageFormat::YUV420, width, height, outputData);

  // Update zoom level for the next frame
  zoomLevel *= ZOOM_FACTOR;

  SetStatus(AlgoStatus::SUCCESS);
  return GetAlgoStatus();
}

/**
 * @brief Close the HDR algorithm, simulating cleanup.
 * @return Status of the operation.
 */
AlgoBase::AlgoStatus MandelbrotSet::Close() {
  std::lock_guard<std::mutex> lock(mutex_);
  SetStatus(AlgoStatus::SUCCESS);
  return GetAlgoStatus();
}

/**
 * @brief max time taken by algo to process a request
 *
 * @return int
 */
int MandelbrotSet::GetTimeout() { return 5000; }

/**
 * @brief Factory function to expose MandelbrotSet via shared library.
 * @return A pointer to the MandelbrotSet instance.
 */
extern "C" AlgoBase *GetAlgoMethod() {
  MandelbrotSet *pInstance = new MandelbrotSet();
  return pInstance;
}

/**
 * @brief Get the algorithm ID.
 */
extern "C" AlgoId GetAlgoId() { return ALGO_MANDELBROTSET; }

/**
 * @brief Get the algorithm name.
 */
extern "C" const char *GetAlgorithmName() { return MANDELBROTSET_NAME; }
