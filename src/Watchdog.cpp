
#include "Watchdog.h"
#include <Log.h>
#include <atomic>
#include <cstring>
#include <time.h>
static void TimerHandler(union sigval sv) {
  Watchdog *watchdog = static_cast<Watchdog *>(sv.sival_ptr);
  if (watchdog) {
    watchdog->bite();
  }
}

// Start the timer with a specified timeout (in milliseconds)
bool Watchdog::StartTimer(int timeoutMs) {
  // If the timer is already created, cancel it first
  if (timer_created_) {
    cancel_flag_.store(true);
    LOG(VERBOSE, WATCHDOG, " Timer already Created");
    CancelTimer();
  }

  timeout_ = timeoutMs;

  // Create the POSIX timer
  struct sigevent sev {};
  memset(&sev, 0, sizeof(sev));
  sev.sigev_notify = SIGEV_THREAD;
  sev.sigev_value.sival_ptr = this; // Pass "this" to the handler
  sev.sigev_notify_function = TimerHandler;

  if (timer_create(CLOCK_REALTIME, &sev, &timer_) != 0) {
    LOG(ERROR, WATCHDOG, "Failed to create timer");
    return false;
  }
  timer_created_ = true;

  // Configure the timer
  struct itimerspec its {};
  its.it_value.tv_sec = timeoutMs / 1000;
  its.it_value.tv_nsec =
      (timeoutMs % 1000) * 1000000; // Remaining milliseconds as nanoseconds

  // Set the timer to be non-repeating
  its.it_interval.tv_sec = 0;
  its.it_interval.tv_nsec = 0;

  if (timer_settime(timer_, 0, &its, nullptr) != 0) {
    LOG(ERROR, WATCHDOG, "Failed to set timer");
    timer_delete(timer_);
    timer_created_ = false;
    return false;
  }
  // LOG(VERBOSE, WATCHDOG, "Watchdog started");
  return true;
}

bool Watchdog::CancelTimer() {
  // LOG(VERBOSE, WATCHDOG, "Watchdog  Cancel Start");
  cancel_flag_.store(true);
  if (!timer_created_) {
    LOG(ERROR, WATCHDOG, "No timer to cancel");
    return false; // No timer to cancel
  }

  // Disable the timer by setting its time to 0
  struct itimerspec its {};
  if (timer_settime(timer_, 0, &its, nullptr) != 0) {
    LOG(ERROR, WATCHDOG, "Failed to reset timer");
    return false;
  }

  // Delete the timer
  if (timer_delete(timer_) != 0) {
    LOG(ERROR, WATCHDOG, "Failed to delete timer");
    return false;
  }

  timer_created_ = false;
  // LOG(VERBOSE, WATCHDOG, "Watchdog  Cancelled");
  return true;
}

// If timeout occurs, this function will be triggered

void Watchdog::bite() const {
  if (!cancel_flag_.load() && callback_) {
    callback_();
    // LOG(VERBOSE, WATCHDOG, "Watchdog  Bite");
  }
}