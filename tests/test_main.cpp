#include <gtest/gtest.h>

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