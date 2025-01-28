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
#include "../include/AlgoRequest.h"
#include <dlfcn.h>
#include <gtest/gtest.h>
#include <memory>

// Type aliases for function pointers
typedef int (*InitAlgoInterfaceFunc)(void **);
typedef int (*DeInitAlgoInterfaceFunc)(void **);
typedef int (*AlgoInterfaceProcessFunc)(void **, std::shared_ptr<AlgoRequest>);
typedef int (*RegisterCallbackFunc)(void **,
                                    int (*)(std::shared_ptr<AlgoRequest>));

// Test Fixture Class
class SharedLibTest : public ::testing::Test {
protected:
  void *libhandle = nullptr;

  // Function pointers
  InitAlgoInterfaceFunc InitAlgoInterface = nullptr;
  DeInitAlgoInterfaceFunc DeInitAlgoInterface = nullptr;
  AlgoInterfaceProcessFunc AlgoInterfaceProcess = nullptr;
  RegisterCallbackFunc RegisterCallback = nullptr;

  void SetUp() override {
    // Load the shared library
    libhandle =
        dlopen("/home/uma/workspace/Gzero/cmake/lib/libAlgoLib.so", RTLD_LAZY);
    ASSERT_NE(libhandle, nullptr)
        << "Failed to load shared library: " << dlerror();

    // Load the functions
    InitAlgoInterface = reinterpret_cast<InitAlgoInterfaceFunc>(
        dlsym(libhandle, "InitAlgoInterface"));
    ASSERT_NE(InitAlgoInterface, nullptr)
        << "Failed to load InitAlgoInterface: " << dlerror();

    DeInitAlgoInterface = reinterpret_cast<DeInitAlgoInterfaceFunc>(
        dlsym(libhandle, "DeInitAlgoInterface"));
    ASSERT_NE(DeInitAlgoInterface, nullptr)
        << "Failed to load DeInitAlgoInterface: " << dlerror();

    AlgoInterfaceProcess = reinterpret_cast<AlgoInterfaceProcessFunc>(
        dlsym(libhandle, "AlgoInterfaceProcess"));
    ASSERT_NE(AlgoInterfaceProcess, nullptr)
        << "Failed to load AlgoInterfaceProcess: " << dlerror();

    RegisterCallback = reinterpret_cast<RegisterCallbackFunc>(
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
int g_cbs = 0;
int TestCallback(std::shared_ptr<AlgoRequest> input) {
  // Simulate callback logic
  g_cbs++;
  return 0; // Return success
}

// Test Cases

TEST_F(SharedLibTest, InitDeInitTest) {
  void *algoHandle = nullptr;

  // Test initialization
  int status = InitAlgoInterface(&algoHandle);
  ASSERT_EQ(status, 0) << "InitAlgoInterface failed with status: " << status;
  ASSERT_NE(algoHandle, nullptr) << "algoHandle is null after initialization";

  // Test deinitialization
  status = DeInitAlgoInterface(&algoHandle);
  ASSERT_EQ(status, 0) << "DeInitAlgoInterface failed with status: " << status;
  ASSERT_EQ(algoHandle, nullptr)
      << "algoHandle not set to null after deinitialization";
}

TEST_F(SharedLibTest, RegisterCallbackTest) {
  void *algoHandle = nullptr;

  // Initialize the library
  int status = InitAlgoInterface(&algoHandle);
  ASSERT_EQ(status, 0) << "InitAlgoInterface failed with status: " << status;

  // Register a callback
  status = RegisterCallback(&algoHandle, TestCallback);
  ASSERT_EQ(status, 0) << "RegisterCallback failed with status: " << status;

  // Deinitialize the library
  status = DeInitAlgoInterface(&algoHandle);
  ASSERT_EQ(status, 0) << "DeInitAlgoInterface failed with status: " << status;
}

#define WIDTH 640
#define HEIGHT 480

int cb_cnt = 0;
int ImageCallback(std::shared_ptr<AlgoRequest> input) {
  // Simulate callback logic
  std::cout << "ImageCallback" << std::endl;
  cb_cnt++;
  return 0; // Return success
}
TEST_F(SharedLibTest, AlgoInterfaceProcessTest) {
  void *algoHandle = nullptr;

  // Initialize the library
  int status = InitAlgoInterface(&algoHandle);
  ASSERT_EQ(status, 0) << "InitAlgoInterface failed with status: " << status;

  // Register a callback
  status = RegisterCallback(&algoHandle, ImageCallback);
  ASSERT_EQ(status, 0) << "RegisterCallback failed with status: " << status;

  cb_cnt = 0;
  // Create a mock AlgoRequest
  std::vector<unsigned char> yuvData(WIDTH * HEIGHT * 3 / 2); // YUV420 format
  auto request = std::make_shared<AlgoRequest>();
  request->mRequestId = 123;
  request->AddImage(ImageFormat::YUV420, WIDTH, HEIGHT, yuvData);

  // Process the request
  status = AlgoInterfaceProcess(&algoHandle, request);
  ASSERT_EQ(status, 0) << "AlgoInterfaceProcess failed with status: " << status;

  while (cb_cnt == 0) {
    usleep(50);
  }
  // Deinitialize the library
  status = DeInitAlgoInterface(&algoHandle);

  ASSERT_EQ(status, 0) << "DeInitAlgoInterface failed with status: " << status;
}
