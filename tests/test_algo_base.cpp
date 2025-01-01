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
  explicit MockDerivedAlgo(const std::string &name) : AlgoBase(name) {}

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
    usleep(1 * 1000); // Simulate processing time 20ms
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
 * @brief Test case for verifying algorithm name retrieval.
 */
TEST(AlgoBaseNameTest, AlgorithmNameRetrieval) {
  MockDerivedAlgo node("TestAlgorithm");

  // Test the algorithm name
  EXPECT_EQ(node.GetAlgorithmName(), "TestAlgorithm");

  // Test another node with a different name
  // MockDerivedAlgo another_node("AnotherAlgorithm");
  // EXPECT_EQ(another_node.GetAlgorithmName(), "AnotherAlgorithm");
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
