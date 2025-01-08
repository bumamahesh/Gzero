#include "Interface.h"

#include "Log.h"

/**
 * @brief Initializes the shared library.
 *
 * @param libhandle
 * @return SHARED_LIB_EXPORT
 */
SHARED_LIB_EXPORT int InitAlgoInterface(void **libhandle) {
  AlgoInterface *algoInterface = new AlgoInterface();
  if (algoInterface == nullptr) {
    return -1;
  }
  *libhandle = algoInterface;
  LOG(ERROR, ALGOINTERFACE, "ALGO VERSION 0.1.2");
  return 0;
}

/**
 * @brief Deinitializes the shared library and releases resources.
 *
 * @param libhandle
 * @return status
 */
SHARED_LIB_EXPORT int DeInitAlgoInterface(void **libhandle) {

  if (*libhandle == nullptr) {
    return -1;
  }
  AlgoInterface *algoInterface = static_cast<AlgoInterface *>(*libhandle);
  delete algoInterface;
  *libhandle = nullptr;
  return 0;
}
/**
 * @brief Processes capture data.
 *
 * @param libhandle
 * @param input
 * @return status
 */

SHARED_LIB_EXPORT int AlgoInterfaceProcess(void **libhandle,
                                           std::shared_ptr<AlgoRequest> input) {
  if (*libhandle == nullptr) {
    return -1;
  }
  AlgoInterface *algoInterface = static_cast<AlgoInterface *>(*libhandle);
  algoInterface->Process(input);
  return 0;
}

/**
 * @brief  Register callbacks
 *
 * @param libhandle
 * @param Callback
 * @return status
 */
SHARED_LIB_EXPORT int
RegisterCallback(void **libhandle,
                 int (*Callback)(std::shared_ptr<AlgoRequest> input)) {

  if (*libhandle == nullptr) {
    return -1;
  }
  AlgoInterface *algoInterface = static_cast<AlgoInterface *>(*libhandle);
  algoInterface->pIntfCallback = Callback;
  return 0;
}
