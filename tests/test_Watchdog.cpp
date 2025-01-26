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
