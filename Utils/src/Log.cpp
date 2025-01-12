#include "../include/Log.h"
#include <chrono>
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
  ss << std::this_thread::get_id();
  return ss.str();
}

// Overload the << operator for LogLevel enum class
std::ostream &operator<<(std::ostream &os, const LogLevel &level) {
  switch (level) {
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

// Log class static member definition and implementation
LogLevel Log::logLevel = LogLevel::L_DEBUG; // Initialize static member

void Log::SetLevel(LogLevel level) { logLevel = level; }

LogLevel Log::GetLevel() { return logLevel; }
