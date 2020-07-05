#pragma once

#include <iostream>
#include <mutex>
#include <sstream>
#include <string>
#include <thread>
#include <cpputil/log/log.h>

namespace cpputil {
namespace log {

struct DummyMutex {
  void lock() {}
  void unlock() {}
  bool try_lock() { return true; }
};

template <class Mutex = std::mutex>
class LogBuilder {
 public:
  LogBuilder() {
    _s.reserve(LOG_MAX_SIZE_INFO);
  }

  ~LogBuilder() = default;

  template <class T>
  void write(const std::string& key, const T& value) {
    std::lock_guard<Mutex> l(_mu);
    if (!_s.empty()) {
      _s.append(" ");
    }
    _s.append(key);
    _s.append("=");
    _s.append(std::to_string(value));
  }

  void write(const std::string& key, const char* value) {
    std::lock_guard<Mutex> l(_mu);
    if (!_s.empty()) {
      _s.append(" ");
    }
    _s.append(key);
    _s.append("=");
    _s.append(value);
  }

  void write(const std::string& key, const std::string& value) {
    std::lock_guard<Mutex> l(_mu);
    if (!_s.empty()) {
      _s.append(" ");
    }
    _s.append(key);
    _s.append("=");
    _s.append(value);
  }

  void flush() { flush_as_info(); }

  void flush_as_trace() {
    std::lock_guard<Mutex> l(_mu);
    LOGF_TRACE("%s", _s.c_str());
    _s.clear();
  }

  void flush_as_trace_every_n(int every_n) {
    std::lock_guard<Mutex> l(_mu);
    LOGF_TRACE_EVERY_N(every_n, "%s", _s.c_str());
    _s.clear();
  }

  void flush_as_debug() {
    std::lock_guard<Mutex> l(_mu);
    LOGF_DEBUG("%s", _s.c_str());
    _s.clear();
  }

  void flush_as_debug_every_n(int every_n) {
    std::lock_guard<Mutex> l(_mu);
    LOGF_DEBUG_EVERY_N(every_n, "%s", _s.c_str());
    _s.clear();
  }

  void flush_as_info() {
    std::lock_guard<Mutex> l(_mu);
    LOGF_INFO("%s", _s.c_str());
    _s.clear();
  }

  void flush_as_info_every_n(int every_n) {
    std::lock_guard<Mutex> l(_mu);
    LOGF_INFO_EVERY_N(every_n, "%s", _s.c_str());
    _s.clear();
  }

  void flush_as_warn() {
    std::lock_guard<Mutex> l(_mu);
    LOGF_WARN("%s", _s.c_str());
    _s.clear();
  }

  void flush_as_warn_every_n(int every_n) {
    std::lock_guard<Mutex> l(_mu);
    LOGF_WARN_EVERY_N(every_n, "%s", _s.c_str());
    _s.clear();
  }

  void flush_as_error() {
    std::lock_guard<Mutex> l(_mu);
    LOGF_ERROR("%s", _s.c_str());
    _s.clear();
  }

  void flush_as_error_every_n(int every_n) {
    std::lock_guard<Mutex> l(_mu);
    LOGF_ERROR_EVERY_N(every_n, "%s", _s.c_str());
    _s.clear();
  }

  void reset() {
    std::lock_guard<Mutex> l(_mu);
    _s.clear();
  }

  std::string str() {
    std::lock_guard<Mutex> l(_mu);
    return _s;
  }

 protected:
  Mutex _mu;
  std::string _s;
};

// thread-safe
typedef LogBuilder<std::mutex> ConcurrentLogBuilder;
// non thread-safe
typedef LogBuilder<DummyMutex> UnsafeLogBuilder;

} /* namespace log */
} /* namespace cpputil */
