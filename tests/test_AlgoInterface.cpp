#include "AlgoInterface.h"
#include <gtest/gtest.h>
#include <memory>

TEST(AlgoInterfaceTest, ProcessTest) {
  auto algoInterface = std::make_shared<AlgoInterface>();
  ASSERT_NE(algoInterface, nullptr);
  auto request = std::make_shared<AlgoRequest>();
  ASSERT_NE(request, nullptr);
  EXPECT_TRUE(algoInterface->Process(request));
}
