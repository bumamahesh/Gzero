#include "AlgoRequest.h"
#include "Interface.h"
#include <gtest/gtest.h>
#include <memory>

int g_ProcessedFrames;
TEST(AlgoInterfaceTest, ProcessTest) {

  void *libhandle = nullptr;
  auto callback = [](std::shared_ptr<AlgoRequest> input) -> int {
    g_ProcessedFrames++;
    return 0;
  };
  InitAlgoInterface(&libhandle);
  RegisterCallback(&libhandle, callback);
  auto request = std::make_shared<AlgoRequest>();
  for (int i = 0; i < 100; i++) {
    AlgoInterfaceProcess(&libhandle, request);
  }
  DeInitAlgoInterface(&libhandle);
  EXPECT_EQ(g_ProcessedFrames, 100);
}
