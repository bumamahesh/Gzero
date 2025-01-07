#include "EventHandlerThread.h"
#include <atomic>
#include <chrono>
#include <gtest/gtest.h>
#include <thread>

typedef struct AlgoEvent {
  void *arg = nullptr;
  AlgoEvent(void *arg = nullptr) : arg(arg) {}
} AlgoEvent;

TEST(EventHandlerThreadTest, BasicTest) {
  std::atomic<int> counter(0);

  // Use a condition variable to wait for all events to be processed
  std::mutex mtx;
  std::condition_variable cv;
  bool done = false;

  auto handler = [&](void *context, std::shared_ptr<AlgoEvent>) {
    counter++;
    {
      std::lock_guard<std::mutex> lock(mtx);
      if (counter == 10) { // If we processed 10 events, notify the main thread
        done = true;
        cv.notify_one();
      }
    }
  };

  EventHandlerThread<AlgoEvent> thread(handler, nullptr);

  // Push 10 events into the queue
  for (int i = 0; i < 10; ++i) {
    std::shared_ptr<AlgoEvent> event = std::make_shared<AlgoEvent>(nullptr);
    thread.SetEvent(event);
  }

  // Wait for the condition variable to notify that all events have been
  // processed
  {
    std::unique_lock<std::mutex> lock(mtx);
    cv.wait(lock, [&done] { return done; });
  }

  // Stop the thread and assert the counter
  thread.stop();
  ASSERT_EQ(counter.load(), 10);
}