#include "AlgoSession.h"
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
    EXPECT_EQ(algoSession->SessionProcess(0, input), true);
    EXPECT_EQ(algoSession->SessionStop(), true);
    EXPECT_EQ(algoSession->SessionGetPipelineCount(), 0);
  } catch (const std::exception &e) {
    FAIL() << "Exception thrown: " << e.what();
  }
}

TEST(AlgoSessionTest, ProcessTest) {

  {
    TotalCallbacks = 0;
    auto algoSession = std::make_shared<AlgoSession>(callback, nullptr);
    EXPECT_NE(algoSession, nullptr);
    EXPECT_EQ(algoSession->SessionGetPipelineCount(), 0);

    for (int i = 0; i < 100; i++) {
      std::shared_ptr<AlgoRequest> input = std::make_shared<AlgoRequest>();
      EXPECT_EQ(algoSession->SessionProcess(input), true);
    }
    EXPECT_EQ(algoSession->SessionGetPipelineCount(), 3);
  }
  EXPECT_EQ(TotalCallbacks, 100);
}
