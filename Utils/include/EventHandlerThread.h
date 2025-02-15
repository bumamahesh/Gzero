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
#ifndef EVENTHANDLERTHREAD_H
#define EVENTHANDLERTHREAD_H
#pragma once

#include <condition_variable>
#include <functional>
#include <memory>
#include <mutex>
#include <queue>
#include "ThreadWrapper.h"

template <typename T>
class EventHandlerThread {
 public:
  using EventHandler = std::function<void(void*, std::shared_ptr<T>)>;

  // Constructor
  EventHandlerThread(EventHandler handler, void* context)
      : mHandler(handler), mContext(context), mRunning(false) {

    mPthread =
        std::make_shared<ThreadWrapper>(&EventHandlerThread::threadFunc, this);
    mPthread->ThreadSetname("EventHandlerThread");
  }

  // Destructor
  ~EventHandlerThread() { stop(); }

  // Stop the event handler thread
  void stop() {
    if (!mRunning)
      return;

    {
      std::lock_guard<std::mutex> lock(mMutex);
      mRunning = false;
    }
    mCv.notify_one();
    mPthread->join();
  }

  // Set an event to be handled by the thread
  void SetEvent(std::shared_ptr<T> event) {
    {
      std::lock_guard<std::mutex> lock(mMutex);
      mEventQueue.push(event);
    }
    mCv.notify_one();
  }

 private:
  // The function executed by the thread
  static void* threadFunc(void* arg) {
    auto self      = static_cast<EventHandlerThread*>(arg);
    self->mRunning = true;
    while (true) {
      std::shared_ptr<T> event = nullptr;
      {
        std::unique_lock<std::mutex> lock(self->mMutex);
        self->mCv.wait(lock, [&]() {
          return !self->mRunning || !self->mEventQueue.empty();
        });
        if (!self->mRunning)
          break;
        if (!self->mEventQueue.empty()) {
          event = self->mEventQueue.front();
          self->mEventQueue.pop();
        }
      }
      if (event && self->mHandler) {
        self->mHandler(self->mContext, event);
      }
    }
    return nullptr;
  }

  EventHandler mHandler                   = nullptr;
  void* mContext                          = nullptr;
  std::shared_ptr<ThreadWrapper> mPthread = nullptr;
  bool mRunning                           = false;
  std::queue<std::shared_ptr<T>> mEventQueue;
  std::mutex mMutex;
  std::condition_variable mCv;
};

#endif  // EVENTHANDLERTHREAD_H
