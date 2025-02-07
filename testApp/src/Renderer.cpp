#include "../include/Renderer.h"
#include <atomic>
#include <condition_variable>
#include <iostream>
#include <queue>
#include <unistd.h>

extern std::mutex g_ResultQueueMutex;
extern std::condition_variable g_ResultQueueCondVar;
extern std::queue<std::shared_ptr<AlgoRequest>> g_ResultQueue;
extern std::atomic<bool> g_quit;

/**
 * @brief Construct a new Renderer:: Renderer object
 *
 */
Renderer::Renderer(int width, int height) : mWindow(nullptr), mRenderer(nullptr), mTexture(nullptr), mWidth(width), mHeight(height) {
  if (!Initialize()) {
    std::cerr << "Failed to initialize Renderer." << std::endl;
    Cleanup();
  }
}

/**
 * @brief Destroy the Renderer:: Renderer object
 *
 */
Renderer::~Renderer() {
  Cleanup();
}

/**
 * @brief
 *
 * @return true
 * @return false
 */
bool Renderer::Initialize() {
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
    return false;
  }

  mWindow = SDL_CreateWindow("GZero Test App", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, mWidth, mHeight, SDL_WINDOW_SHOWN);
  if (!mWindow) {
    std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
    SDL_Quit();
    return false;
  }

  mRenderer = SDL_CreateRenderer(mWindow, -1, SDL_RENDERER_ACCELERATED);
  if (!mRenderer) {
    std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
    SDL_DestroyWindow(mWindow);
    SDL_Quit();
    return false;
  }

  Uint32 format = (isYUV) ? SDL_PIXELFORMAT_IYUV : SDL_PIXELFORMAT_RGB24;
  mTexture      = SDL_CreateTexture(mRenderer, format, SDL_TEXTUREACCESS_STREAMING, mWidth, mHeight);
  if (!mTexture) {
    std::cerr << "Texture could not be created! SDL_Error: " << SDL_GetError() << std::endl;
    SDL_DestroyRenderer(mRenderer);
    SDL_DestroyWindow(mWindow);
    SDL_Quit();
    return false;
  }

  return true;
}

/**
 * @brief
 *
 * @param processFunc
 * @param libraryHandle
 * @param inputFilePath
 */
void Renderer::RenderLoop(std::shared_ptr<AlgoInterfaceManager> pAlgoInteface) {
  SDL_Event event;
  // const int targetFPS  = 120;
  // const int frameDelay = 1000 / targetFPS; // ms per frame

  while (!g_quit.load()) {
    // Uint32 frameStart = SDL_GetTicks();

    while (SDL_PollEvent(&event) != 0) {
      if (event.type == SDL_QUIT) {
        g_quit.store(true);
        std::cerr << "Sdl Quite Event" << std::endl;
        break;
      }
    }

    std::shared_ptr<AlgoRequest> request; // Try to lock, and avoid blocking forever
    {
      std::unique_lock<std::mutex> lock(g_ResultQueueMutex);
      if (g_ResultQueue.empty()) {
        continue;
      }
      request = g_ResultQueue.front();
      g_ResultQueue.pop();
    }
    if (!request) {
      continue;
    }

    std::shared_ptr<ImageData> image = request->GetImage(0);

    {
      /*image validation logic */
      if (!image || !image->GetDataSize()) {
        std::cerr << "Error: Image data is null or size is zero." << std::endl;
        continue;
      }
      bool isrgb              = (image->GetFormat() == ImageFormat::RGB) ? true : false;
      size_t Width            = image->GetWidth();
      size_t Height           = image->GetHeight();
      size_t OutputBufferSize = (isrgb) ? (Width * Height * 3) : (Width * Height * 3 / 2); // either RGB or YUV

      // Check if the image data size is correct
      if (image->GetData().size() < OutputBufferSize) {
        std::cerr << "Error: Image data size is smaller than expected."
                  << std::endl;
        continue;
      }
    }

    unsigned char *OutputBuffer = image->GetData().data();
    if (image->GetFormat() == ImageFormat::YUV420) {

      // YUV420p format requires separate planes
      SDL_UpdateYUVTexture(mTexture, nullptr,
                           OutputBuffer, mWidth,                                   // Y plane
                           OutputBuffer + (mWidth * mHeight), mWidth / 2,          // U plane
                           OutputBuffer + (mWidth * mHeight * 5 / 4), mWidth / 2); // V plane
    } else {
      SDL_UpdateTexture(mTexture, nullptr, OutputBuffer, mWidth * 3);
    }

    SDL_RenderClear(mRenderer);
    SDL_RenderCopy(mRenderer, mTexture, nullptr, nullptr);
    SDL_RenderPresent(mRenderer);
  }

  if (pAlgoInteface) {
    pAlgoInteface->SubmitRequest();
  }

  // Limit the frame rate to targetFPS
  // Uint32 frameEnd  = SDL_GetTicks();
  // Uint32 frameTime = frameEnd - frameStart;
  // if (frameTime < frameDelay) {
  // SDL_Delay(frameDelay - frameTime);
  //}
}

void Renderer::Cleanup() {

  SDL_DestroyTexture(mTexture);
  SDL_DestroyRenderer(mRenderer);
  SDL_DestroyWindow(mWindow);
  SDL_Quit();
  mWindow   = nullptr;
  mRenderer = nullptr;
  mTexture  = nullptr;
}