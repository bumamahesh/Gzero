#include "../include/AlgoDecisionManager.h"
#include <gtest/gtest.h>

TEST(AlgoDecisionManagerTest, IsAlgoEnabledTest) {
  AlgoDecisionManager manager;

  // Test with ALGO_MAX
  EXPECT_FALSE(manager.IsAlgoEnabled(ALGO_MAX));

  // Test enabling and disabling algorithms
  manager.SetAlgoFlag(ALGO_MANDELBROTSET);
  EXPECT_TRUE(manager.IsAlgoEnabled(ALGO_MANDELBROTSET));

  manager.SetAlgoFlag(ALGO_HDR);
  EXPECT_TRUE(manager.IsAlgoEnabled(ALGO_HDR));
  EXPECT_TRUE(manager.IsAlgoEnabled(ALGO_MANDELBROTSET));

  // Test with an invalid AlgoId
  EXPECT_FALSE(manager.IsAlgoEnabled(
      static_cast<AlgoId>(static_cast<int>(ALGO_MAX) + 1)));
}

TEST(AlgoDecisionManagerTest, GetAlgoFlagTest) {
  AlgoDecisionManager manager;
  EXPECT_EQ(manager.GetAlgoFlag(), 0);

  manager.SetAlgoFlag(ALGO_MANDELBROTSET);
  EXPECT_NE(manager.GetAlgoFlag(), 0);

  manager.SetAlgoFlag(ALGO_HDR);
  EXPECT_NE(manager.GetAlgoFlag(), 0);

  // Test with an invalid AlgoId
  EXPECT_EQ(manager.GetAlgoFlag(), (1U << ALGO_OFFSET(ALGO_MANDELBROTSET)) |
                                       (1U << ALGO_OFFSET(ALGO_HDR)));
}

TEST(AlgoDecisionManagerTest, SetAlgoFlagTest) {
  AlgoDecisionManager manager;
  unsigned ExpectedFlag = (1 << ALGO_OFFSET(ALGO_MANDELBROTSET));
  EXPECT_EQ(manager.SetAlgoFlag(ALGO_MANDELBROTSET),
            (1U << ALGO_OFFSET(ALGO_MANDELBROTSET)));

  ExpectedFlag |= (1 << ALGO_OFFSET(ALGO_HDR));
  EXPECT_EQ(manager.SetAlgoFlag(ALGO_HDR), ExpectedFlag);

  // Test with an invalid AlgoId
  EXPECT_EQ(
      manager.SetAlgoFlag(static_cast<AlgoId>(static_cast<int>(ALGO_MAX) + 1)),
      ExpectedFlag);
}

TEST(AlgoDecisionManagerTest, ParseMetadataTest) {
  AlgoDecisionManager manager;
  auto req = std::make_shared<AlgoRequest>();
  auto algos = manager.ParseMetadata(req);
  ASSERT_EQ(algos.size(), 1);
}