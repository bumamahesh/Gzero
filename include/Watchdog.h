
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