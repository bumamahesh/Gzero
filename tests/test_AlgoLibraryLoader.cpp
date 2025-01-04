#include "../include/AlgoLibraryLoader.h" // Include the header file for your class
#include <gmock/gmock.h>
#include <gtest/gtest.h>

// Test fixture
class AlgoLibraryLoaderTest : public ::testing::Test {
protected:
  // Path to a mock shared library (this would be a path to a real or test
  // library)
  const std::string validLibraryPath =
      "/home/uma/workspace/Gzero/cmake/lib/com.Algo.Hdr.so";
};

// Test case for loading a valid shared library
TEST_F(AlgoLibraryLoaderTest, LoadValidLibrary) {
  // Assuming we have a valid test library
  try {
    auto loader = std::make_shared<AlgoLibraryLoader>(validLibraryPath);
    ASSERT_NE(loader, nullptr);
    ASSERT_EQ(loader->GetAlgoId(), 0XCAFEBABE);
    ASSERT_EQ(loader->GetAlgorithmName(), std::string("HDRAlgorithm"));

    auto algo = loader->GetAlgoMethod();

    ASSERT_NE(algo, nullptr);

    ASSERT_EQ(algo->GetAlgoId(), loader->GetAlgoId());
    ASSERT_EQ(algo->GetAlgorithmName(), loader->GetAlgorithmName());
    // algo->WaitForQueueCompetion();
    algo = nullptr;
    loader = nullptr;
  } catch (const std::exception &e) {
    FAIL() << "Exception thrown: " << e.what();
  }
}

// Test case for successfully retrieving the algorithm method
TEST_F(AlgoLibraryLoaderTest, RetrieveAlgoMethod) {
  // Here, we test the interaction with the returned AlgoBase method.
  AlgoLibraryLoader loader(validLibraryPath);
  ASSERT_EQ(loader.GetAlgoId(), 0XCAFEBABE);
  ASSERT_EQ(loader.GetAlgorithmName(), std::string("HDRAlgorithm"));

  std::shared_ptr<AlgoBase> algo = loader.GetAlgoMethod();
  ASSERT_NE(algo, nullptr);
  algo->SetNotifyEvent(
      [](void *ctx, std::shared_ptr<AlgoBase::ALGOCALLBACKMSG> msg) {
        assert(msg != nullptr);
        assert(ctx != nullptr);

        auto algo = static_cast<AlgoBase *>(ctx);
        assert(algo != nullptr);

        switch (msg->type) {
        case AlgoBase::ALGO_PROCESSING_FAILED:
          break;
        default:
          assert(true);
          break;
        }
      });

  // Ensure the algorithm method is valid
  ASSERT_NE(algo, nullptr);
  ASSERT_EQ(algo->GetAlgoId(), loader.GetAlgoId());
  ASSERT_EQ(algo->GetAlgorithmName(), loader.GetAlgorithmName());

  // Call the Open method
  ASSERT_NE(algo->GetAlgorithmName(), "");
  ASSERT_EQ(algo->GetAlgorithmName(), "HDRAlgorithm");
  ASSERT_EQ(algo->Open(), AlgoBase::AlgoStatus::SUCCESS);
  ASSERT_EQ(algo->GetStatus(), AlgoBase::AlgoStatus::SUCCESS);

  int i = 0;
  while (i < 500) {
    std::string msg = std::string("GSD");
    auto task = std::make_shared<Task_t>();
    task->args = nullptr;
    // std::cout << "RetrieveAlgoMethod ::EnqueueRequest E" << std::endl;
    algo->EnqueueRequest(task);
    // std::cout << "RetrieveAlgoMethod ::EnqueueRequest X" << std::endl;
    i++;
  }
  /*
    algo->WaitForQueueCompetion();
    ASSERT_EQ(algo->GetStatus(), AlgoBase::AlgoStatus::SUCCESS);
    ASSERT_EQ(algo->Close(), AlgoBase::AlgoStatus::SUCCESS);
    ASSERT_EQ(algo->GetStatus(), AlgoBase::AlgoStatus::SUCCESS);
    */
}
