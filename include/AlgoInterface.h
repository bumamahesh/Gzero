#ifndef ALGO_INTERFACE_H
#define ALGO_INTERFACE_H

#include "AlgoSession.h"

class AlgoInterface;
class AlgoInterfaceSession : public AlgoSession {

public:
  AlgoInterfaceSession(AlgoInterface *pAlgoInterface);
  ~AlgoInterfaceSession();

  std::vector<AlgoId> GetAlgoList() override;

private:
  AlgoInterface *pAlgoInterface = nullptr;
};

class AlgoInterface {
public:
  AlgoInterface();
  ~AlgoInterface();
  bool Process(std::shared_ptr<AlgoRequest> request);

private:
  std::shared_ptr<AlgoInterfaceSession> mSession;
  // decision manager instance @todo
};

#endif // ALGO_INTERFACE_H
