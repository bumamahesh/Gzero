/*
 * Copyright (c) [2025] [Uma Mahesh B]
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#include "../include/KpiMonitor.h"
#include "../include/Log.h"

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
    LOG(VERBOSE, KPI, " %s took %f seconds", mMsg.c_str(), delta.count());
  } else {
    LOG(ERROR, KPI, "Took %f seconds", delta.count());
  }
}
