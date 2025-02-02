#include "include/AlgoInterfaceManager.h"
#include "include/Renderer.h"
#include <cstring>
#include <memory>
#include <pthread.h>
#include <thread>
#include <unistd.h>
extern bool g_quit;
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

const int idx = 2;

int main() {

  int width                                                   = yuvFiles[idx].width;
  int height                                                  = yuvFiles[idx].height;
  std::shared_ptr<AlgoInterfaceManager> pAlgoInterfaceManager = std::make_shared<AlgoInterfaceManager>(yuvFiles[idx].filePath, width, height);
  std::shared_ptr<Renderer> pRenderer                         = std::make_shared<Renderer>(width, height);
#if 0
  pRenderer->RenderLoop(pAlgoInterfaceManager);
#else
  /*decople render and submit on diffrent threads */
  auto submit = [pRenderer, pAlgoInterfaceManager]() {
    while (!g_quit) {
      pAlgoInterfaceManager->SubmitRequest();
    }
  };

  std::thread t2(submit);
  pRenderer->RenderLoop(nullptr);
  t2.join();
#endif

  return 0;
}