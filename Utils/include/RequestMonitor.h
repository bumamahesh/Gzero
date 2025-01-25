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

  // Map to store requests being monitored
  std::unordered_map<std::shared_ptr<Task_t>, Request> requests_;
};

#endif // REQUEST_MONITOR_H
