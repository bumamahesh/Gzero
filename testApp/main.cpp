#include "include/AlgoInterfaceManager.h"
#include "include/Renderer.h"
#include <atomic>
#include <cstring>
#include <iostream>
#include <memory>
#include <pthread.h>
#include <thread>
#include <unistd.h>

extern std::atomic<bool> g_quit;
struct YUVFile {
  std::string filePath;
  int width;
  int height;
};

const YUVFile yuvFiles[] = {
    {"/home/uma/workspace/Gzero/res/lena.yuv", 352, 288},
    {"/home/uma/workspace/Gzero/res/test_yuv420p_320x180.yuv", 320, 180},
    {"/home/uma/workspace/Gzero/res/1920x1080.yuv", 1920, 1080},
    {"/home/uma/workspace/Gzero/res/video_test_vector.yuv", 1280, 720}};

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " <index>" << std::endl;
    return 1;
  }

  std::cout << "Number of arguments: " << argc << std::endl;

  // Loop through all the arguments and print them
  for (int i = 0; i < argc; ++i) {
    std::cout << "Argument " << i << ": " << argv[i] << std::endl;
  }

  int idx = std::stoi(argv[1]);

  if (idx >= sizeof(yuvFiles) / sizeof(YUVFile)) {
    std::cerr << "Index out of range, defaulting to 0" << std::endl;
    idx = 0;
  }

  int width                                                   = yuvFiles[idx].width;
  int height                                                  = yuvFiles[idx].height;
  std::shared_ptr<AlgoInterfaceManager> pAlgoInterfaceManager = std::make_shared<AlgoInterfaceManager>(yuvFiles[idx].filePath, width, height);
  std::shared_ptr<Renderer> pRenderer                         = std::make_shared<Renderer>(width, height);

  for (int i = 2; i < argc; ++i) {
    if (strcmp(argv[i], "m") == 0) {
      pAlgoInterfaceManager->m_algoDecisionManager.SetAlgoFlag(ALGO_MANDELBROTSET);
      std::cout << "ALGO_MANDELBROTSET  is set" << std::endl;
    } else if (strcmp(argv[i], "f") == 0) {
      pAlgoInterfaceManager->m_algoDecisionManager.SetAlgoFlag(ALGO_FILTER);
      std::cout << "ALGO_FILTER  is set" << std::endl;
    } else if (strcmp(argv[i], "w") == 0) {
      pAlgoInterfaceManager->m_algoDecisionManager.SetAlgoFlag(ALGO_WATERMARK);
      std::cout << "ALGO_WATERMARK  is set" << std::endl;
    } else {
      std::cerr << "Unknown algorithm flag: " << argv[i] << std::endl;
    }
  }

#if 0
  pRenderer->RenderLoop(pAlgoInterfaceManager);
#else
  /* decouple render and submit on different threads */
  auto submit = [pRenderer, pAlgoInterfaceManager]() {
    int rc = 0;
    while (!g_quit.load()) {
      rc = pAlgoInterfaceManager->SubmitRequest();
      if (rc != 0) {
        g_quit.store(true);
      }
    }
  };

  std::thread t2(submit);
  pRenderer->RenderLoop(nullptr);
  t2.join();
#endif

  return 0;
}
