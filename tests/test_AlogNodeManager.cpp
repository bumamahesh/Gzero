
#include "AlgoNodeManager.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>

// Test fixture
class AlgoNodeManagerTest : public ::testing::Test {
protected:
};

TEST_F(AlgoNodeManagerTest, AlgoNodeManagerApi) {

  try {
    std::string HdralgoName(HDR_NAME);
    auto algoNodeManager = AlgoNodeManager::Getinstance();

    // Check if the algo is loaded
    EXPECT_EQ(algoNodeManager.GetLoadedAlgosSize(), 2);

    // Check algo available by Id
    EXPECT_EQ(algoNodeManager.IsAlgoAvailable(ALGO_HDR), true);

    // Check algo available by name
    EXPECT_EQ(algoNodeManager.IsAlgoAvailable(HdralgoName), true);

  } catch (const std::exception &e) {
    FAIL() << "Exception thrown: " << e.what();
  }
}

TEST_F(AlgoNodeManagerTest, GetAlgoObjectByName) {
  std::string HdralgoName(HDR_NAME);
  try {
    auto algoNodeManager = AlgoNodeManager::Getinstance();

    // Check if the algo is loaded
    EXPECT_EQ(algoNodeManager.GetLoadedAlgosSize(), 2);

    // Check algo available by name
    EXPECT_EQ(algoNodeManager.IsAlgoAvailable(HdralgoName), true);

    // Create an algo object by name
    auto algo = algoNodeManager.CreateAlgo(HdralgoName);

    // Check if the algo object is created
    EXPECT_NE(algo, nullptr);

    // Check if the algo object is created with the correct algoName
    EXPECT_EQ(algo->GetAlgorithmName(), HdralgoName);

  } catch (const std::exception &e) {
    FAIL() << "Exception thrown: " << e.what();
  }
}

TEST_F(AlgoNodeManagerTest, GetAlgoObjectById) {

  try {

    auto algoNodeManager = AlgoNodeManager::Getinstance();

    // Check if the algo is loaded
    EXPECT_EQ(algoNodeManager.GetLoadedAlgosSize(), 2);

    // Check algo available by Id
    EXPECT_EQ(algoNodeManager.IsAlgoAvailable(ALGO_HDR), true);

    // Create an algo object
    auto algo = algoNodeManager.CreateAlgo(ALGO_HDR);

    // Check if the algo object is created
    EXPECT_NE(algo, nullptr);

    // Check if the algo object is created with the correct algoId
    EXPECT_EQ(algo->GetAlgoId(), ALGO_HDR);

  } catch (const std::exception &e) {
    FAIL() << "Exception thrown: " << e.what();
  }
}