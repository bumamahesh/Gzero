#include "../include/Renderer.h"
#include <condition_variable>
#include <iostream>
#include <queue>

extern unsigned char *g_rgbBuffer;
extern std::mutex g_rgbBufferMutex;
extern bool g_quit;

extern std::mutex g_ResultQueueMutex;
extern std::condition_variable g_ResultQueueCondVar;
extern std::queue<unsigned char *> g_ResultQueue;
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

  mTexture = SDL_CreateTexture(mRenderer, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_STREAMING, mWidth, mHeight);
  if (!mTexture) {
    std::cerr << "Texture could not be created! SDL_Error: " << SDL_GetError() << std::endl;
    SDL_DestroyRenderer(mRenderer);
    SDL_DestroyWindow(mWindow);
    SDL_Quit();
    return false;
  }

  g_rgbBuffer = static_cast<unsigned char *>(malloc(mWidth * mHeight * 3));
  if (!g_rgbBuffer) {
    std::cerr << "Failed to allocate memory for RGB data." << std::endl;
    SDL_DestroyTexture(mTexture);
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
  int requestId = 0;
  SDL_Event event;

  const int targetFPS  = 120;
  const int frameDelay = 1000 / targetFPS; // ms per frame

  while (!g_quit) {
    Uint32 frameStart = SDL_GetTicks();

    while (SDL_PollEvent(&event) != 0) {
      if (event.type == SDL_QUIT) {
        g_quit = true;
      }
    }

    unsigned char *rgbBuffer = nullptr;

    // Try to lock, and avoid blocking forever
    {
      std::unique_lock<std::mutex> lock(g_ResultQueueMutex);
      if (g_ResultQueue.empty()) {
        continue;
      }

      rgbBuffer = g_ResultQueue.front();
      g_ResultQueue.pop();
    }

    if (rgbBuffer) {
      SDL_UpdateTexture(mTexture, nullptr, rgbBuffer, mWidth * 3);
      SDL_RenderClear(mRenderer);
      SDL_RenderCopy(mRenderer, mTexture, nullptr, nullptr);
      SDL_RenderPresent(mRenderer);
      free(rgbBuffer);
    }

    if (pAlgoInteface) {
      pAlgoInteface->SubmitRequest();
    }

    // Limit the frame rate to targetFPS
    Uint32 frameEnd  = SDL_GetTicks();
    Uint32 frameTime = frameEnd - frameStart;
    if (frameTime < frameDelay) {
      SDL_Delay(frameDelay - frameTime);
    }
  }
}

void Renderer::Cleanup() {
  if (g_rgbBuffer) {
    free(g_rgbBuffer);
  }
  SDL_DestroyTexture(mTexture);
  SDL_DestroyRenderer(mRenderer);
  SDL_DestroyWindow(mWindow);
  SDL_Quit();
  mWindow     = nullptr;
  mRenderer   = nullptr;
  mTexture    = nullptr;
  g_rgbBuffer = nullptr;
}