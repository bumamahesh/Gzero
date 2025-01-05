#include "AlgoSession.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>

class AlgoSessionMock : public AlgoSession {

  std::vector<AlgoId> GetAlgoList() override {
    static int count = 0;
    std::vector<AlgoId> algoList;
    if (count % 3 == 0) {
      algoList.push_back(ALGO_HDR);
      algoList.push_back(ALGO_BOKEH);
    } else if (count % 3 == 1) {
      algoList.push_back(ALGO_HDR);
    } else {
      algoList.push_back(ALGO_BOKEH);
    }
    count++;
    return algoList;
  }
};

TEST(AlgoSessionTest, AlgoSessionApi) {
  try {
    std::string config = "config";
    auto algoSession = std::make_shared<AlgoSessionMock>();
    EXPECT_NE(algoSession, nullptr);
    EXPECT_EQ(algoSession->Initialize(config), true);
    EXPECT_EQ(algoSession->GetPipelineCount(), 0);
    std::shared_ptr<AlgoPipeline> pipeline = std::make_shared<AlgoPipeline>();
    std::vector<AlgoId> algoList = {ALGO_HDR, ALGO_BOKEH};
    pipeline->ConfigureAlgoPipeline(algoList);
    EXPECT_EQ(algoSession->AddPipeline(pipeline), true);
    EXPECT_EQ(algoSession->GetPipelineCount(), 1);
    std::string input = "test";
    EXPECT_EQ(algoSession->Process(0, input), true);
    EXPECT_EQ(algoSession->Stop(), true);
    EXPECT_EQ(algoSession->GetPipelineCount(), 0);
  } catch (const std::exception &e) {
    FAIL() << "Exception thrown: " << e.what();
  }
}

TEST(AlgoSessionTest, ProcessTest) {

  auto algoSession = std::make_shared<AlgoSessionMock>();
  EXPECT_NE(algoSession, nullptr);
  EXPECT_EQ(algoSession->GetPipelineCount(), 0);

  for (int i = 0; i < 3; i++) {
    std::string input = "test";
    EXPECT_EQ(algoSession->Process(input), true);
  }
  EXPECT_EQ(algoSession->GetPipelineCount(), 3);
}