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
#ifndef REQUEST_SCHEDULER_H
#define REQUEST_SCHEDULER_H

#include <poll.h>
#include <sys/timerfd.h>
#include <unistd.h>
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstring>
#include <functional>
#include <iostream>
#include <memory>
#include <mutex>
#include <optional>
#include <queue>
#include <thread>

template <typename T>
class RequestScheduler {
 public:
  using Callback = std::function<void(T)>;

  explicit RequestScheduler(int fps, Callback callback,
                            bool bIsJitterEnabled = false);
  ~RequestScheduler();

  void queueRequest(T request);
  void enableJitter(bool enable);

 private:
  void processRequests();
  void startTimer();
  void stopTimer();
  void createTimer();  // Ensures timerFd is always initialized safely

  std::queue<T> requestQueue;
  std::mutex queueMutex;
  std::condition_variable cv;

  int intervalMs;
  std::atomic<bool> jitterEnabled = false;
  std::atomic<bool> running;

  Callback outputCallback;
  std::unique_ptr<std::thread> workerThread;
  int timerFd;
};

// Constructor
template <typename T>
RequestScheduler<T>::RequestScheduler(int fps, Callback callback,
                                      bool bIsJitterEnabled)
    : jitterEnabled(bIsJitterEnabled),
      running(true),
      outputCallback(callback),
      timerFd(-1) {
  intervalMs = (fps > 0) ? 1000 / fps : 33;

  createTimer();  // Ensure timerFd is always initialized

  if (jitterEnabled) {
    startTimer();
    workerThread =
        std::make_unique<std::thread>(&RequestScheduler::processRequests, this);
  }
}

// Destructor
template <typename T>
RequestScheduler<T>::~RequestScheduler() {
  running = false;

  if (workerThread && workerThread->joinable()) {
    workerThread->join();
  }

  stopTimer();

  if (timerFd != -1) {
    close(timerFd);
    timerFd = -1;
  }
}

// Add a request to the queue
template <typename T>
void RequestScheduler<T>::queueRequest(T request) {
  if (!jitterEnabled) {
    outputCallback(request);
    return;
  }
  {
    std::lock_guard<std::mutex> lock(queueMutex);
    requestQueue.push(request);
  }
  cv.notify_one();
}

// Create timer (ensures it's safely initialized)
template <typename T>
void RequestScheduler<T>::createTimer() {
  if (timerFd == -1) {
    timerFd = timerfd_create(CLOCK_MONOTONIC, 0);
    if (timerFd == -1) {
      throw std::runtime_error("Failed to create timerfd");
    }
  }
}

// Timer setup
template <typename T>
void RequestScheduler<T>::startTimer() {
  struct itimerspec ts {};
  ts.it_interval.tv_sec  = intervalMs / 1000;
  ts.it_interval.tv_nsec = (intervalMs % 1000) * 1000000;
  ts.it_value            = ts.it_interval;

  if (timerfd_settime(timerFd, 0, &ts, nullptr) == -1) {
    throw std::runtime_error("Failed to start timer");
  }
}

// Stop the timer
template <typename T>
void RequestScheduler<T>::stopTimer() {
  struct itimerspec ts {};
  timerfd_settime(timerFd, 0, &ts, nullptr);
}

// Process requests triggered by the timer
template <typename T>
void RequestScheduler<T>::processRequests() {
  struct pollfd pfd {};
  pfd.fd     = timerFd;
  pfd.events = POLLIN;

  while (running) {
    int ret = poll(&pfd, 1, 100);  // Wait up to 100ms for timer event
    if (ret == 0)
      continue;  // Timeout, loop again
    if (ret == -1) {
      if (errno == EINTR)
        continue;  // Interrupted by signal, retry
      std::cerr << "Poll error: " << strerror(errno) << "\n";
      break;
    }

    uint64_t expirations;
    ssize_t s = read(timerFd, &expirations, sizeof(expirations));
    if (s != sizeof(expirations)) {
      std::cerr << "Timer read error: " << strerror(errno) << "\n";
      continue;
    }

    std::optional<T> request;
    {
      std::lock_guard<std::mutex> lock(queueMutex);
      if (!requestQueue.empty()) {
        request = requestQueue.front();
        requestQueue.pop();
      }
    }

    if (request.has_value()) {
      outputCallback(request.value());
    }
  }
}

// Enable or disable jitter logic
template <typename T>
void RequestScheduler<T>::enableJitter(bool enable) {
  if (enable == jitterEnabled)
    return;  // No change needed

  jitterEnabled = enable;

  if (!enable) {  // Jitter is being disabled
    stopTimer();

    // Process any pending requests immediately
    std::queue<T> tempQueue;
    {
      std::lock_guard<std::mutex> lock(queueMutex);
      std::swap(tempQueue, requestQueue);
    }

    while (!tempQueue.empty()) {
      outputCallback(tempQueue.front());
      tempQueue.pop();
    }

    // Stop the worker thread safely
    running = false;
    if (workerThread && workerThread->joinable()) {
      workerThread->join();
      workerThread.reset();
    }
  } else {  // Jitter is being enabled again
    running = true;
    createTimer();  // Ensure timerFd is valid
    startTimer();

    // Only start a new thread if one isn’t already running
    if (!workerThread || !workerThread->joinable()) {
      workerThread = std::make_unique<std::thread>(
          &RequestScheduler::processRequests, this);
    }
  }
}

#endif  // REQUEST_SCHEDULER_H
