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
#include "../include/AlgoLibraryLoader.h" // Include the header file for your class
#include "EventHandlerThread.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>

// Test fixture
class AlgoLibraryLoaderTest : public ::testing::Test {
protected:
  // Path to a mock shared library (this would be a path to a real or test
  // library)
  const std::string validLibraryPath =
      "/home/uma/workspace/Gzero/cmake/lib/com.Algo.Hdr.so";
};

// Test case for loading a valid shared library
TEST_F(AlgoLibraryLoaderTest, LoadValidLibrary) {
  // Assuming we have a valid test library
  try {
    auto loader = std::make_shared<AlgoLibraryLoader>(validLibraryPath);
    ASSERT_NE(loader, nullptr);
    ASSERT_EQ(loader->GetAlgoId(), ALGO_HDR);
    ASSERT_EQ(loader->GetAlgorithmName(), std::string("HDRAlgorithm"));

    auto algo = loader->GetAlgoMethod();

    ASSERT_NE(algo, nullptr);

    ASSERT_EQ(algo->GetAlgoId(), loader->GetAlgoId());
    ASSERT_EQ(algo->GetAlgorithmName(), loader->GetAlgorithmName());
    // algo->WaitForQueueCompetion();
    algo = nullptr;
    loader = nullptr;
  } catch (const std::exception &e) {
    FAIL() << "Exception thrown: " << e.what();
  }
}

// Test case for successfully retrieving the algorithm method
TEST_F(AlgoLibraryLoaderTest, RetrieveAlgoMethod) {
  // Here, we test the interaction with the returned AlgoBase method.
  AlgoLibraryLoader loader(validLibraryPath);
  ASSERT_EQ(loader.GetAlgoId(), ALGO_HDR);
  ASSERT_EQ(loader.GetAlgorithmName(), std::string("HDRAlgorithm"));

  std::shared_ptr<AlgoBase> algo = loader.GetAlgoMethod();
  ASSERT_NE(algo, nullptr);

  auto callback = [](void *ctx,
                     std::shared_ptr<AlgoBase::AlgoCallbackMessage> msg) {
    assert(msg != nullptr);
    switch (msg->mType) {
    case AlgoBase::AlgoMessageType::ProcessingCompleted:
      break;
    default:
      assert(true);
      break;
    }
  };
  auto eventHandler =
      std::make_shared<EventHandlerThread<AlgoBase::AlgoCallbackMessage>>(
          callback, nullptr);

  algo->SetEventThread(eventHandler);
  /**/

  // Ensure the algorithm method is valid
  ASSERT_NE(algo, nullptr);
  ASSERT_EQ(algo->GetAlgoId(), loader.GetAlgoId());
  ASSERT_EQ(algo->GetAlgorithmName(), loader.GetAlgorithmName());

  // Call the Open method
  ASSERT_NE(algo->GetAlgorithmName(), "");
  ASSERT_EQ(algo->GetAlgorithmName(), "HDRAlgorithm");
  ASSERT_EQ(algo->Open(), AlgoBase::AlgoStatus::SUCCESS);
  ASSERT_EQ(algo->GetAlgoStatus(), AlgoBase::AlgoStatus::SUCCESS);

  int i = 0;
  while (i < 500) {
    std::string msg = std::string("GSD");
    auto task = std::make_shared<Task_t>();
    task->request = nullptr; // make a request here  @todo
    algo->EnqueueRequest(task);
    i++;
  }

  algo->WaitForQueueCompetion();
  ASSERT_EQ(algo->GetAlgoStatus(), AlgoBase::AlgoStatus::SUCCESS);
  ASSERT_EQ(algo->Close(), AlgoBase::AlgoStatus::SUCCESS);
  ASSERT_EQ(algo->GetAlgoStatus(), AlgoBase::AlgoStatus::SUCCESS);
}
