#include "AlgoPipeline.h" // Include the header file for your class
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#define STRESS_CNT 10000

// Test fixture
class AlgoPipelineTest : public ::testing::Test {
protected:
};

TEST_F(AlgoPipelineTest, CtorDtorID) {
  std::vector<AlgoId> algoList = {ALGO_HDR, ALGO_BOKEH};
  auto algoPipeline = std::make_shared<AlgoPipeline>();
  EXPECT_EQ(algoPipeline->GetState(), ALGOPIPELINESTATE::INITIALISED);
  EXPECT_NE(algoPipeline, nullptr);
  EXPECT_EQ(algoList.size(), 2);
  algoPipeline->ConfigureAlgoPipeline(algoList);
  EXPECT_EQ(algoPipeline->GetState(), ALGOPIPELINESTATE::CONFIGURED_WITH_ID);
}

TEST_F(AlgoPipelineTest, CtorDtorName) {
  std::vector<std::string> algoList = {HDR_NAME, BOKEH_NAME};
  auto algoPipeline = std::make_shared<AlgoPipeline>();
  EXPECT_NE(algoPipeline, nullptr);
  EXPECT_EQ(algoPipeline->GetState(), ALGOPIPELINESTATE::INITIALISED);
  EXPECT_EQ(algoList.size(), 2);
  algoPipeline->ConfigureAlgoPipeline(algoList);
  EXPECT_EQ(algoPipeline->GetState(), ALGOPIPELINESTATE::CONFIGURED_WITH_NAME);
}

TEST_F(AlgoPipelineTest, Processtest) {

  std::vector<AlgoId> algoList = {ALGO_HDR, ALGO_BOKEH};
  auto algoPipeline = std::make_shared<AlgoPipeline>();
  EXPECT_NE(algoPipeline, nullptr);
  EXPECT_EQ(algoPipeline->GetState(), ALGOPIPELINESTATE::INITIALISED);
  algoPipeline->ConfigureAlgoPipeline(algoList);
  EXPECT_EQ(algoPipeline->GetState(), ALGOPIPELINESTATE::CONFIGURED_WITH_ID);

  for (int i = 0; i < STRESS_CNT; i++) {
    std::string input = "Test";
    algoPipeline->Process(input);
  }

  algoPipeline->WaitForQueueCompetion();
  EXPECT_EQ(algoPipeline->GetProcessedFrames(), STRESS_CNT);
}

TEST_F(AlgoPipelineTest, ProcesstestFail) {
  std::vector<AlgoId> algoList = {ALGO_MAX};
  auto algoPipeline = std::make_shared<AlgoPipeline>();
  EXPECT_NE(algoPipeline, nullptr);
  EXPECT_EQ(algoPipeline->GetState(), ALGOPIPELINESTATE::INITIALISED);
  algoPipeline->ConfigureAlgoPipeline(algoList);
  EXPECT_EQ(algoPipeline->GetState(), ALGOPIPELINESTATE::FAILED_TO_CONFIGURE);

  std::string input = "Test";
  algoPipeline->Process(input);

  algoPipeline->WaitForQueueCompetion();
  EXPECT_EQ(algoPipeline->GetProcessedFrames(), 0);
}
