#ifndef ALGO_INTERFACE_H
#define ALGO_INTERFACE_H

#include "AlgoSession.h"

class AlgoInterface {
public:
  AlgoInterface();
  ~AlgoInterface();
  bool Process(std::shared_ptr<AlgoRequest> request);

private:
  std::shared_ptr<AlgoSession> mSession;
  // decision manager instance @todo
};

#endif // ALGO_INTERFACE_H
