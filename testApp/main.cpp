#include "../include/AlgoDefs.h"
#include "../include/AlgoRequest.h"
#include "AlgoDecisionManager.h"
#include <SDL2/SDL.h>
#include <algorithm>
#include <cstring>
#include <dlfcn.h>
#include <fstream>
#include <iostream>
#include <memory>
#include <mutex>
#include <unistd.h>
#include <vector>

#define WIDTH 1920
#define HEIGHT 1080

//#define YUV_TEST_FILE "/home/uma/workspace/Gzero/testApp/lena.yuv" //352 X288
//#define YUV_TEST_FILE
//"/home/uma/workspace/Gzero/testApp/test_yuv420p_320x180.yuv" // 320 x180
#define YUV_TEST_FILE "/home/uma/workspace/Gzero/testApp/1920x1080.yuv"

template <typename T> T clamp(T value, T min, T max) {
  if (value < min)
    return min;
  if (value > max)
    return max;
  return value;
}

using InitAlgoInterfaceFunc = int (*)(void **);
using DeInitAlgoInterfaceFunc = int (*)(void **);
using AlgoInterfaceProcessFunc = int (*)(void **, std::shared_ptr<AlgoRequest>,
                                         std::vector<AlgoId>);
using RegisterCallbackFunc = int (*)(void **,
                                     int (*)(std::shared_ptr<AlgoRequest>));

class DecisionManager : public AlgoDecisionManager {
public:
  DecisionManager() = default;
  ~DecisionManager() = default;
  std::vector<AlgoId> ParseMetadata(std::shared_ptr<AlgoRequest> req) override {
    SetAlgoFlag(ALGO_FILTER);
    SetAlgoFlag(ALGO_WATERMARK);
    return AlgoDecisionManager::ParseMetadata(req);
  }
} g_algoDecisionManager;

// Global Variables
unsigned char *g_rgbBuffer = nullptr;
volatile int g_dataReadyFlag = 0;
SDL_Renderer *g_renderer = nullptr;
SDL_Texture *g_texture = nullptr;
std::mutex g_rgbBufferMutex;
int g_submittedCount = 0;
int g_resultCount = 0;

// Callback Function
int ProcessCallback(std::shared_ptr<AlgoRequest> request) {
  if (request) {
    std::shared_ptr<ImageData> image = request->GetImage(0);
    if (image) {
      std::lock_guard<std::mutex> lock(g_rgbBufferMutex);
      memcpy(g_rgbBuffer, image->data.data(), WIDTH * HEIGHT * 3);
      g_dataReadyFlag = 1;
      ++g_resultCount;
    }
  }
  return 0;
}

