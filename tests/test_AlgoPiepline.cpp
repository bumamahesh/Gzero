#include "AlgoPipeline.h" // Include the header file for your class
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#define STRESS_CNT 10000

// Test fixture
class AlgoPipelineTest : public ::testing::Test {
protected:
};

TEST_F(AlgoPipelineTest, CtorDtor) {
  std::vector<size_t> algoList = {0XCAFEBABE, 0XCAFEBABE + 1};
  auto algoPipeline = std::make_shared<AlgoPipeline>(algoList);

  // Check if the algoPipeline is created
  EXPECT_NE(algoPipeline, nullptr);
}

TEST_F(AlgoPipelineTest, Processtest) {
  std::vector<size_t> algoList = {0XCAFEBABE, 0XCAFEBABE + 1};
  auto algoPipeline = std::make_shared<AlgoPipeline>(algoList);

  // Check if the algoPipeline is created
  EXPECT_NE(algoPipeline, nullptr);

  for (int i = 0; i < STRESS_CNT; i++) {
    std::string input = "Test";
    algoPipeline->Process(input);
  }

  algoPipeline->WaitForQueueCompetion();
  EXPECT_EQ(algoPipeline->GetProcessedFrames(), STRESS_CNT);
}
