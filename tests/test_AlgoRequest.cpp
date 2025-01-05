#include "AlgoRequest.h"
#include <gtest/gtest.h>

TEST(AlgoRequestTest, AlgoRequestTest) {
  auto request = std::make_shared<AlgoRequest>();
  EXPECT_NE(request, nullptr);

  EXPECT_EQ(request->GetImageCount(), 0);
}
