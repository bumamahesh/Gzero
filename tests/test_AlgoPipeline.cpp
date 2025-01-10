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
  std::vector<std::string> algoList = {HDR_NAME, BOKEH_NAME};
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
