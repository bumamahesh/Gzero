#ifndef ALGO_BASE_H
#define ALGO_BASE_H

#include "AlgoDefs.h"
#include "AlgoRequest.h"
#include "EventHandlerThread.h"
#include "KpiMonitor.h"
#include "TaskQueue.h"
#include <memory>
#include <pthread.h>
#include <string>

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
    INTERNAL_ERROR = 11,
    FAILURE
  };

  enum class AlgoMessageType {
    ProcessingCompleted, // A generic processing completion
    ProcessingFailed,    // Processing failed
    ProcessingTimeout,   // Processing timed out
    ProcessingPartial,   // An intermediate processing step completed (not the
                         // last)
    ProcessDone          // All nodes are done Processing
  };

  typedef struct AlgoCallbackMessage {
    AlgoMessageType mType;
    AlgoStatus mStatus;
    std::shared_ptr<Task_t> mRequest;
    AlgoId mAlgoId;
    AlgoCallbackMessage(AlgoMessageType type, AlgoStatus status,
                        std::shared_ptr<Task_t> request, AlgoId algoId)
        : mType(type), mStatus(status), mRequest(request), mAlgoId(algoId) {}
  } AlgoCallbackMessage;

  struct AlgorithmOperations {
    std::string mAlgoName;
    void *pctx = nullptr;
  };
  // Constructors
  AlgoBase();
  // Constructor
  explicit AlgoBase(const char *name);
  // Destructor
  virtual ~AlgoBase();
  // Public member functions
  virtual AlgoStatus Open() = 0;
  virtual AlgoStatus Process(std::shared_ptr<AlgoRequest> req) = 0;
  virtual AlgoStatus Close() = 0;
  virtual int GetTimeout() = 0;
  void StopAlgoThread();
  AlgoStatus GetAlgoStatus() const;
  std::string GetStatusString() const;
  std::string GetAlgorithmName() const;
  AlgoId GetAlgoId() const;
  void EnqueueRequest(std::shared_ptr<Task_t> request);
  void SetEventThread(
      std::shared_ptr<EventHandlerThread<AlgoBase::AlgoCallbackMessage>>
          mEventCallbackThread);
  void WaitForQueueCompetion();
  void SetNextAlgo(std::weak_ptr<AlgoBase>);
  std::weak_ptr<AlgoBase> GetNextAlgo();
  void SetEvent(std::shared_ptr<AlgoCallbackMessage> msg);
  bool bIslastNode = false;

protected:
  AlgorithmOperations mAlgoOperations;
  AlgoStatus mCurrentStatus = AlgoStatus::SUCCESS;
  std::shared_ptr<TaskQueue> mAlgoThread;
  AlgoId mAlgoId = ALGO_MAX;
  void SetStatus(AlgoStatus status);

  /*Linked list */
  std::weak_ptr<AlgoBase> mNextAlgo;
  std::shared_ptr<EventHandlerThread<AlgoBase::AlgoCallbackMessage>>
      pEventHandlerThread = nullptr;

private:
  static void ThreadFunction(void *Ctx, std::shared_ptr<Task_t> task);
  static void ThreadCallback(void *Ctx, std::shared_ptr<Task_t> task);
  static void ProcessTimeoutCallback(void *Ctx, std::shared_ptr<Task_t> task);
};

#endif // ALGO_BASE_H
