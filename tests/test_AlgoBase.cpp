#include "AlgoBase.h"
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
  AlgoStatus Open() override {
    if (is_open_called_) {
      SetStatus(AlgoStatus::ALREADY_OPEN);
      return GetStatus();
    }
    if (!resources_available_) {
      SetStatus(AlgoStatus::RESOURCE_UNAVAILABLE);
      return GetStatus();
    }
    is_open_called_ = true;
    SetStatus(AlgoStatus::SUCCESS);
    return GetStatus();
  }

  /**
   * @brief Process the algorithm, simulating input validation.
   * @return Status of the operation.
   */
  AlgoStatus Process() override {
    if (!is_open_called_) {
      SetStatus(AlgoStatus::NOT_INITIALIZED);
      return GetStatus();
    }
    if (input_data_invalid_) {
      SetStatus(AlgoStatus::INVALID_INPUT);
      return GetStatus();
    }
    is_process_called_ = true;
    usleep(5 * 1000); // Simulate processing time 20ms
    SetStatus(AlgoStatus::SUCCESS);
    return GetStatus();
  }

  /**
   * @brief Close the algorithm, simulating cleanup operations.
   * @return Status of the operation.
   */
  AlgoStatus Close() override {
    if (!is_open_called_) {
      SetStatus(AlgoStatus::NOT_INITIALIZED);
      return GetStatus();
    }
    if (is_close_called_) {
      SetStatus(AlgoStatus::ALREADY_CLOSED);
      return GetStatus();
    }
    is_close_called_ = true;
    SetStatus(AlgoStatus::SUCCESS);
    return GetStatus();
  }

private:
  bool is_open_called_ = false;    ///< Tracks if Open has been called.
  bool is_process_called_ = false; ///< Tracks if Process has been called.
  bool is_close_called_ = false;   ///< Tracks if Close has been called.

  // Simulated error conditions
  bool resources_available_ = true; ///< Simulates resource availability.
  bool input_data_invalid_ = false; ///< Simulates valid input.
};

/**
 * @brief
 *
 */
TEST(AlgoBaseNameTest, AlgorithmNameRetrieval) {
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
TEST(AlgoBaseNameTest, CallBackTest) {

  MockDerivedAlgo node("TestAlgorithm");

  // Test the algorithm name
  EXPECT_EQ(node.GetAlgorithmName(), "TestAlgorithm");
  g_callbacks = 0;
  node.SetNotifyEvent(
      [](void *ctx, std::shared_ptr<AlgoBase::ALGOCALLBACKMSG> msg) {
        assert(msg != nullptr);
        assert(ctx != nullptr);
        AlgoBase *algo = reinterpret_cast<AlgoBase *>(ctx);
        assert(algo != nullptr);

        switch (msg->type) {
        case AlgoBase::ALGO_PROCESSING_COMPLETED: {
          g_callbacks++;
          std::shared_ptr<AlgoBase> NextAlgo = algo->GetNextAlgo().lock();
          if (NextAlgo) {
            NextAlgo->EnqueueRequest(msg->request);
          } else {
            /*last node so let free obj */
            std::string *input =
                reinterpret_cast<std::string *>(msg->request->args);
            delete input;
          }
        }

        default:
          /*  std::cout << "AlgoPipeline::NodeEventHandler: Unknown MessageType"
                      << std::endl;
          */
          break;
        }
      });

  for (int i = 0; i < 10000; i++) {
    std::string request = std::to_string(i);
    auto task = std::make_shared<Task_t>();
    task->ctx = &node;
    task->args = new std::string(request);
    node.EnqueueRequest(task);
  }
  node.WaitForQueueCompetion();
  EXPECT_EQ(g_callbacks, 10000);
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
