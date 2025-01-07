#include "AlgoBase.h"
#include "EventHandlerThread.h"
#include <gtest/gtest.h>
#include <string>

/**
 * @class MockDerivedAlgo
 * @brief Mock implementation of AlgoBase for testing purposes.
 */
class MockDerivedAlgo : public AlgoBase {
public:
  /**
   * @brief Constructor for MockDerivedAlgo.
   * @param name Name of the algorithm.
   */
  explicit MockDerivedAlgo(const char *name) : AlgoBase(name) {}

  /**
   * @brief Destructor for MockDerivedAlgo.
   */
  ~MockDerivedAlgo() override = default;

  /**
   * @brief Open the algorithm, simulating resource availability checks.
   * @return Status of the operation.
   */
  AlgoStatus Open() override { return GetAlgoStatus(); }

  /**
   * @brief Process the algorithm, simulating input validation.
   * @return Status of the operation.
   */
  AlgoStatus Process() override {
    // usleep(5 * 1000); // Simulate processing time 20ms
    SetStatus(AlgoStatus::SUCCESS);
    return GetAlgoStatus();
  }

  /**
   * @brief Close the algorithm, simulating cleanup operations.
   * @return Status of the operation.
   */
  AlgoStatus Close() override {

    SetStatus(AlgoStatus::SUCCESS);
    return GetAlgoStatus();
  }

private:
};

/**
 * @brief
 *
 */
TEST(AlgoBaseTest, AlgorithmNameRetrieval) {
  MockDerivedAlgo node("TestAlgorithm");

  // Test the algorithm name
  EXPECT_EQ(node.GetAlgorithmName(), "TestAlgorithm");

  // Test another node with a different name
  MockDerivedAlgo another_node("AnotherAlgorithm");

  // Test the algorithm name
  EXPECT_EQ(another_node.GetAlgorithmName(), "AnotherAlgorithm");
}

/**
 * @brief Test case for verifying algorithm status retrieval.
 */
int g_callbacks = 0;
TEST(AlgoBaseTest, CallBackTest) {

  std::shared_ptr<MockDerivedAlgo> node =
      std::make_shared<MockDerivedAlgo>("TestAlgorithm");

  EXPECT_NE(node, nullptr);
  EXPECT_EQ(node->GetAlgorithmName(), "TestAlgorithm");
  g_callbacks = 0;

  auto callback = [](void *ctx,
                     std::shared_ptr<AlgoBase::ALGOCALLBACKMSG> msg) {
    assert(msg != nullptr);
    switch (msg->mType) {
    case AlgoBase::ALGO_PROCESSING_COMPLETED:
      g_callbacks++;
      break;
    default:
      assert(true);
      break;
    }
  };
  auto eventHandler =
      std::make_shared<EventHandlerThread<AlgoBase::ALGOCALLBACKMSG>>(callback,
                                                                      nullptr);

  EXPECT_NE(eventHandler, nullptr);

  node->SetEventThread(eventHandler);

  for (int i = 0; i < 500; i++) {
    auto task = std::make_shared<Task_t>();
    std::shared_ptr<AlgoRequest> algoRequest = std::make_shared<AlgoRequest>();
    task->request = algoRequest;
    node->EnqueueRequest(task);
  }
  node->WaitForQueueCompetion();
  EXPECT_EQ(g_callbacks, 500);
}

/**
 * @class MockDerivedAlgoFail
 * @brief Mock implementation of AlgoBase for testing purposes.
 */
class MockDerivedAlgoFail : public AlgoBase {
public:
  /**
   * @brief Constructor for MockDerivedAlgo.
   * @param name Name of the algorithm.
   */
  explicit MockDerivedAlgoFail(const char *name) : AlgoBase(name) {}

  /**
   * @brief Destructor for MockDerivedAlgoFail.
   */
  ~MockDerivedAlgoFail() override = default;

  /**
   * @brief Open the algorithm, simulating resource availability checks.
   * @return Status of the operation.
   */
  AlgoStatus Open() override {

    SetStatus(AlgoStatus::SUCCESS);
    return GetAlgoStatus();
  }

  /**
   * @brief Process the algorithm, simulating input validation.
   * @return Status of the operation.
   */
  AlgoStatus Process() override {

    // usleep(5 * 1000); // Simulate processing time 20ms
    SetStatus(AlgoStatus::INTERNAL_ERROR);
    return GetAlgoStatus();
  }

  /**
   * @brief Close the algorithm, simulating cleanup operations.
   * @return Status of the operation.
   */
  AlgoStatus Close() override {

    SetStatus(AlgoStatus::SUCCESS);
    return GetAlgoStatus();
  }

private:
};

size_t g_Failcallbacks = 0;
TEST(AlgoBaseTest, CallBackTestFail) {
  std::shared_ptr<MockDerivedAlgoFail> node =
      std::make_shared<MockDerivedAlgoFail>("TestAlgorithmFail");

  EXPECT_EQ(node->GetAlgorithmName(), "TestAlgorithmFail");
  g_Failcallbacks = 0;

  auto callback = [](void *ctx,
                     std::shared_ptr<AlgoBase::ALGOCALLBACKMSG> msg) {
    assert(msg != nullptr);
    switch (msg->mType) {
    case AlgoBase::ALGO_PROCESSING_FAILED:
      g_Failcallbacks++;
      break;
    default:
      assert(true);
      break;
    }
  };
  auto eventHandler =
      std::make_shared<EventHandlerThread<AlgoBase::ALGOCALLBACKMSG>>(callback,
                                                                      nullptr);
  node->SetEventThread(eventHandler);

  for (int i = 0; i < 500; i++) {
    auto task = std::make_shared<Task_t>();
    task->request = nullptr;
    node->EnqueueRequest(task);
  }
  node->WaitForQueueCompetion();
  EXPECT_EQ(g_Failcallbacks, 500);
}

/**
 * @brief Main function to execute all tests.
 * @param argc Argument count.
 * @param argv Argument vector.
 * @return Test execution result.
 */
int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
