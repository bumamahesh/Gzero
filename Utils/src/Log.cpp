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
#include "../include/Log.h"
#include <chrono>
#include <iomanip>
//#include <thread>
#include <pthread.h>
// Function to get current time
std::string getCurrentTime() {
  auto now = std::chrono::system_clock::now();
  auto in_time_t = std::chrono::system_clock::to_time_t(now);
  auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(
                          now.time_since_epoch()) %
                      1000;

  std::stringstream ss;
  ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S");
  ss << '.' << std::setfill('0') << std::setw(3) << milliseconds.count();
  return ss.str();
}

// Function to get current thread id
std::string getCurrentThreadId() {
  std::stringstream ss;
  ss << pthread_self();
  return ss.str();
}

// Overload the << operator for LogLevel enum class
std::ostream &operator<<(std::ostream &os, const LogLevel &level) {
  switch (level) {
  case LogLevel::L_TRACE:
    os << "TRACE";
    break;
  case LogLevel::L_DEBUG:
    os << "DEBUG";
    break;
  case LogLevel::L_INFO:
    os << "INFO";
    break;
  case LogLevel::L_WARNING:
    os << "WARNING";
    break;
  case LogLevel::L_ERROR:
    os << "ERROR";
    break;
  case LogLevel::L_FATAL:
    os << "FATAL";
    break;
  case LogLevel::L_VERBOSE:
    os << "VERBOSE";
    break;
  default:
    os << "UNKNOWN";
    break;
  }
  return os;
}

// Helper function to get log level abbreviation
std::string getLogLevelAbbreviation(LogLevel level) {
  switch (level) {
  case LogLevel::L_TRACE:
    return "T"; // Trace
  case LogLevel::L_DEBUG:
    return "D"; // Debug
  case LogLevel::L_VERBOSE:
    return "V"; // Verbose
  case LogLevel::L_INFO:
    return "I"; // Info
  case LogLevel::L_WARNING:
    return "W"; // Warning
  case LogLevel::L_ERROR:
    return "E"; // Error
  case LogLevel::L_FATAL:
    return "F"; // Fatal
  default:
    return "?"; // Unknown level
  }
}

// Log class static member definition and implementation
LogLevel Log::logLevel = LogLevel::L_VERBOSE; // Initialize static member

void Log::SetLevel(LogLevel level) { logLevel = level; }

LogLevel Log::GetLevel() { return logLevel; }
