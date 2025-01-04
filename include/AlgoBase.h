#ifndef ALGO_BASE_H
#define ALGO_BASE_H

#include "TaskQueue.h"
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <pthread.h>
#include <queue>
#include <string>
#include <unordered_map>

class AlgoBase {
public:
  enum class AlgoStatus {
    SUCCESS = 0,
    NOT_INITIALIZED = 1,
    ALREADY_OPEN = 2,
    ALREADY_CLOSED = 3,
    INVALID_INPUT = 4,
    RESOURCE_UNAVAILABLE = 5,
    TIMEOUT = 6,
    OPERATION_FAILED = 7,
    OUT_OF_MEMORY = 8,
    PERMISSION_DENIED = 9,
    NOT_SUPPORTED = 10,
    INTERNAL_ERROR = 11
  };

  typedef enum AlgoMeassageType {
    ALGO_PROCESSING_COMPLETED, // A generic processing completion
    ALGO_PROCESSING_FAILED,    // Processing failed
    ALGO_PROCESSING_TIMEOUT,   // Processing timed out
    ALGO_PROCESSING_PARTIAL    // An intermediate processing step completed (not
                               // the last)
  } ALGOMESSAGETYPE;

  typedef struct AlgoCallBaclMessage {
    ALGOMESSAGETYPE type;
    AlgoStatus status;
    std::shared_ptr<Task_t> request;
  } ALGOCALLBACKMSG;

  struct AlgorithmOperations {
    std::string algorithm_name;
    void *context = nullptr;
  };
  // Constructors
  AlgoBase();
  // Constructor
  explicit AlgoBase(const std::string &);
  // Destructor
  virtual ~AlgoBase();
  // Public member functions
  virtual AlgoStatus Open() = 0;
  virtual AlgoStatus Process() = 0;
  virtual AlgoStatus Close() = 0;
  void StopAlgoThread();
  AlgoStatus GetStatus() const;
  std::string GetStatusString() const;
  std::string GetAlgorithmName() const;
  size_t GetAlgoId() const;
  void EnqueueRequest(std::shared_ptr<Task_t> request);
  void SetNotifyEvent(
      void (*EventHandler)(void *ctx, std::shared_ptr<ALGOCALLBACKMSG> msg));
  void WaitForQueueCompetion();
  void SetNextAlgo(std::weak_ptr<AlgoBase>);
  std::weak_ptr<AlgoBase> GetNextAlgo();
  void *m_pipCtx = nullptr;

  // Callback function to notify the event Upper Layer
  void (*NotifyEvent)(void *ctx,
                      std::shared_ptr<ALGOCALLBACKMSG> msg) = nullptr;

protected:
  AlgorithmOperations ops_;
  AlgoStatus current_status_ = AlgoStatus::SUCCESS;
  pthread_t thread_;
  bool thread_started_ = false;
  // std::mutex thread_mutex_;
  std::condition_variable thread_cond_;
  std::condition_variable queue_cond_;
  std::atomic<bool> should_exit_ = false;
  std::shared_ptr<TaskQueue> mAlgoThread;
  size_t algo_id_ = 0XDEADBEEF;
  void SetStatus(AlgoStatus status);

  /*Linked list */
  std::weak_ptr<AlgoBase> m_nextAlgo;

private:
  static void ThreadFunction(void *Ctx, std::shared_ptr<Task_t> task);
  static void ThreadCallback(void *Ctx, std::shared_ptr<Task_t> task);
};

#endif // ALGO_BASE_H
