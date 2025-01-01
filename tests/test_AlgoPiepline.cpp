#include "AlgoPipeline.h" // Include the header file for your class
#include <gmock/gmock.h>
#include <gtest/gtest.h>

// Test fixture
class AlgoPipelineTest : public ::testing::Test {
protected:
};

TEST_F(AlgoPipelineTest, ctorDtor) {
  std::vector<size_t> algoList = {0XCAFEBABE};
  auto algoPipeline = std::make_shared<AlgoPipeline>(algoList);

  // Check if the algoPipeline is created
  EXPECT_NE(algoPipeline, nullptr);
}

TEST_F(AlgoPipelineTest, Processtest) {
  std::vector<size_t> algoList = {0XCAFEBABE};
  auto algoPipeline = std::make_shared<AlgoPipeline>(algoList);

  // Check if the algoPipeline is created
  EXPECT_NE(algoPipeline, nullptr);

  std::string input = "Test";
  algoPipeline->Process(input);
  usleep(300 * 1000); // 3s wait
}
