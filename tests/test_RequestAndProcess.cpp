/*
 * Copyright (c) [2025] [Uma Mahesh B]
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#include "../include/AlgoDecisionManager.h"
#include "../include/AlgoDefs.h"
#include "../include/AlgoMetadata.h"
#include "../include/AlgoRequest.h"
#include <dlfcn.h>
#include <gtest/gtest.h>
#include <memory>

#define WIDTH 640
#define HEIGHT 480
// Type aliases for function pointers
typedef int (*Init)(void **);
typedef int (*DeInit)(void **);
typedef int (*Process)(void **, std::shared_ptr<AlgoRequest>,
                       std::vector<AlgoId>);
typedef int (*Callback)(void **,
                        int (*)(std::shared_ptr<AlgoRequest>));

// DecisionManager class
class DecisionManager : public AlgoDecisionManager {
public:
  std::vector<AlgoId>
  ParseMetadata(std::shared_ptr<AlgoRequest> req) override;

private:
  bool bIsHdrenabled       = false;
  bool bIswatermarkenabled = false;
};

std::vector<AlgoId>
DecisionManager::ParseMetadata(std::shared_ptr<AlgoRequest> req) {
  std::vector<AlgoId> algos;

  if (req) {
    req->mMetadata.GetMetadata(ALGO_HDR_ENABLED, bIsHdrenabled);
    req->mMetadata.GetMetadata(ALGO_WATERMARK_ENABLED, bIswatermarkenabled);
  }

  if (bIsHdrenabled) {
    algos.push_back(ALGO_HDR);
  }
  if (bIswatermarkenabled) {
    algos.push_back(ALGO_WATERMARK);
  }
  AlgoDecisionManager::ParseMetadata(req);
  return algos;
}

// Test Fixture Class
class RequestReponseTest : public ::testing::Test {
protected:
  void *libhandle = nullptr;

  // Function pointers
  Init InitAlgoInterface       = nullptr;
  DeInit DeInitAlgoInterface   = nullptr;
  Process AlgoInterfaceProcess = nullptr;
  Callback RegisterCallback    = nullptr;
  DecisionManager decisionManager;

  void SetUp() override {
    // Load the shared library
    libhandle =
        dlopen("/home/uma/workspace/Gzero/cmake/lib/libAlgoLib.so", RTLD_LAZY);
    ASSERT_NE(libhandle, nullptr)
        << "Failed to load shared library: " << dlerror();

    // Load the functions
    InitAlgoInterface = reinterpret_cast<Init>(
        dlsym(libhandle, "InitAlgoInterface"));
    ASSERT_NE(InitAlgoInterface, nullptr)
        << "Failed to load InitAlgoInterface: " << dlerror();

    DeInitAlgoInterface = reinterpret_cast<DeInit>(
        dlsym(libhandle, "DeInitAlgoInterface"));
    ASSERT_NE(DeInitAlgoInterface, nullptr)
        << "Failed to load DeInitAlgoInterface: " << dlerror();

    AlgoInterfaceProcess = reinterpret_cast<Process>(
        dlsym(libhandle, "AlgoInterfaceProcess"));
    ASSERT_NE(AlgoInterfaceProcess, nullptr)
        << "Failed to load AlgoInterfaceProcess: " << dlerror();

    RegisterCallback = reinterpret_cast<Callback>(
        dlsym(libhandle, "RegisterCallback"));
    ASSERT_NE(RegisterCallback, nullptr)
        << "Failed to load RegisterCallback: " << dlerror();
  }

  void TearDown() override {
    if (libhandle) {
      dlclose(libhandle);
    }
  }
};

// Callback function for testing
int g_Responsecbs = 0;
int ProcessedFlag = 0x00;
int ImageResponseCallback(std::shared_ptr<AlgoRequest> input) {
  // Simulate callback logic
  g_Responsecbs++;
  if (input) {
    input->mMetadata.GetMetadata(ALGO_PROCESS_DONE, ProcessedFlag);
  }
  return 0; // Return success
}
TEST_F(RequestReponseTest, ProcessRequestAndCheckResponse) {
  void *algoHandle = nullptr;

  // Initialize the library
  int status = InitAlgoInterface(&algoHandle);
  ASSERT_EQ(status, 0) << "InitAlgoInterface failed with status: " << status;

  // Register a callback
  status = RegisterCallback(&algoHandle, ImageResponseCallback);
  ASSERT_EQ(status, 0) << "RegisterCallback failed with status: " << status;

  g_Responsecbs = 0;
  // Create a mock AlgoRequest
  std::vector<unsigned char> yuvData(WIDTH * HEIGHT * 3 / 2); // YUV420 format
  auto request        = std::make_shared<AlgoRequest>();
  request->mRequestId = 123;
  int rc              = request->AddImage(ImageFormat::YUV420, WIDTH, HEIGHT, yuvData);
  ASSERT_EQ(rc, 0);
  rc = request->mMetadata.SetMetadata(ALGO_HDR_ENABLED, true);
  ASSERT_EQ(rc, 0);
  rc = request->mMetadata.SetMetadata(ALGO_WATERMARK_ENABLED, true);
  ASSERT_EQ(rc, 0);

  // Process the request
  status = AlgoInterfaceProcess(&algoHandle, request, decisionManager.ParseMetadata(request));
  ASSERT_EQ(status, 0) << "AlgoInterfaceProcess failed with status:" << status;

  while (g_Responsecbs == 0) {
    usleep(50);
  }
  // Deinitialize the library
  status = DeInitAlgoInterface(&algoHandle);

  ASSERT_EQ(status, 0) << "DeInitAlgoInterface failed with status:" << status;

  // check Processed algos
  int ExpectedFlag = (1 << ALGO_OFFSET(ALGO_HDR)) | (1 << ALGO_OFFSET(ALGO_WATERMARK));
  ASSERT_EQ(ProcessedFlag, ExpectedFlag);
}