void Cleanup(SDL_Window *window, void *libraryHandle,
             DeInitAlgoInterfaceFunc deinitFunc) {
  free(g_rgbBuffer);
  SDL_DestroyTexture(g_texture);
  SDL_DestroyRenderer(g_renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
  if (deinitFunc) {
    deinitFunc(&libraryHandle);
  }
  if (libraryHandle) {
    dlclose(libraryHandle);
  }
}

bool InitializeSDL(SDL_Window *&window) {
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError()
              << std::endl;
    return false;
  }

  window = SDL_CreateWindow("YUV to RGB Display", SDL_WINDOWPOS_UNDEFINED,
                            SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT,
                            SDL_WINDOW_SHOWN);
  if (!window) {
    std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError()
              << std::endl;
    SDL_Quit();
    return false;
  }

  g_renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  if (!g_renderer) {
    std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError()
              << std::endl;
    SDL_DestroyWindow(window);
    SDL_Quit();
    return false;
  }

  g_texture = SDL_CreateTexture(g_renderer, SDL_PIXELFORMAT_RGB24,
                                SDL_TEXTUREACCESS_STREAMING, WIDTH, HEIGHT);
  if (!g_texture) {
    std::cerr << "Texture could not be created! SDL_Error: " << SDL_GetError()
              << std::endl;
    SDL_DestroyRenderer(g_renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return false;
  }

  g_rgbBuffer = static_cast<unsigned char *>(malloc(WIDTH * HEIGHT * 3));
  if (!g_rgbBuffer) {
    std::cerr << "Failed to allocate memory for RGB data." << std::endl;
    SDL_DestroyTexture(g_texture);
    SDL_DestroyRenderer(g_renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return false;
  }

  return true;
}

bool LoadLibraryFunctions(void *&libraryHandle, InitAlgoInterfaceFunc &initFunc,
                          DeInitAlgoInterfaceFunc &deinitFunc,
                          AlgoInterfaceProcessFunc &processFunc,
                          RegisterCallbackFunc &registerCallbackFunc) {
  libraryHandle =
      dlopen("/home/uma/workspace/Gzero/cmake/lib/libAlgoLib.so", RTLD_LAZY);
  if (!libraryHandle) {
    std::cerr << "Failed to load library." << std::endl;
    return false;
  }

  initFunc = reinterpret_cast<InitAlgoInterfaceFunc>(
      dlsym(libraryHandle, "InitAlgoInterface"));
  deinitFunc = reinterpret_cast<DeInitAlgoInterfaceFunc>(
      dlsym(libraryHandle, "DeInitAlgoInterface"));
  processFunc = reinterpret_cast<AlgoInterfaceProcessFunc>(
      dlsym(libraryHandle, "AlgoInterfaceProcess"));
  registerCallbackFunc = reinterpret_cast<RegisterCallbackFunc>(
      dlsym(libraryHandle, "RegisterCallback"));

  if (!initFunc || !deinitFunc || !processFunc || !registerCallbackFunc) {
    std::cerr << "Failed to load one or more functions from the library."
              << std::endl;
    dlclose(libraryHandle);
    return false;
  }

  return true;
}

void YUVtoRGB(const unsigned char *yuvBuffer, unsigned char *rgbBuffer) {
  for (int i = 0; i < HEIGHT; ++i) {
    for (int j = 0; j < WIDTH; ++j) {
      int yIndex = i * WIDTH + j;
      int uvIndex = (i / 2) * (WIDTH / 2) + (j / 2);

      int Y = yuvBuffer[yIndex];
      int U = yuvBuffer[WIDTH * HEIGHT + uvIndex] - 128;
      int V = yuvBuffer[WIDTH * HEIGHT + (WIDTH * HEIGHT / 4) + uvIndex] - 128;

      int R = Y + 1.402 * V;
      int G = Y - 0.344136 * U - 0.714136 * V;
      int B = Y + 1.772 * U;

      rgbBuffer[(i * WIDTH + j) * 3 + 0] = clamp(R, 0, 255);
      rgbBuffer[(i * WIDTH + j) * 3 + 1] = clamp(G, 0, 255);
      rgbBuffer[(i * WIDTH + j) * 3 + 2] = clamp(B, 0, 255);
    }
  }
}

void RenderLoop(SDL_Window *window, AlgoInterfaceProcessFunc processFunc,
                void *libraryHandle, const std::string &inputFilePath) {
  std::ifstream inputFile(inputFilePath, std::ios::binary);
  if (!inputFile) {
    std::cerr << "Failed to open input file: " << inputFilePath << std::endl;
    return;
  }

  std::vector<unsigned char> yuvBuffer(WIDTH * HEIGHT * 3 / 2);
  std::vector<unsigned char> rgbBuffer(WIDTH * HEIGHT * 3);

  int requestId = 0;
  SDL_Event event;
  bool quit = false;

  while (!quit) {
    while (SDL_PollEvent(&event) != 0) {
      if (event.type == SDL_QUIT) {
        quit = true;
      }
    }

    if (g_dataReadyFlag) {
      std::lock_guard<std::mutex> lock(g_rgbBufferMutex);
      SDL_UpdateTexture(g_texture, nullptr, g_rgbBuffer, WIDTH * 3);
      SDL_RenderClear(g_renderer);
      SDL_RenderCopy(g_renderer, g_texture, nullptr, nullptr);
      SDL_RenderPresent(g_renderer);
      g_dataReadyFlag = 0;
    }

    if ((g_submittedCount - g_resultCount < 20) || (g_submittedCount < 30)) {
      if (inputFile.read(reinterpret_cast<char *>(yuvBuffer.data()),
                         yuvBuffer.size())) {
        YUVtoRGB(yuvBuffer.data(), rgbBuffer.data());

        auto request = std::make_shared<AlgoRequest>();
        request->mRequestId = requestId++;
        request->AddImage(ImageFormat::RGB, WIDTH, HEIGHT, rgbBuffer);

        int status = processFunc(&libraryHandle, request,
                                 g_algoDecisionManager.ParseMetadata(request));
        if (status != 0) {
          std::cerr << "Failed to process algorithm request." << std::endl;
          quit = true;
        }
        ++g_submittedCount;
      } else {
        inputFile.clear();                 // Clear EOF flag
        inputFile.seekg(0, std::ios::beg); // Seek back to the start of the file
      }
    } else {
      usleep(50);
    }
  }

  inputFile.close();
}

int main() {
  SDL_Window *window = nullptr;
  void *libraryHandle = nullptr;
  InitAlgoInterfaceFunc initFunc = nullptr;
  DeInitAlgoInterfaceFunc deinitFunc = nullptr;
  AlgoInterfaceProcessFunc processFunc = nullptr;
  RegisterCallbackFunc registerCallbackFunc = nullptr;

  if (!InitializeSDL(window)) {
    return -1;
  }

  if (!LoadLibraryFunctions(libraryHandle, initFunc, deinitFunc, processFunc,
                            registerCallbackFunc)) {
    Cleanup(window, nullptr, nullptr);
    return -1;
  }

  if (initFunc(&libraryHandle) != 0) {
    std::cerr << "Failed to initialize algorithm interface." << std::endl;
    Cleanup(window, libraryHandle, deinitFunc);
    return -1;
  }

  if (registerCallbackFunc(&libraryHandle, ProcessCallback) != 0) {
    std::cerr << "Failed to register callback." << std::endl;
    Cleanup(window, libraryHandle, deinitFunc);
    return -1;
  }

  const std::string inputFilePath = YUV_TEST_FILE;
  RenderLoop(window, processFunc, libraryHandle, inputFilePath);
  Cleanup(window, libraryHandle, deinitFunc);

  return 0;
}
