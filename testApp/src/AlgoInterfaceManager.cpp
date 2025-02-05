#include "../include/AlgoInterfaceManager.h"
#include <atomic>
#include <condition_variable>
#include <cstring>
#include <dlfcn.h>
#include <iostream>
#include <queue>
#include <unistd.h>
#include <vector>

int g_ResultCount          = 0;
int g_SubmittedCount       = 0;
size_t g_rgbSize           = 0;
size_t g_yuvSize           = 0;
unsigned char *g_rgbBuffer = nullptr;
std::mutex g_ResultQueueMutex;
std::condition_variable g_ResultQueueCondVar;
std::queue<unsigned char *> g_ResultQueue;

std::atomic<bool> g_quit(false);

bool g_HdrEnabled           = false;
bool g_WatermarkEnabled     = false;
bool g_MandlebrotSetEnabled = false;
bool g_FilterEnabled        = false;

/**
 * @brief Construct a new Algo Interfaceptr:: Algo Interfaceptr object
 *
 * @param path
 */
AlgoInterfaceptr::AlgoInterfaceptr(const std::string &path) {
  libraryHandle = dlopen(path.c_str(), RTLD_NOW | RTLD_NODELETE);
  if (!libraryHandle) {
    throw std::runtime_error(dlerror());
  }
  initFunc             = LOAD_SYM(InitAlgoInterface);
  deinitFunc           = LOAD_SYM(DeInitAlgoInterface);
  processFunc          = LOAD_SYM(AlgoInterfaceProcess);
  registerCallbackFunc = LOAD_SYM(RegisterCallback);

  if (!initFunc || !deinitFunc || !processFunc || !registerCallbackFunc) {
    std::cerr << "Failed to load one or more functions from the library."
              << std::endl;

    dlclose(libraryHandle);
    libraryHandle = nullptr;
    throw std::runtime_error(nullptr);
  }
}
/**
 * @brief Destroy the Algo Interfaceptr:: Algo Interfaceptr object
 *
 */
AlgoInterfaceptr::~AlgoInterfaceptr() {
  if (libraryHandle) {
    dlclose(libraryHandle);
  }
}
/**
 * @brief load Symbol from shared library
 *
 * @param symbolName
 * @return void*
 */
void *AlgoInterfaceptr::getSymbol(const char *symbolName) {
  return dlsym(libraryHandle, symbolName);
}

/**
 * @brief Construct a new Algo Interface Manager:: Algo Interface Manager object
 *
 */
AlgoInterfaceManager::AlgoInterfaceManager(const std::string inputFilePath, int width, int height) {

  phandle = std::make_shared<AlgoInterfaceptr>(ALGOLIB_PATH);
  if (!phandle) {
    std::cerr << "Error: Failed to load algorithm library." << std::endl;
    return;
  }
  if (InitAlgoInterface() != 0) {
    std::cerr << "Error: Failed to initialize algorithm interface." << std::endl;
    return;
  }

  mInputFile.open(inputFilePath, std::ios::binary);
  if (!mInputFile) {
    std::cerr << "Failed to open input file: " << inputFilePath << std::endl;
    return;
  }
  mWidth    = width;
  mHeight   = height;
  g_rgbSize = mWidth * mHeight * 3;
  g_yuvSize = mWidth * mHeight * 3 / 2;
  yuvBuffer.resize(g_yuvSize);
}

/**
 * @brief Destroy the Algo Interface Manager:: Algo Interface Manager object
 *
 */
AlgoInterfaceManager::~AlgoInterfaceManager() {
  Cleanup();
}

// Callback Function
/**
 * @brief
 *
 * @param request
 * @return int
 */
int ProcessCallback(std::shared_ptr<AlgoRequest> request) {
  if (request) {
    std::shared_ptr<ImageData> image = request->GetImage(0);
    if (image) {
      if (image->GetDataSize() == 0) {
        return -1;
      }
      if (image->GetFormat() != ImageFormat::RGB) {
        return -2;
      }
      if (g_rgbBuffer == nullptr) {
        return -3;
      }
      // Check if the image data size is correct
      if (image->GetData().size() < g_rgbSize) {
        std::cerr << "Error: Image data size is smaller than expected."
                  << std::endl;
        return -4;
      }
      {
        auto rgbbuffer = new unsigned char[g_rgbSize];
        memcpy(rgbbuffer, image->GetData().data(), g_rgbSize);
        {
          std::unique_lock<std::mutex> lock(g_ResultQueueMutex);
          g_ResultQueue.push(rgbbuffer);
          g_ResultQueueMutex.unlock();
        }
        g_ResultQueueCondVar.notify_one();
      }
      ++g_ResultCount;
    }
  }
  return 0;
}

template <typename T>
T clamp(T value, T min, T max) {
  if (value < min)
    return min;
  if (value > max)
    return max;
  return value;
}
/**
 * @brief
 *
 * @param yuvBuffer
 * @param rgbBuffer
 * @param width
 * @param height
 */
