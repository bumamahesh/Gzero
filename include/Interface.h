#include "AlgoInterface.h"
#include <memory>
#define SHARED_LIB_EXPORT __attribute__((visibility("default")))
// API declarations
extern "C" {

/**
 * @brief  Initializes the shared library.
 *
 * @param libhandle
 * @return status
 */
SHARED_LIB_EXPORT int InitAlgoInterface(void **libhandle);

/**
 * @brief Deinitializes the shared library and releases resources.
 *
 * @param libhandle
 * @return status
 */
SHARED_LIB_EXPORT int DeInitAlgoInterface(void **libhandle);

/**
 * @brief Processes capture data.
 *
 * @param input
 * @return status
 */
SHARED_LIB_EXPORT int AlgoInterfaceProcess(void **libhandle,
                                           std::shared_ptr<AlgoRequest> input);

/**
 * @brief  Register callbacks
 *
 * @param Callback
 * @return status
 */
SHARED_LIB_EXPORT int
RegisterCallback(void **libhandle,
                 int (*Callback)(std::shared_ptr<AlgoRequest> input));
}