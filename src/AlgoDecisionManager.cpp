#include "AlgoDecisionManager.h"
#include "Log.h"
/**
 * @brief Construct a new Algo Decision Manager:: Algo Decision Manager object
 *
 */
AlgoDecisionManager::AlgoDecisionManager() {}

/**
 * @brief Destroy the Algo Decision Manager:: Algo Decision Manager object
 *
 */
AlgoDecisionManager::~AlgoDecisionManager() {}

/**
 * @brief
 *
 * @param algoId
 * @return true
 * @return false
 */
bool AlgoDecisionManager::IsAlgoEnabled(AlgoId algoId) {
  if (algoId < ALGO_BASE_ID || algoId > ALGO_MAX)
    return false;
  return ((mAlgoFlag & (ALGO_MASK(algoId))) != 0);
}

/**
 * @brief Get Algo Flags
 *
 * @return unsigned int
 */
uint32_t AlgoDecisionManager::GetAlgoFlag() const {
  return mAlgoFlag;
}

/**
 * @brief Set Algo Flags
 *
 * @param unsigned int
 */
uint32_t AlgoDecisionManager::SetAlgoFlag(AlgoId algoId) {
  if (algoId < ALGO_BASE_ID || algoId > ALGO_MAX)
    return mAlgoFlag;
  mAlgoFlag |= (ALGO_MASK(algoId));
  return mAlgoFlag;
}

/**
 * @brief
 *
 * @return std::vector<AlgoId>
 */
std::vector<AlgoId> AlgoDecisionManager::ParseMetadata(
    std::shared_ptr<AlgoRequest> req) {
  (void)(req);
  std::vector<AlgoId> algos;

  for (int i = (int)ALGO_BASE_ID; i <= (int)ALGO_MAX; ++i) {
    // LOG(DEBUG, ALGODECISIONMANAGER, "i =%d enable= %d", i,
    //     IsAlgoEnabled(static_cast<AlgoId>(i)));
    if (IsAlgoEnabled(static_cast<AlgoId>(i))) {
      algos.push_back(static_cast<AlgoId>(i));
    }
  }

  if (algos.size() == 0) {
    SetAlgoFlag(ALGO_NOP);
    algos.push_back(static_cast<AlgoId>(ALGO_NOP));
  }

  /*LOG(VERBOSE, ALGODECISIONMANAGER,
      "BaseAlgoManager:: Algos Size::%ld mAlgoFlag = %d", algos.size(),
      mAlgoFlag);*/

  return algos;
}