void YUVtoRGB(const unsigned char *yuvBuffer, unsigned char *rgbBuffer, int width, int height) {
  for (int i = 0; i < height; ++i) {
    for (int j = 0; j < width; ++j) {
      int yIndex  = i * width + j;
      int uvIndex = (i / 2) * (width / 2) + (j / 2);

      int Y = yuvBuffer[yIndex];
      int U = yuvBuffer[width * height + uvIndex] - 128;
      int V = yuvBuffer[width * height + (width * height / 4) + uvIndex] - 128;

      int R = Y + 1.402 * V;
      int G = Y - 0.344136 * U - 0.714136 * V;
      int B = Y + 1.772 * U;

      rgbBuffer[(i * width + j) * 3 + 0] = clamp(R, 0, 255);
      rgbBuffer[(i * width + j) * 3 + 1] = clamp(G, 0, 255);
      rgbBuffer[(i * width + j) * 3 + 2] = clamp(B, 0, 255);
    }
  }
}

/**
 * @brief Set the Metadata object
 *
 * @param request
 */
void SetMetadata(std::shared_ptr<AlgoRequest> request) {
  if (request) {
    request->mMetadata.SetMetadata(ALGO_HDR_ENABLED, g_HdrEnabled);
    request->mMetadata.SetMetadata(ALGO_WATERMARK_ENABLED, g_WatermarkEnabled);
    request->mMetadata.SetMetadata(ALGO_MANDELBROTSET_ENABLED, g_MandlebrotSetEnabled);
    request->mMetadata.SetMetadata(ALGO_FILTER_ENABLED, g_FilterEnabled);
    std::cerr << "g_HdrEnabled = " << g_HdrEnabled << " g_WatermarkEnabled = " << g_WatermarkEnabled << " g_MandlebrotSetEnabled = " << g_MandlebrotSetEnabled << " g_FilterEnabled = " << g_FilterEnabled << std::endl;
  }
}

int AlgoInterfaceManager::SubmitRequest() {
  int rc = 0;
  if ((g_SubmittedCount - g_ResultCount < 20) || (g_SubmittedCount < 30)) {

    // read a yuv buffer from file and convert it to RGB
    std::vector<unsigned char> rgbBuffer(mWidth * mHeight * 3);
    if (!mInputFile.read(reinterpret_cast<char *>(yuvBuffer.data()), yuvBuffer.size())) {
      mInputFile.clear();                                                            // Clear EOF flag
      mInputFile.seekg(0, std::ios::beg);                                            // Seek back to the start of the file
      mInputFile.read(reinterpret_cast<char *>(yuvBuffer.data()), yuvBuffer.size()); // read after reset
    }
    YUVtoRGB(yuvBuffer.data(), rgbBuffer.data(), mWidth, mHeight);

    // prepare and submit request
    auto request        = std::make_shared<AlgoRequest>();
    request->mRequestId = mRequestId++;
    rc                  = request->AddImage(ImageFormat::RGB, mWidth, mHeight, std::move(rgbBuffer));
    if (rc != 0) {
      std::cerr << "Failed to add image to request rc=" << rc << std::endl;
      return -1;
    }
    request->mMetadata.SetMetadata(IMAGE_WIDTH, mWidth);
    request->mMetadata.SetMetadata(IMAGE_HEIGHT, mHeight);
    SetMetadata(request);
    std::vector<AlgoId> algoSuggested = m_algoDecisionManager.ParseMetadata(request);

    /*std::cerr << "Algo Suggested :";
    for (auto id : algoSuggested) {
      std::cerr << " " << algoName[ALGO_OFFSET(id)] << std::endl;
    }*/

    rc = phandle->processFunc(&phandle->libraryHandle, request, algoSuggested);
    if (rc != 0) {
      std::cerr << "Failed to process algorithm request rc = " << rc << std::endl;
      return -2;
    }
    ++g_SubmittedCount;
  } else {
    usleep(50);
  }
  return 0;
}

/**
 * @brief
 *
 * @return int
 */
int AlgoInterfaceManager::InitAlgoInterface() {
  if (phandle->initFunc(&phandle->libraryHandle) != 0) {
    std::cerr << "Failed to initialize algorithm interface." << std::endl;
    return -1;
  }

  if (phandle->registerCallbackFunc(&phandle->libraryHandle, ProcessCallback) != 0) {
    std::cerr << "Failed to register callback." << std::endl;
    return -1;
  }
  return 0;
}

/**
 * @brief
 *
 */
void AlgoInterfaceManager::Cleanup() {
  if (phandle->deinitFunc) {
    phandle->deinitFunc(&phandle->libraryHandle);
  }
  if (phandle->libraryHandle) {
    dlclose(phandle->libraryHandle);
  }
  if (mInputFile.is_open()) {
    mInputFile.close();
  }
  {
    std::unique_lock<std::mutex> lock(g_ResultQueueMutex);
    while (!g_ResultQueue.empty()) {
      g_ResultQueue.pop();
    }
  }

  phandle->libraryHandle        = nullptr;
  phandle->initFunc             = nullptr;
  phandle->deinitFunc           = nullptr;
  phandle->processFunc          = nullptr;
  phandle->registerCallbackFunc = nullptr;
}
