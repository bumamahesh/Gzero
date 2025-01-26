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
#include "RequestMonitor.h"
#include "Log.h"
#include <cassert>
#include <chrono>
#include <unistd.h>
/**
 * @brief Construct a new Request Monitor:: Request Monitor object
 *
 */
RequestMonitor::RequestMonitor() : running_(true) {
  // Initialize mutex
  pthread_mutex_init(&mutex_, nullptr);

  // Create the monitoring thread
  pthread_create(&monitorThread_, nullptr, &RequestMonitor::monitorTimeouts,
                 this);
  while (!bThreadStarted) {
    usleep(50);
  }
  // LOG(ERROR, REQUESTMONITOR, " Request Monitor Created ");
}

/**
 * @brief Destroy the Request Monitor:: Request Monitor object
 *
 */
RequestMonitor::~RequestMonitor() {
  running_ = false;
  if (monitorThread_) {
    pthread_join(monitorThread_, nullptr); // Wait for monitor thread to finish
  }

  // Destroy mutex
  pthread_mutex_destroy(&mutex_);
}

/**
 * @brief api to Set callback and its context
 *
 * @param Callback
 * @param context
 */
void RequestMonitor::SetCallback(TASKCALLBACK Callback, void *context) {
  pCallback = Callback;
  pcontext = context;
}

void RequestMonitor::StartRequestMonitoring(std::shared_ptr<Task_t> task,
                                            int timeoutMs) {
  pthread_mutex_lock(&mutex_);

  if (requests_.find(task) != requests_.end()) {
    LOG(ERROR, REQUESTMONITOR, "Request %p is already being monitored.",
        task.get());
    pthread_mutex_unlock(&mutex_);
    return;
  }

  requests_[task].start = std::chrono::high_resolution_clock::now();
  requests_[task].timeout = std::chrono::milliseconds(timeoutMs);
  requests_[task].isStopped = false;
  // LOG(ERROR, REQUESTMONITOR, "Started monitoring request %p", task.get());

  pthread_mutex_unlock(&mutex_);
}

/**
 * @brief api to stop monitoring given request number
 *
 * @param requestId
 */
void RequestMonitor::StopRequestMonitoring(std::shared_ptr<Task_t> task) {
  pthread_mutex_lock(&mutex_);
  auto it = requests_.find(task);
  if (it == requests_.end()) {
    // LOG(VERBOSE, REQUESTMONITOR, "Request %d was not being monitored.",
    //     requestId);
    pthread_mutex_unlock(&mutex_);
    return;
  }

  requests_.erase(task); // Remove request from tracking as it's completed
  pthread_mutex_unlock(&mutex_);
}

/**
 * @brief  Thread to monitor request
 *
 * @param arg
 * @return void*
 */
void *RequestMonitor::monitorTimeouts(void *arg) {
  RequestMonitor *monitor = static_cast<RequestMonitor *>(arg);

  // LOG(ERROR, REQUESTMONITOR, "Entering Monitor Thread");
  monitor->bThreadStarted = true;

  while (monitor->running_) {
    // std::this_thread::sleep_for(std::chrono::seconds(1)); // Check every
    // second
    usleep(10); // do not choke cpu
    pthread_mutex_lock(&monitor->mutex_);
    auto now = std::chrono::high_resolution_clock::now();

    for (auto it = monitor->requests_.begin();
         it != monitor->requests_.end();) {
      auto &request = it->second;

      if (!request.isStopped) {
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
            now - request.start);
        if (elapsed > request.timeout) {
          /*LOG(ERROR, REQUESTMONITOR,
              "Req exceeded timeout! elapsed=%ld ms reqtimeout=%ld ms",
              elapsed.count(), request.timeout.count());*/
          if (monitor->pCallback) {
            monitor->pCallback(monitor->pcontext,
                               it->first); // Trigger the callback
          }
          it = monitor->requests_.erase(it); // Remove from the tracking map
        } else {
          ++it;
        }
      } else {
        ++it;
      }
    }
    pthread_mutex_unlock(&monitor->mutex_);
  }
  // LOG(ERROR, REQUESTMONITOR, "Exiting Monitor Thread");
  return nullptr;
}
