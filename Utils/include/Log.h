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

#include <chrono>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>

// Logging levels
enum class LogLevel {
  L_DEBUG = 0,
  L_INFO,
  L_VERBOSE,
  L_WARNING,
  L_ERROR,
  L_FATAL
};

#define DEBUG 0
#define INFO 1
#define VERBOSE 2
#define WARNING 3
#define ERROR 4
#define FATAL 5

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

// Function declarations
std::string getCurrentTime();
std::string getCurrentThreadId();
// Macro to log message with log component, log level, and variable parameters
#define LOG(level, component, fmt, ...)                                        \
  do {                                                                         \
    if (Log::GetLevel() <= static_cast<LogLevel>(level)) {                     \
      char buffer[1024];                                                       \
      std::snprintf(buffer, sizeof(buffer), fmt, ##__VA_ARGS__);               \
      std::stringstream ss;                                                    \
      ss << "[" << getCurrentTime() << "]"                                     \
         << "[" << component << "]"                                            \
         << "[" << __func__ << "]"                                             \
         << "[" << __LINE__ << "]"                                             \
         << "[" << getCurrentThreadId() << "] " << buffer << std::endl;        \
      std::cerr << ss.str();                                                   \
    }                                                                          \
  } while (0)

// Overload the << operator for LogLevel enum class
std::ostream &operator<<(std::ostream &os, const LogLevel &level);

// Log class for managing log levels
class Log {
public:
  static void SetLevel(LogLevel level);
  static LogLevel GetLevel();

private:
  static LogLevel logLevel;
};

#endif // LOG_H
