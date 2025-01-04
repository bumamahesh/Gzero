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
    ALGOMESSAGETYPE mType;
    AlgoStatus mStatus;
    std::shared_ptr<Task_t> mRequest;
  } ALGOCALLBACKMSG;

  struct AlgorithmOperations {
    std::string mAlgoName;
    void *pctx = nullptr;
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
  void *pPipelineCtx = nullptr;

  // Callback function to notify the event Upper Layer
  void (*pNotifyEvent)(void *ctx,
                       std::shared_ptr<ALGOCALLBACKMSG> msg) = nullptr;

protected:
  AlgorithmOperations mAlgoOperations;
  AlgoStatus mCurrentStatus = AlgoStatus::SUCCESS;
  std::shared_ptr<TaskQueue> mAlgoThread;
  size_t mAlgoId = 0XDEADBEEF;
  void SetStatus(AlgoStatus status);

  /*Linked list */
  std::weak_ptr<AlgoBase> mNextAlgo;

private:
  static void ThreadFunction(void *Ctx, std::shared_ptr<Task_t> task);
  static void ThreadCallback(void *Ctx, std::shared_ptr<Task_t> task);
};

#endif // ALGO_BASE_H
