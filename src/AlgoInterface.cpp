#include "AlgoInterface.h"

/**
 * @brief Construct a new Algo Interface Session:: Algo Interface Session object
 *
 * @param pAlgoInterface
 */
AlgoInterfaceSession::AlgoInterfaceSession(AlgoInterface *pAlgoInterface) {
  this->pAlgoInterface = pAlgoInterface;
}

/**
 * @brief Destroy the Algo Interface Session:: Algo Interface Session object
 *
 */
AlgoInterfaceSession::~AlgoInterfaceSession() { pAlgoInterface = nullptr; }

std::vector<AlgoId> AlgoInterfaceSession::GetAlgoList() {

  if (pAlgoInterface) {
    // pAlgoInterface->GetAlgoList();
    /**get object of Decisionmanager and get a algo list  */
    return {ALGO_NOP, ALGO_HDR, ALGO_BOKEH};
  } else {
    // LOG(ERROR, ALGOINTERFACE, "pAlgoInterface is nullptr");
  }
  return {};
}

/**
 * @brief Construct a new Algo Interface:: Algo Interface object
 *
 */
AlgoInterface::AlgoInterface() {
  mSession = std::make_shared<AlgoInterfaceSession>(this);
}

/**
 * @brief Destroy the Algo Interface:: Algo Interface object
 *
 */
AlgoInterface::~AlgoInterface() {}

/**
 * @brief
 *
 * @param request
 * @return true
 * @return false
 */
bool AlgoInterface::Process(std::shared_ptr<AlgoRequest> request) {

  if (mSession) {
    mSession->Process(request);
  } else {
    // LOG(ERROR, ALGOINTERFACE, "mSession is nullptr");
  }
  return true;
}
