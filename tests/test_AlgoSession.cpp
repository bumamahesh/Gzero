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
#include "../include/AlgoSession.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>

int TotalCallbacks = 0;

auto callback = [](void *ctx, std::shared_ptr<AlgoRequest> input) {
  TotalCallbacks++;
};

TEST(AlgoSessionTest, AlgoSessionApi) {
  try {
    std::string config = "config";
    auto algoSession = std::make_shared<AlgoSession>(callback, nullptr);
    EXPECT_NE(algoSession, nullptr);
    EXPECT_EQ(algoSession->SessionGetPipelineCount(), 0);
    std::shared_ptr<AlgoPipeline> pipeline = std::make_shared<AlgoPipeline>();
    std::vector<AlgoId> algoList = {ALGO_HDR, ALGO_BOKEH};
    pipeline->ConfigureAlgoPipeline(algoList);
    EXPECT_EQ(algoSession->SessionAddPipeline(pipeline), true);
    EXPECT_EQ(algoSession->SessionGetPipelineCount(), 1);
    std::shared_ptr<AlgoRequest> input = std::make_shared<AlgoRequest>();
    input->mRequestId = 0x100;
    EXPECT_EQ(algoSession->SessionProcess(0, input), true);
    EXPECT_EQ(algoSession->SessionStop(), true);
    EXPECT_EQ(algoSession->SessionGetPipelineCount(), 0);
  } catch (const std::exception &e) {
    FAIL() << "Exception thrown: " << e.what();
  }
}

#define SESSION_STRESS_CNT 25

TEST(AlgoSessionTest, ProcessTest) {

  {
    TotalCallbacks = 0;
    auto algoSession = std::make_shared<AlgoSession>(callback, nullptr);
    EXPECT_NE(algoSession, nullptr);
    EXPECT_EQ(algoSession->SessionGetPipelineCount(), 0);
    std::vector<AlgoId> algoList = {ALGO_HDR, ALGO_BOKEH};
    for (int i = 0; i < SESSION_STRESS_CNT; i++) {
      std::shared_ptr<AlgoRequest> input = std::make_shared<AlgoRequest>();
      input->mRequestId = i;
      EXPECT_EQ(algoSession->SessionProcess(input, algoList), true);
    }
    EXPECT_EQ(algoSession->SessionGetPipelineCount(), 1);
  }
  EXPECT_EQ(TotalCallbacks, SESSION_STRESS_CNT);
}
