#include "Watchdog.h"
#include <chrono>
#include <gtest/gtest.h>
#include <thread>

int g_WatchDogTotalCallbacks = 0;
int timeoutms = 100;

int minResolutionMs = 10;

TEST(WatchdogTest, BarkTest) {
  g_WatchDogTotalCallbacks = 0;
  auto barkcallback = []() { g_WatchDogTotalCallbacks++; };

  Watchdog wd(barkcallback);

  // Start the timer with a timeout
  EXPECT_EQ(wd.StartTimer(timeoutms), true);

  // wait for more then timeout to bark
  std::this_thread::sleep_for(
      std::chrono::milliseconds(timeoutms + minResolutionMs));

  EXPECT_EQ(wd.CancelTimer(), true);

  EXPECT_EQ(g_WatchDogTotalCallbacks, 1);
}

TEST(WatchdogTest, ShouldNotBiteTest) {
  auto callback = []() { assert(true); };

  Watchdog wd(callback);

  // Start the timer with a timeout
  EXPECT_EQ(wd.StartTimer(timeoutms), true);

  // wait less time then  timout and cancel timeout
  std::this_thread::sleep_for(
      std::chrono::milliseconds(timeoutms - minResolutionMs));

  EXPECT_EQ(wd.CancelTimer(), true);
}

TEST(WatchdogTest, StressTest) {}
