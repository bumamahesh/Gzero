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
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include "../include/AlgoNodeManager.h"
#include "ImageUtils.h"
// Test fixture

constexpr size_t NumAlgos = (int)(ALGO_END);
class AlgoNodeManagerTest : public ::testing::Test {
 protected:
};
TEST_F(AlgoNodeManagerTest, CtorDtor) {

  auto algoNodeManager = &AlgoNodeManager ::Getinstance();
  EXPECT_EQ(algoNodeManager->GetLoadedAlgosSize(), NumAlgos);
}

TEST_F(AlgoNodeManagerTest, AlgoNodeManagerApi) {

  try {
    std::string HdralgoName("HdrAlgorithm");
    auto algoNodeManager = &AlgoNodeManager::Getinstance();

    // Check if the algo is loaded
    EXPECT_EQ(algoNodeManager->GetLoadedAlgosSize(), NumAlgos);

    // Check algo available by Id
    EXPECT_EQ(algoNodeManager->IsAlgoAvailable(ALGO_HDR), true);

    // Check algo available by name
    EXPECT_EQ(algoNodeManager->IsAlgoAvailable(HdralgoName), true);

  } catch (const std::exception& e) {
    FAIL() << "Exception thrown: " << e.what();
  }
}

TEST_F(AlgoNodeManagerTest, GetAlgoObjectByName) {
  std::string HdralgoName("HdrAlgorithm");
  try {
    auto algoNodeManager = &AlgoNodeManager::Getinstance();

    // Check if the algo is loaded
    EXPECT_EQ(algoNodeManager->GetLoadedAlgosSize(), NumAlgos);

    // Check algo available by name
    EXPECT_EQ(algoNodeManager->IsAlgoAvailable(HdralgoName), true);

    // Create an algo object by name
    auto algo = algoNodeManager->CreateAlgo(HdralgoName);

    // Check if the algo object is created
    EXPECT_NE(algo, nullptr);

    // Check if the algo object is created with the correct algoName
    EXPECT_EQ(algo->GetAlgorithmName(), HdralgoName);

  } catch (const std::exception& e) {
    FAIL() << "Exception thrown: " << e.what();
  }
}

TEST_F(AlgoNodeManagerTest, GetAlgoObjectById) {

  try {

    auto algoNodeManager = &AlgoNodeManager::Getinstance();

    // Check if the algo is loaded
    EXPECT_EQ(algoNodeManager->GetLoadedAlgosSize(), NumAlgos);

    // Check algo available by Id
    EXPECT_EQ(algoNodeManager->IsAlgoAvailable(ALGO_HDR), true);

    // Create an algo object
    auto algo = algoNodeManager->CreateAlgo(ALGO_HDR);

    // Check if the algo object is created
    EXPECT_NE(algo, nullptr);

    // Check if the algo object is created with the correct algoId
    EXPECT_EQ(algo->GetAlgoId(), ALGO_HDR);

  } catch (const std::exception& e) {
    FAIL() << "Exception thrown: " << e.what();
  }
}

TEST_F(AlgoNodeManagerTest, TryMandelbrotSetProcess) {

  auto callback = [](void* ctx,
                     std::shared_ptr<AlgoBase::AlgoCallbackMessage> msg) {
    (void)(ctx);
    assert(msg != nullptr);
    switch (msg->mType) {
      case AlgoBase::AlgoMessageType::ProcessingCompleted: {
        auto rawData        = msg->mRequest->request->GetImage(0)->GetData();
        static int i        = 0;
        std::string outfile = "output" + std::to_string(i++) + ".raw";
        SaveRawDataToFile(outfile, rawData);
      } break;
      case AlgoBase::AlgoMessageType::ProcessingFailed:
        break;
      case AlgoBase::AlgoMessageType::ProcessingTimeout:
        break;
      case AlgoBase::AlgoMessageType::ProcessingPartial:
        break;
      case AlgoBase::AlgoMessageType::ProcessDone:
        break;
      default:
        assert(false);
        break;
    }
  };
  // Get algo and process
  auto algoNodeManager = &AlgoNodeManager::Getinstance();
  // Get algo
  auto algo = algoNodeManager->CreateAlgo(ALGO_MANDELBROTSET);
  EXPECT_NE(algo, nullptr);

  auto eventHandler =
      std::make_shared<EventHandlerThread<AlgoBase::AlgoCallbackMessage>>(
          callback, nullptr);
  algo->SetEventThread(eventHandler);

  // Process
  // LOG(ERROR, ALGOTIMER, "Start processing");
  for (int i = 0; i < 1; i++) {
    // Create image buffer and add to task
    int width    = 320;
    int height   = 240;
    auto sizeRGB = width * height * 3;
    std::vector<unsigned char> rawData(sizeRGB);

    EXPECT_EQ(rawData.size(), sizeRGB * sizeof(unsigned char));
    auto image = std::make_shared<AlgoRequest>();
    int rc =
        image->AddImage(ImageFormat::RGB, width, height, std::move(rawData));
    EXPECT_EQ(rc, 0);

    auto request = std::make_shared<Task_t>();
    EXPECT_NE(request, nullptr);
    request->request = image;

    algo->EnqueueRequest(request);
  }
  // LOG(ERROR, ALGOTIMER, "Processing completed");
  algo->WaitForQueueCompetion();
  eventHandler->stop();
}