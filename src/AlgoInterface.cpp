#include "AlgoInterface.h"
#include "Log.h"
#include <cassert>
/**
 * @brief Construct a new Algo Interface:: Algo Interface object
 *
 */
AlgoInterface::AlgoInterface() {
  mSession = std::make_shared<AlgoSession>(
      AlgoInterface::SessionCallbackHandler, this);
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
    mSession->SessionProcess(request);
  } else {
    LOG(ERROR, ALGOINTERFACE, "mSession is nullptr");
  }
  return true;
}

/**
 * @brief Session Callback Handler
 *
 * @param pctx
 * @param input
 */
void AlgoInterface::SessionCallbackHandler(void *pctx,
                                           std::shared_ptr<AlgoRequest> input) {
  assert(pctx != nullptr);
  AlgoInterface *algoInterface = static_cast<AlgoInterface *>(pctx);
  if (algoInterface->pIntfCallback) {
    algoInterface->pIntfCallback(input);
  }
}
