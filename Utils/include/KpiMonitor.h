#ifndef KPIMONITOR_H
#define KPIMONITOR_H
#pragma once
#include <chrono>
#include <string>

class KpiMonitor {
public:
  KpiMonitor(std::string Msg = "");
  ~KpiMonitor();

private:
  std::string mMsg;
  std::chrono::high_resolution_clock::time_point mStartTimer;
  std::chrono::duration<double> delta;
};
#endif // KPIMONITOR_H
