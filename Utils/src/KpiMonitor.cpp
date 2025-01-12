#include "KpiMonitor.h"
#include "Log.h"

/**
 * @brief Construct a new Kpi Monitor:: Kpi Monitor object
 *
 * @param Msg
 */
KpiMonitor::KpiMonitor(std::string Msg) : mMsg(Msg) {
  mStartTimer = std::chrono::high_resolution_clock::now();
}

/**
 * @brief Destroy the Kpi Monitor:: Kpi Monitor object
 *
 */
KpiMonitor::~KpiMonitor() {
  if (mMsg.size() > 0) {
    delta = std::chrono::high_resolution_clock::now() - mStartTimer;
    LOG(ERROR, KPI, " %s took %f seconds", mMsg.c_str(), delta.count());
  } else {
    LOG(ERROR, KPI, "Took %f seconds", delta.count());
  }
}
