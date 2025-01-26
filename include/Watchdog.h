
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
#ifndef WAQTCHDOG_H
#define WAQTCHDOG_H
#pragma once

#include <atomic>
#include <signal.h>
#include <time.h>

class Watchdog {
private:
  int timeout_ = 0; // in millisecond
  timer_t timer_;
  std::atomic<bool> timer_created_;
  void (*callback_)();

public:
  explicit Watchdog(void (*callback)())
      : timer_created_(false), callback_(callback) {
    cancel_flag_.store(false);
    thread_created_ = false;
  }

  bool StartTimer(int timeoutMs);
  bool CancelTimer();
  /*if timout bike will be called*/
  void bite() const;
  bool thread_created_;
  std::atomic<bool> cancel_flag_;
};
#endif