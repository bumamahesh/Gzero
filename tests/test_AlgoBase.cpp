/*
 * Copyright (c) [2025] [Uma Mahesh B]
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#include "../include/AlgoBase.h"
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
  AlgoStatus Process(std::shared_ptr<AlgoRequest> req) override {
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

  /**
   * @brief Get the Timeout object
   *
   * @return int
   */
  int GetTimeout() override { return 1000; }

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
                     std::shared_ptr<AlgoBase::AlgoCallbackMessage> msg) {
    assert(msg != nullptr);
    switch (msg->mType) {
    case AlgoBase::AlgoMessageType::ProcessingCompleted:
      g_callbacks++;
      break;
    default:
      assert(true);
      break;
    }
  };
  auto eventHandler =
      std::make_shared<EventHandlerThread<AlgoBase::AlgoCallbackMessage>>(
          callback, nullptr);

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
  AlgoStatus Process(std::shared_ptr<AlgoRequest> req) override {

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

  /**
   * @brief Get the Timeout object
   *
   * @return int
   */
  int GetTimeout() override { return 1000; }

private:
};

size_t g_Failcallbacks = 0;
TEST(AlgoBaseTest, CallBackTestFail) {
  std::shared_ptr<MockDerivedAlgoFail> node =
      std::make_shared<MockDerivedAlgoFail>("TestAlgorithmFail");

  EXPECT_EQ(node->GetAlgorithmName(), "TestAlgorithmFail");
  g_Failcallbacks = 0;

  auto callback = [](void *ctx,
                     std::shared_ptr<AlgoBase::AlgoCallbackMessage> msg) {
    assert(msg != nullptr);
    switch (msg->mType) {
    case AlgoBase::AlgoMessageType::ProcessingFailed:
      g_Failcallbacks++;
      break;
    default:
      assert(true);
      break;
    }
  };
  auto eventHandler =
      std::make_shared<EventHandlerThread<AlgoBase::AlgoCallbackMessage>>(
          callback, nullptr);
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
 * @class MockDerivedAlgoFail
 * @brief Mock implementation of AlgoBase for testing purposes.
 */
class MockDerivedAlgoTimeOutCallback : public AlgoBase {
public:
  /**
   * @brief Constructor for MockDerivedAlgo.
   * @param name Name of the algorithm.
   */
  explicit MockDerivedAlgoTimeOutCallback(const char *name) : AlgoBase(name) {}

  /**
   * @brief Destructor for MockDerivedAlgoFail.
   */
  ~MockDerivedAlgoTimeOutCallback() override = default;

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
  AlgoStatus Process(std::shared_ptr<AlgoRequest> req) override {

    SetStatus(AlgoStatus::INTERNAL_ERROR);
    usleep(20 * 1000); // 100 ms

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

  /**
   * @brief Get the Timeout object
   *
   * @return int
   */
  int GetTimeout() override { return 5; }

private:
};

size_t g_Timeoutcallbacks = 0;
TEST(AlgoBaseTest, AlgoTimeOutCallback) {
  {
    std::shared_ptr<MockDerivedAlgoTimeOutCallback> node =
        std::make_shared<MockDerivedAlgoTimeOutCallback>(
            "MockDerivedAlgoTimeOutCallback");

    EXPECT_EQ(node->GetAlgorithmName(), "MockDerivedAlgoTimeOutCallback");
    g_Timeoutcallbacks = 0;
    auto callback = [](void *ctx,
                       std::shared_ptr<AlgoBase::AlgoCallbackMessage> msg) {
      assert(msg != nullptr);
      switch (msg->mType) {
      case AlgoBase::AlgoMessageType::ProcessingTimeout:
        g_Timeoutcallbacks++;
        break;
      default:
        assert(true);
        break;
      }
    };
    auto eventHandler =
        std::make_shared<EventHandlerThread<AlgoBase::AlgoCallbackMessage>>(
            callback, nullptr);
    node->SetEventThread(eventHandler);

    for (int i = 0; i < 100; i++) {
      auto task = std::make_shared<Task_t>();
      task->timeoutMs = node->GetTimeout();
      task->request = std::make_shared<AlgoRequest>();
      task->request->mRequestId = i;
      node->EnqueueRequest(task);
    }
    node->WaitForQueueCompetion();
  }
  EXPECT_EQ(g_Timeoutcallbacks, 100);
}
