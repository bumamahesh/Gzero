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

#include <fstream>
#include "test_common.h"

// Callback function for testing
int g_AlgoProcessTestCallback      = 0;
int g_ProcessedFlagAlgoProcessTest = 0x00;
int AlgoProcessTestResponseCallback(std::shared_ptr<AlgoRequest> input) {
  // Simulate callback logic
  g_AlgoProcessTestCallback++;

  if (input) {

    int rc = input->mMetadata.GetMetadata(ALGO_PROCESS_DONE,
                                          g_ProcessedFlagAlgoProcessTest);
    /*std::cerr << "Output Format ::" << (int)input->GetImage(0)->GetFormat()
              << "rc =" << rc << "g_ProcessedFlagAlgoProcessTest::"
              << g_ProcessedFlagAlgoProcessTest << std::endl;*/
    if (rc != 0) {
      return -1;
    }
  }
  if (input && input->GetImage(0)->GetFormat() == ImageFormat::JPEG) {
    //std::cerr << "we have Jpeg Image " << std::endl;
    std::ofstream outFile("output.jpeg", std::ios::binary);
    if (outFile.is_open()) {
      outFile.write(
          reinterpret_cast<const char*>(input->GetImage(0)->GetData().data()),
          input->GetImage(0)->GetData().size());
      outFile.close();
    } else {
      std::cerr << "Failed to open file for writing JPEG data" << std::endl;
    }
  }
  return 0;  // Return success
}
// Test Fixture Class
class AlgoProcessTest : public ::testing::Test {
 protected:
  void* libhandle  = nullptr;
  void* algoHandle = nullptr;
  // Function pointers
  Init InitAlgoInterface       = nullptr;
  DeInit DeInitAlgoInterface   = nullptr;
  Process AlgoInterfaceProcess = nullptr;
  Callback RegisterCallback    = nullptr;

  void SetUp() override {
    // Load the shared library
    libhandle =
        dlopen("/home/uma/workspace/Gzero/cmake/lib/libAlgoLib.so", RTLD_LAZY);
    ASSERT_NE(libhandle, nullptr)
        << "Failed to load shared library: " << dlerror();

    // Load the functions
    InitAlgoInterface =
        reinterpret_cast<Init>(dlsym(libhandle, "InitAlgoInterface"));
    ASSERT_NE(InitAlgoInterface, nullptr)
        << "Failed to load InitAlgoInterface: " << dlerror();

    DeInitAlgoInterface =
        reinterpret_cast<DeInit>(dlsym(libhandle, "DeInitAlgoInterface"));
    ASSERT_NE(DeInitAlgoInterface, nullptr)
        << "Failed to load DeInitAlgoInterface: " << dlerror();

    AlgoInterfaceProcess =
        reinterpret_cast<Process>(dlsym(libhandle, "AlgoInterfaceProcess"));
    ASSERT_NE(AlgoInterfaceProcess, nullptr)
        << "Failed to load AlgoInterfaceProcess: " << dlerror();

    RegisterCallback =
        reinterpret_cast<Callback>(dlsym(libhandle, "RegisterCallback"));
    ASSERT_NE(RegisterCallback, nullptr)
        << "Failed to load RegisterCallback: " << dlerror();

    // Initialize the library
    int status = InitAlgoInterface(&algoHandle);
    ASSERT_EQ(status, 0) << "InitAlgoInterface failed with status: " << status;

    // Register a callback
    status = RegisterCallback(&algoHandle, AlgoProcessTestResponseCallback);
    ASSERT_EQ(status, 0) << "RegisterCallback failed with status: " << status;
  }

  void TearDown() override {

    // Deinitialize the library
    int status = DeInitAlgoInterface(&algoHandle);
    ASSERT_EQ(status, 0) << "DeInitAlgoInterface failed with status:" << status;
    if (libhandle) {
      dlclose(libhandle);
    }
  }
};

