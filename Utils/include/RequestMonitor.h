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
#ifndef REQUEST_MONITOR_H
#define REQUEST_MONITOR_H

#include <atomic>
#include <chrono>
#include <memory>
#include <pthread.h>
#include <unordered_map>

struct Task_t;
typedef void (*TASKCALLBACK)(void *Ctx, std::shared_ptr<Task_t> task);
class RequestMonitor {
public:
  // Constructor that accepts tolerance and callback
  RequestMonitor();

  // Destructor that ensures the monitor thread is properly joined
  ~RequestMonitor();

  // Starts monitoring a request with a given timeout in milliseconds
  void StartRequestMonitoring(std::shared_ptr<Task_t> task, int timeoutMs);

  // Stops monitoring a request and triggers callback if tolerance is exceeded
  void StopRequestMonitoring(std::shared_ptr<Task_t> task);

  // set callback and its conext
  void SetCallback(TASKCALLBACK Callback, void *context);

private:
  // Structure to store request start time, stop time, timeout and stopped
  // status
  struct Request {
    std::chrono::high_resolution_clock::time_point start;
    std::chrono::high_resolution_clock::time_point stop;
    std::chrono::milliseconds timeout; // Timeout in milliseconds

    bool isStopped; // Flag to check if the request has been stopped
  };

  // Method to monitor timeouts for requests in a separate thread
  static void *monitorTimeouts(void *arg);

  // Mutex for thread safety
  pthread_mutex_t mutex_;

  // Callback to trigger if timeout or tolerance exceeded
  TASKCALLBACK pCallback = nullptr;
  void *pcontext = nullptr;

  // Flag to indicate if the monitor is still running
  std::atomic<bool> running_;

  // Thread for monitoring timeouts
  pthread_t monitorThread_;

  bool bThreadStarted = false;

  // fps monitor
  std::chrono::duration<double, std::milli> mdeltas;
  std::size_t mtotalRequest = 0;
  double averagfps = 0;

  // Map to store requests being monitored
  std::unordered_map<std::shared_ptr<Task_t>, Request> requests_;
};

#endif // REQUEST_MONITOR_H
