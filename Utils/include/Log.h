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
#ifndef LOG_H
#define LOG_H
#pragma once

#include <cstdarg>
#include <cstdio>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

extern std::ofstream logFile;
// Logging levels
// Updated LogLevel enum class with VERBOSE included
enum class LogLevel {
  L_TRACE = 0,  // Logs even more detailed than DEBUG  //more logs
  L_FATAL,      // Critical errors, often resulting in termination //less logs
  L_ERROR,      // Errors during execution
  L_DEBUG,      // Debugging information
  L_VERBOSE,    // More detailed logs than DEBUG, often for diagnostics
  L_INFO,       // General informational messages
  L_WARNING,    // Warnings about potentially harmful situations

};
extern LogLevel logLevel;
// Updated macro definitions linked to LogLevel values
#define TRACE (LogLevel::L_TRACE)
#define DEBUG (LogLevel::L_DEBUG)
#define VERBOSE (LogLevel::L_VERBOSE)
#define INFO (LogLevel::L_INFO)
#define WARNING (LogLevel::L_WARNING)
#define ERROR (LogLevel::L_ERROR)
#define FATAL (LogLevel::L_FATAL)

#define TASKQUEUE "TASKQUEUE"
#define ALGOBASE "ALGOBASE"
#define ALGOLIBLOADER "ALGOLIBLOADER"
#define ALGOMANAGER "ALGOMANAGER"
#define ALGOPIPELINE "ALGOPIPELINE"
#define ALGOSESSION "ALGOSESSION"
#define ALGOINTERFACE "ALGOINTERFACE"
#define ALGOTIMER "ALGOTIMER"
#define WATCHDOG "WATCHDOG"
#define REQUESTMONITOR "REQUESTMONITOR"
#define KPI "KPI"
#define ALGODECISIONMANAGER "ALGODECISIONMANAGER"

// Function declarations
std::string getCurrentTime();
std::string getCurrentThreadId();
std::string getLogLevelAbbreviation(LogLevel level);
// Macro to log message with log component, log level, and variable parameters
void LOG(LogLevel level, const std::string& component, const char* fmt, ...);

void SetLevel(LogLevel level);
LogLevel GetLevel();

#endif  // LOG_H
