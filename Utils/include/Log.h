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
