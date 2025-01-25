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
    : AlgoBase(MANDELBROTSET_NAME), zoomLevel(INITIAL_ZOOM), offsetX(CENTER_X),
      offsetY(CENTER_Y) {
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

  auto inputImage = req->GetImage(0); // Assume the first image as input
  if (!inputImage) {
    SetStatus(AlgoStatus::FAILURE);
    return GetAlgoStatus();
  }

  const int width = inputImage->width;
  const int height = inputImage->height;

  // Preallocate combined YUV420 buffer for output
  size_t ySize = width * height;
  size_t uvSize = ySize / 4; // U and V are subsampled
  std::vector<unsigned char> outputData(ySize + 2 * uvSize, 0);

  unsigned char *yData = outputData.data();
  unsigned char *uData = yData + ySize;
  unsigned char *vData = uData + uvSize;

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

      // Convert RGB to YUV420
      unsigned char y = (77 * r + 150 * g + 29 * b) >> 8;
      unsigned char u = ((-43 * r - 85 * g + 128 * b) >> 8) + 128;
      unsigned char v = ((128 * r - 107 * g - 21 * b) >> 8) + 128;

      // Write to Y plane
      yData[py * width + px] = y;

      // Write to U and V planes (subsampled 2x2 for YUV420)
      if ((py % 2 == 0) && (px % 2 == 0)) {
        int uvIndex = (py / 2) * (width / 2) + (px / 2);
        uData[uvIndex] = u;
        vData[uvIndex] = v;
      }
    }
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
int MandelbrotSet::GetTimeout() { return 1000; }

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
