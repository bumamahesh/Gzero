#include "../include/AlgoInterfaceManager.h"
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

bool g_quit = false;

/**
 * @brief Construct a new Algo Interface Manager:: Algo Interface Manager object
 *
 */
AlgoInterfaceManager::AlgoInterfaceManager(const std::string inputFilePath, int width, int height) {
  if (!LoadLibraryFunctions()) {
    std::cerr << "Error: Failed to load library functions." << std::endl;
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

/**
 * @brief
 *
 * @return true
 * @return false
 */
bool AlgoInterfaceManager::LoadLibraryFunctions() {
  handle.libraryHandle =
      dlopen("/home/uma/workspace/Gzero/cmake/lib/libAlgoLib.so", RTLD_LAZY);
  if (!handle.libraryHandle) {
    std::cerr << "Failed to load library." << std::endl;
    return false;
  }

  handle.initFunc = reinterpret_cast<InitAlgoInterfaceFunc>(
      dlsym(handle.libraryHandle, "InitAlgoInterface"));
  handle.deinitFunc = reinterpret_cast<DeInitAlgoInterfaceFunc>(
      dlsym(handle.libraryHandle, "DeInitAlgoInterface"));
  handle.processFunc = reinterpret_cast<AlgoInterfaceProcessFunc>(
      dlsym(handle.libraryHandle, "AlgoInterfaceProcess"));
  handle.registerCallbackFunc = reinterpret_cast<RegisterCallbackFunc>(
      dlsym(handle.libraryHandle, "RegisterCallback"));

  if (!handle.initFunc || !handle.deinitFunc || !handle.processFunc || !handle.registerCallbackFunc) {
    std::cerr << "Failed to load one or more functions from the library."
              << std::endl;
    dlclose(handle.libraryHandle);
    handle.libraryHandle = nullptr;
    return false;
  }

  return true;
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
int AlgoInterfaceManager::SubmitRequest() {
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
    request->AddImage(ImageFormat::RGB, mWidth, mHeight, std::move(rgbBuffer));

    int status = handle.processFunc(&handle.libraryHandle, request, m_algoDecisionManager.ParseMetadata(request));
    if (status != 0) {
      std::cerr << "Failed to process algorithm request." << std::endl;
      return -1;
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
  if (handle.initFunc(&handle.libraryHandle) != 0) {
    std::cerr << "Failed to initialize algorithm interface." << std::endl;
    return -1;
  }

  if (handle.registerCallbackFunc(&handle.libraryHandle, ProcessCallback) != 0) {
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
  if (handle.deinitFunc) {
    handle.deinitFunc(&handle.libraryHandle);
  }
  if (handle.libraryHandle) {
    dlclose(handle.libraryHandle);
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

  handle.libraryHandle        = nullptr;
  handle.initFunc             = nullptr;
  handle.deinitFunc           = nullptr;
  handle.processFunc          = nullptr;
  handle.registerCallbackFunc = nullptr;
}
