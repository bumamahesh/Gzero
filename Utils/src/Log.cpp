#include "../include/Log.h"

// Function to get current time
std::string getCurrentTime() {
  auto now = std::chrono::system_clock::now();
  auto in_time_t = std::chrono::system_clock::to_time_t(now);

  std::stringstream ss;
  ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S");
  return ss.str();
}

// Function to get current thread id
std::string getCurrentThreadId() {
  std::stringstream ss;
  ss << std::this_thread::get_id();
  return ss.str();
}

// Function to get current line number
int getCurrentLineNumber() {
  return __LINE__; // Note: __LINE__ expands to the line number where it's used
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
  default:
    os << "UNKNOWN";
    break;
  }
  return os;
}

// Log class static member definition and implementation
LogLevel Log::logLevel = LogLevel::L_WARNING; // Initialize static member

void Log::SetLevel(LogLevel level) { logLevel = level; }

LogLevel Log::GetLevel() { return logLevel; }
