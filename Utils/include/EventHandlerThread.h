#ifndef EVENTHANDLERTHREAD_H
#define EVENTHANDLERTHREAD_H
#pragma once

#include <condition_variable>
#include <functional>
#include <memory>
#include <mutex>
#include <pthread.h>
#include <queue>

template <typename T> class EventHandlerThread {
public:
  using EventHandler = std::function<void(void *, std::shared_ptr<T>)>;

  // Constructor
  EventHandlerThread(EventHandler handler, void *context)
      : mHandler(handler), mContext(context), mRunning(false) {
    pthread_create(&mPthread, nullptr, &EventHandlerThread::threadFunc, this);
    pthread_setname_np(mPthread, "EventHandlerThread");
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
    pthread_join(mPthread, nullptr);
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
  static void *threadFunc(void *arg) {
    auto self = static_cast<EventHandlerThread *>(arg);
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

  EventHandler mHandler;
  void *mContext;
  pthread_t mPthread;
  bool mRunning;
  std::queue<std::shared_ptr<T>> mEventQueue;
  std::mutex mMutex;
  std::condition_variable mCv;
};

#endif // EVENTHANDLERTHREAD_H
