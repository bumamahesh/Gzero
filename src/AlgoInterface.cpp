#include "AlgoInterface.h"

/**
 * @brief Construct a new Algo Interface:: Algo Interface object
 *
 */
AlgoInterface::AlgoInterface() { mSession = std::make_shared<AlgoSession>(); }

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
