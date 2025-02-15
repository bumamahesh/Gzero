#include "include/AlgoInterfaceManager.h"
#ifdef __RENDER__
#include "include/Renderer.h"
#endif
#include <unistd.h>
#include <atomic>
#include <cstring>
#include <iostream>
#include <memory>
#include <thread>

extern std::atomic<bool> g_quit;
extern bool g_HdrEnabled;
extern bool g_WatermarkEnabled;
extern bool g_MandlebrotSetEnabled;
extern bool g_FilterEnabled;
extern bool g_LdcEnabled;
extern bool g_BokehEnabled;
bool g_DisplayEnable = false;
struct YUVFile {
  std::string filePath;
  int width;
  int height;
};

const std::string RESOURCE_PATH = "/home/uma/workspace/Gzero/res/";
const YUVFile yuvFiles[]        = {
           {"lena.yuv", 352, 288},        {"test_yuv420p_320x180.yuv", 320, 180},
           {"1920x1080.yuv", 1920, 1080}, {"video_test_vector.yuv", 1280, 720},
           {"tsukuba_l.yuv", 384, 288},   {"tsukuba_r.yuv", 384, 288},
};

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " <index>" << std::endl;
    return 1;
  }

  std::cout << "Number of arguments: " << argc << std::endl;

  // Loop through all the arguments and print them
  for (int i = 0; i < argc; ++i) {
    std::cout << "Argument " << i << ": " << argv[i] << std::endl;
  }

  int idx;

  char* endptr;
  long idx_long = strtol(argv[1], &endptr, 10);
  if (*endptr != '\0' || errno == ERANGE || idx_long < 0 ||
      idx_long >= (long)(sizeof(yuvFiles) / sizeof(YUVFile))) {
    std::cerr << "Invalid index, defaulting to 0" << std::endl;
    idx = 0;
  } else {
    idx = static_cast<int>(idx_long);
  }

  g_HdrEnabled           = false;
  g_WatermarkEnabled     = false;
  g_MandlebrotSetEnabled = false;
  g_FilterEnabled        = false;
  g_LdcEnabled           = false;
  g_BokehEnabled         = false;
  g_DisplayEnable        = false;

  for (int i = 2; i < argc; ++i) {
    if (strcmp(argv[i], "m") == 0) {
      g_MandlebrotSetEnabled = true;
      std::cout << "ALGO_MANDELBROTSET  is set" << std::endl;
    } else if (strcmp(argv[i], "f") == 0) {
      g_FilterEnabled = true;
      std::cout << "ALGO_FILTER  is set" << std::endl;
    } else if (strcmp(argv[i], "w") == 0) {
      g_WatermarkEnabled = true;
      std::cout << "ALGO_WATERMARK  is set" << std::endl;
    } else if (strcmp(argv[i], "h") == 0) {
      g_HdrEnabled = true;
      std::cout << "ALGO_HDR  is set" << std::endl;
    } else if (strcmp(argv[i], "l") == 0) {
      g_LdcEnabled = true;
      std::cout << "ALGO_LDC  is set" << std::endl;
    } else if (strcmp(argv[i], "D") == 0) {
      g_DisplayEnable = true;
      std::cout << "Display is set" << std::endl;
    } else if (strcmp(argv[i], "b") == 0) {
      g_BokehEnabled = true;
      idx            = 4;
      std::cout << "ALGO_BOKEH  is set" << std::endl;
    } else {
      std::cerr << "Unknown algorithm flag: " << argv[i] << std::endl;
    }
  }
  std::cerr << " g_HdrEnabled = " << g_HdrEnabled
            << " g_WatermarkEnabled = " << g_WatermarkEnabled
            << " g_MandlebrotSetEnabled = " << g_MandlebrotSetEnabled
            << " g_FilterEnabled = " << g_FilterEnabled
            << " g_LdcEnabled = " << g_LdcEnabled
            << " g_BokehEnabled = " << g_BokehEnabled << std::endl;

  int width  = yuvFiles[idx].width;
  int height = yuvFiles[idx].height;
  std::vector<std::string> inputFilePath;
  if (g_BokehEnabled) {
    inputFilePath.push_back(RESOURCE_PATH + yuvFiles[idx + 0].filePath);
    inputFilePath.push_back(RESOURCE_PATH + yuvFiles[idx + 1].filePath);
  } else {
    inputFilePath.push_back(RESOURCE_PATH + yuvFiles[idx].filePath);
  }
  auto pAlgoInterfaceManager = std::make_shared<AlgoInterfaceManager>(
      inputFilePath, width, height, g_BokehEnabled);

#ifdef __RENDER__
  std::shared_ptr<Renderer> pRenderer = nullptr;
  if (g_DisplayEnable) {
    pRenderer = std::make_shared<Renderer>(width, height);
  }
#endif
#ifdef SYNC_THREAD
  if (pRenderer != nullptr) {
    pRenderer->RenderLoop(pAlgoInterfaceManager);
  }
#else
  /* decouple render and submit on different threads */
  auto submit = [pAlgoInterfaceManager]() {
    int rc = 0;
    while (!g_quit.load()) {
      if (!g_BokehEnabled) {
        rc = pAlgoInterfaceManager->SubmitRequest();
      } else {
        rc = pAlgoInterfaceManager->SubmitStereoRequest();
      }
      if (rc != 0) {
        g_quit.store(true);
      }
    }
    std::cerr << "Exiting submit thread" << std::endl;
  };

  std::thread t2(submit);
  if (g_DisplayEnable) {
#ifdef __RENDER__
    if (pRenderer != nullptr) {
      pRenderer->RenderLoop(nullptr);
    }
#endif
  } else {
    while (!g_quit.load()) {
      std::string name;
      std::getline(std::cin, name);  // Reads the entire line
      if (name == "e") {
        g_quit.store(true);
      }
    }
  }
  t2.join();
#endif
  return 0;
}