TEST_F(AlgoProcessTest, AlgoNodeProcessTest) {

  for (int id = ALGO_START; id < ALGO_END; id++) {
    // int id = ALGO_OFFSET(ALGO_SWJPEG);
    // std::cerr << "Test Algo::" << algoName[id] << std::endl;
    g_AlgoProcessTestCallback      = 0;
    g_ProcessedFlagAlgoProcessTest = 0;
    ALGOID Algoid                  = static_cast<ALGOID>(ALGO_BASE_ID + id);
    // Create a mock AlgoRequest
    std::vector<unsigned char> yuvData(WIDTH * HEIGHT * 3 /
                                       2);  // YUV420 format
    auto request        = std::make_shared<AlgoRequest>();
    request->mRequestId = 123;
    int rc              = request->AddImage(ImageFormat::YUV420, WIDTH, HEIGHT,
                                            std::move(yuvData));
    ASSERT_EQ(rc, 0);
    // Clear Process flags
    rc = request->mMetadata.SetMetadata(ALGO_PROCESS_DONE, 0x00);
    ASSERT_EQ(rc, 0);

    // Process the request
    int status = AlgoInterfaceProcess(&algoHandle, request, {Algoid});
    ASSERT_EQ(status, 0) << "AlgoInterfaceProcess failed with status:"
                         << status;

    while (g_AlgoProcessTestCallback == 0) {
      usleep(50);
    }

    // Check Processed algos
    int ExpectedFlag = ALGO_MASK(Algoid);
    ASSERT_EQ(g_ProcessedFlagAlgoProcessTest, ExpectedFlag);
  }
}

TEST_F(AlgoProcessTest, TestComboNodes) {
  std::vector<std::vector<AlgoId>> AlgoCobo = {
      {ALGO_HDR, ALGO_BOKEH},
      {ALGO_HDR, ALGO_WATERMARK, ALGO_SWJPEG},
      {ALGO_BOKEH, ALGO_SWJPEG},
      {ALGO_HDR, ALGO_SWJPEG},
      {ALGO_WATERMARK, ALGO_SWJPEG},
      {ALGO_HDR, ALGO_BOKEH, ALGO_WATERMARK},
      {ALGO_HDR, ALGO_BOKEH, ALGO_SWJPEG},
      {ALGO_HDR, ALGO_WATERMARK},
      {ALGO_BOKEH, ALGO_WATERMARK},
      {ALGO_HDR, ALGO_BOKEH, ALGO_WATERMARK, ALGO_SWJPEG},
  };
  for (int id = 0; id < (int)AlgoCobo.size(); id++) {

    g_AlgoProcessTestCallback      = 0;
    g_ProcessedFlagAlgoProcessTest = 0;
    // Create a mock AlgoRequest
    std::vector<unsigned char> yuvData(WIDTH * HEIGHT * 3 /
                                       2);  // YUV420 format
    auto request        = std::make_shared<AlgoRequest>();
    request->mRequestId = 123;
    int rc              = request->AddImage(ImageFormat::YUV420, WIDTH, HEIGHT,
                                            std::move(yuvData));
    ASSERT_EQ(rc, 0);
    // Clear Process flags
    rc = request->mMetadata.SetMetadata(ALGO_PROCESS_DONE, 0x00);
    ASSERT_EQ(rc, 0);

    // Process the request
    int status = AlgoInterfaceProcess(&algoHandle, request, AlgoCobo[id]);
    ASSERT_EQ(status, 0) << "AlgoInterfaceProcess failed with status:"
                         << status;

    while (g_AlgoProcessTestCallback == 0) {
      usleep(50);
    }

    // Check Processed algos
    int ExpectedFlag = 0x00;
    for (auto id : AlgoCobo[id]) {
      ExpectedFlag |= ALGO_MASK(id);
    }
    ASSERT_EQ(g_ProcessedFlagAlgoProcessTest, ExpectedFlag);
  }
}