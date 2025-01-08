#ifndef ALGO_INTERFACE_H
#define ALGO_INTERFACE_H

#include "AlgoSession.h"

class AlgoInterface {
public:
  AlgoInterface();
  ~AlgoInterface();
  bool Process(std::shared_ptr<AlgoRequest> request);
  int (*pIntfCallback)(std::shared_ptr<AlgoRequest> input) = nullptr;

private:
  std::shared_ptr<AlgoSession> mSession;
  static void SessionCallbackHandler(void *pctx,
                                     std::shared_ptr<AlgoRequest> input);

  // decision manager instance @todo
};
#endif // ALGO_INTERFACE_H
