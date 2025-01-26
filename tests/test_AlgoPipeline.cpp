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
#include "AlgoPipeline.h" // Include the header file for your class
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <mutex>
#define STRESS_CNT 10000

// Test fixture
class AlgoPipelineTest : public ::testing::Test {
protected:
};

TEST_F(AlgoPipelineTest, CtorDtorID) {
  std::vector<AlgoId> algoList = {ALGO_HDR, ALGO_BOKEH};
  auto algoPipeline = std::make_shared<AlgoPipeline>();
  EXPECT_EQ(algoPipeline->GetState(), AlgoPipelineState::Initialised);
  EXPECT_NE(algoPipeline, nullptr);
  EXPECT_EQ(algoList.size(), 2);
  algoPipeline->ConfigureAlgoPipeline(algoList);
  EXPECT_EQ(algoPipeline->GetState(), AlgoPipelineState::ConfiguredWithId);
}

TEST_F(AlgoPipelineTest, CtorDtorName) {
  std::vector<std::string> algoList = {"HDRAlgorithm", "BokehAlgorithm"};
  auto algoPipeline = std::make_shared<AlgoPipeline>();
  EXPECT_NE(algoPipeline, nullptr);
  EXPECT_EQ(algoPipeline->GetState(), AlgoPipelineState::Initialised);
  EXPECT_EQ(algoList.size(), 2);
  algoPipeline->ConfigureAlgoPipeline(algoList);
  EXPECT_EQ(algoPipeline->GetState(), AlgoPipelineState::ConfiguredWithName);
}

int ProcessedFrame = 0;
TEST_F(AlgoPipelineTest, Processtest) {

  std::vector<AlgoId> algoList = {ALGO_HDR, ALGO_BOKEH};

  auto pipelineCallback = [](void *ctx, std::shared_ptr<AlgoRequest> input) {
    static std::mutex cbmux;
    std::lock_guard<std::mutex> lock(cbmux);
    ProcessedFrame++;
  };
  ProcessedFrame = 0;
  auto algoPipeline = std::make_shared<AlgoPipeline>(pipelineCallback);
  EXPECT_NE(algoPipeline, nullptr);
  EXPECT_EQ(algoPipeline->GetState(), AlgoPipelineState::Initialised);
  algoPipeline->ConfigureAlgoPipeline(algoList);
  EXPECT_EQ(algoPipeline->GetState(), AlgoPipelineState::ConfiguredWithId);

  for (int i = 0; i < STRESS_CNT; i++) {
    std::shared_ptr<AlgoRequest> input = std::make_shared<AlgoRequest>();
    input->mRequestId = i;
    algoPipeline->Process(input);
  }

  algoPipeline->WaitForQueueCompetion();
  EXPECT_EQ(algoPipeline->GetProcessedFrames(), STRESS_CNT);
  EXPECT_EQ(ProcessedFrame, STRESS_CNT);
}

TEST_F(AlgoPipelineTest, ProcesstestFail) {
  std::vector<AlgoId> algoList = {ALGO_MAX};
  auto pipelineCallback = [](void *ctx, std::shared_ptr<AlgoRequest> input) {
    ProcessedFrame++;
  };
  ProcessedFrame = 0;
  auto algoPipeline = std::make_shared<AlgoPipeline>(pipelineCallback);
  EXPECT_NE(algoPipeline, nullptr);
  EXPECT_EQ(algoPipeline->GetState(), AlgoPipelineState::Initialised);
  algoPipeline->ConfigureAlgoPipeline(algoList);
  EXPECT_EQ(algoPipeline->GetState(), AlgoPipelineState::FailedToConfigure);

  std::shared_ptr<AlgoRequest> input = std::make_shared<AlgoRequest>();
  input->mRequestId = 0x100;
  algoPipeline->Process(input);

  algoPipeline->WaitForQueueCompetion();
  EXPECT_EQ(algoPipeline->GetProcessedFrames(), 0);
  EXPECT_EQ(ProcessedFrame, 0);
}
