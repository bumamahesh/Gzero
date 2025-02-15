#include "../include/AlgoInterfaceManager.h"
#include <unistd.h>
#include <atomic>
#include <condition_variable>
#include <cstring>
#include <iostream>
#include <queue>

int g_ResultCount    = 0;
int g_SubmittedCount = 0;

std::mutex g_ResultQueueMutex;
std::condition_variable g_ResultQueueCondVar;
std::queue<std::shared_ptr<AlgoRequest>> g_ResultQueue;
std::atomic<bool> g_quit(false);

bool g_HdrEnabled           = false;
bool g_WatermarkEnabled     = false;
bool g_MandlebrotSetEnabled = false;
bool g_FilterEnabled        = false;
bool g_LdcEnabled           = false;
bool g_BokehEnabled         = false;

std::vector<AlgoMetadataList> g_AlgoMetadataList = {
    {MetaId::ALGO_HDR_ENABLED, AlgoId::ALGO_HDR},
    {MetaId::ALGO_FILTER_ENABLED, AlgoId::ALGO_FILTER},
    {MetaId::ALGO_WATERMARK_ENABLED, AlgoId::ALGO_WATERMARK},
    {MetaId::ALGO_MANDELBROTSET_ENABLED, AlgoId::ALGO_MANDELBROTSET},
    {MetaId::ALGO_LDC_ENABLED, AlgoId::ALGO_LDC},
    {MetaId::ALGO_BOKEH_ENABLED, AlgoId::ALGO_BOKEH}};

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
void YUVtoRGB(const unsigned char* yuvBuffer, unsigned char* rgbBuffer,
              int width, int height) {
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
 * @brief Construct a new Algo Interfaceptr:: Algo Interfaceptr object
 *
 * @param path
 */
AlgoInterfaceptr::AlgoInterfaceptr(const std::string& path) {
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
  {
    std::unique_lock<std::mutex> lock(g_ResultQueueMutex);
    while (!g_ResultQueue.empty()) {
      g_ResultQueue.pop();
    }
  }
}
/**
 * @brief load Symbol from shared library
 *
 * @param symbolName
 * @return void*
 */
void* AlgoInterfaceptr::getSymbol(const char* symbolName) {
  return dlsym(libraryHandle, symbolName);
}

/**
 * @brief Construct a new Algo Interface Manager:: Algo Interface Manager object
 *
 */
AlgoInterfaceManager::AlgoInterfaceManager(
    std::vector<std::string> inputFilePath, int width, int height,
    bool stereo) {
  assert(inputFilePath.size() > 0);
  phandle = std::make_shared<AlgoInterfaceptr>(ALGOLIB_PATH);
  if (!phandle) {
    std::cerr << "Error: Failed to load algorithm library." << std::endl;
    return;
  }
  mWidth  = width;
  mHeight = height;
  if (InitAlgoInterface() != 0) {
    std::cerr << "Error: Failed to initialize algorithm interface."
              << std::endl;
    return;
  }
  auto size = mWidth * mHeight * 3 / 2;
  if (stereo) {
    assert(inputFilePath.size() == 2);
    mStereoInputFile[0].open(inputFilePath[0], std::ios::binary);
    if (!mStereoInputFile[0]) {
      std::cerr << "Failed to open input 1 file: " << inputFilePath[0]
                << std::endl;
      return;
    }
    mStereoInputFile[1].open(inputFilePath[1], std::ios::binary);
    if (!mStereoInputFile[1]) {
      std::cerr << "Failed to open input 2 file: " << inputFilePath[1]
                << std::endl;
      return;
    }
    yuvStereoBufferp[0].resize(size);
    yuvStereoBufferp[1].resize(size);
  } else {
    mInputFile.open(inputFilePath[0], std::ios::binary);
    if (!mInputFile) {
      std::cerr << "Failed to open input file: " << inputFilePath[0]
                << std::endl;
      return;
    }
    yuvBuffer.resize(size);
  }
}

/**
 * @brief Destroy the Algo Interface Manager:: Algo Interface Manager object
 *
 */
AlgoInterfaceManager::~AlgoInterfaceManager() {
  Cleanup();
}
#include <chrono>
#include <iostream>
int totalFrames = 0;
std::chrono::steady_clock::time_point Lasttime;
std::chrono::duration<double> totalElapsed(0);
// Callback Function
/**
 * @brief
 *
 * @param request
 * @return int
 */
int ProcessCallback(std::shared_ptr<AlgoRequest> request) {
  {
    /**fps calcualtion Logic */
    if (totalFrames == 0) {
      Lasttime = std::chrono::steady_clock::now();
    }
    totalFrames++;

    auto Currenttime = std::chrono::steady_clock::now();
    totalElapsed += (Currenttime - Lasttime);
    Lasttime = Currenttime;
    //if (totalElapsed.count() >= 1.0)
    if (totalFrames % 10 == 0) {  // Compute average FPS after 1 seconds
      double avgFPS = totalFrames / totalElapsed.count();
      std::cout << "[" << totalFrames << "] Average FPS: " << avgFPS << "\r"
                << std::flush;
    }
  }
  if (request) {
    std::shared_ptr<ImageData> image = request->GetImage(0);
    if (image) {
      if (image->GetDataSize() == 0) {
        return -1;
      }
      {
        {
          std::unique_lock<std::mutex> lock(g_ResultQueueMutex);
          g_ResultQueue.push(request);
          g_ResultQueueMutex.unlock();
        }
        g_ResultQueueCondVar.notify_one();
      }
      ++g_ResultCount;
    }
  }
  return 0;
}

/**
 * @brief Set the Metadata object
 *
 * @param request
 */
void SetMetadata(std::shared_ptr<AlgoRequest> request) {
  if (request) {
    request->mMetadata.SetMetadata(MetaId::ALGO_HDR_ENABLED, g_HdrEnabled);
    request->mMetadata.SetMetadata(MetaId::ALGO_WATERMARK_ENABLED,
                                   g_WatermarkEnabled);
    request->mMetadata.SetMetadata(MetaId::ALGO_MANDELBROTSET_ENABLED,
                                   g_MandlebrotSetEnabled);
    request->mMetadata.SetMetadata(MetaId::ALGO_FILTER_ENABLED,
                                   g_FilterEnabled);
    request->mMetadata.SetMetadata(MetaId::ALGO_LDC_ENABLED, g_LdcEnabled);
    request->mMetadata.SetMetadata(MetaId::ALGO_BOKEH_ENABLED, g_BokehEnabled);
  }
}

int AlgoInterfaceManager::SubmitRequest() {
  int rc = 0;
  if ((g_SubmittedCount - g_ResultCount < 20) || (g_SubmittedCount < 30)) {

    // read a yuv buffer from file and convert it to RGB

    if (!mInputFile.read(reinterpret_cast<char*>(yuvBuffer.data()),
                         yuvBuffer.size())) {
      mInputFile.clear();                  // Clear EOF flag
      mInputFile.seekg(0, std::ios::beg);  // Seek back to the start of the file
      mInputFile.read(reinterpret_cast<char*>(yuvBuffer.data()),
                      yuvBuffer.size());  // read after reset
    }

    // prepare and submit request
    auto request        = std::make_shared<AlgoRequest>();
    request->mRequestId = mRequestId++;
    if (false /*processRGB*/) {
      std::vector<unsigned char> rgbBuffer(mWidth * mHeight * 3);
      YUVtoRGB(yuvBuffer.data(), rgbBuffer.data(), mWidth, mHeight);
      rc = request->AddImage(ImageFormat::RGB, mWidth, mHeight,
                             std::move(rgbBuffer));
    } else {
      std::vector<unsigned char> Inputyuvbuf = yuvBuffer;
      rc = request->AddImage(ImageFormat::YUV420, mWidth, mHeight,
                             std::move(Inputyuvbuf));
    }
    if (rc != 0) {
      std::cerr << "Failed to add image to request rc=" << rc << std::endl;
      return -1;
    }
    request->mMetadata.SetMetadata(MetaId::IMAGE_WIDTH, mWidth);
    request->mMetadata.SetMetadata(MetaId::IMAGE_HEIGHT, mHeight);
    SetMetadata(request);
    std::vector<AlgoId> algoSuggested =
        m_algoDecisionManager.ParseMetadata(request);

    /*std::cerr << "Algo Suggested :";
    for (auto id : algoSuggested) {
      std::cerr << " " << algoName[ALGO_OFFSET(id)] << std::endl;
    }*/

    rc = phandle->processFunc(&phandle->libraryHandle, request, algoSuggested);
    if (rc != 0) {
      std::cerr << "Failed to process algorithm request rc = " << rc
                << std::endl;
      return -2;
    }
    ++g_SubmittedCount;
  } else {
    usleep(50);
  }
  return 0;
}

int AlgoInterfaceManager::SubmitStereoRequest() {
  int rc = 0;
  if ((g_SubmittedCount - g_ResultCount < 20) || (g_SubmittedCount < 30)) {

    // read a yuv buffer from file  assumed yuv image
    for (int i = 0; i < 2; ++i) {
      if (!mStereoInputFile[i].read(
              reinterpret_cast<char*>(yuvStereoBufferp[i].data()),
              yuvStereoBufferp[i].size())) {
        // Clear EOF flag
        mStereoInputFile[i].clear();
        // Seek back to the start of the file
        mStereoInputFile[i].seekg(0, std::ios::beg);
        // Read after reset
        mStereoInputFile[i].read(
            reinterpret_cast<char*>(yuvStereoBufferp[i].data()),
            yuvStereoBufferp[i].size());
      }
    }

    // prepare and submit request
    auto request                            = std::make_shared<AlgoRequest>();
    request->mRequestId                     = mRequestId++;
    std::vector<unsigned char> Inputyuvbuf1 = yuvStereoBufferp[0];
    std::vector<unsigned char> Inputyuvbuf2 = yuvStereoBufferp[1];
    rc = request->AddImage(ImageFormat::YUV420, mWidth, mHeight,
                           std::move(Inputyuvbuf1));
    if (rc != 0) {
      std::cerr << "Failed to add image to request 1 rc=" << rc << std::endl;
      return -1;
    }
    rc = request->AddImage(ImageFormat::YUV420, mWidth, mHeight,
                           std::move(Inputyuvbuf2));
    if (rc != 0) {
      std::cerr << "Failed to add image to request 2 rc=" << rc << std::endl;
      return -2;
    }
    request->mMetadata.SetMetadata(MetaId::IMAGE_WIDTH, mWidth);
    request->mMetadata.SetMetadata(MetaId::IMAGE_HEIGHT, mHeight);
    SetMetadata(request);
    std::vector<AlgoId> algoSuggested =
        m_algoDecisionManager.ParseMetadata(request);
    rc = phandle->processFunc(&phandle->libraryHandle, request, algoSuggested);
    if (rc != 0) {
      std::cerr << "Failed to process algorithm request rc = " << rc
                << std::endl;
      return -3;
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

  if (phandle->registerCallbackFunc(&phandle->libraryHandle, ProcessCallback) !=
      0) {
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
