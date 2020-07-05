#pragma once

#include <stdio.h>
#include <stdarg.h>
#include <string>

// 不使用glog定义的短名字
#ifndef GLOG_NO_ABBREVIATED_SEVERITIES
#define GLOG_NO_ABBREVIATED_SEVERITIES
#endif

#include <log4cxx/logger.h>

namespace cpputil {
namespace log {

enum Level {
  kLevelTrace = 0,
  kLevelDebug = 1,
  kLevelInfo = 2,
  kLevelWarn = 3,
  kLevelError = 4,
  kLevelFatal = 5
};

enum Rotation {
  kRotationDaily = 0,
  kRotationHourly = 1
};

// initialize logger using module name and default configurations
int init(const std::string& module_name, int /*dummy*/) __attribute__((deprecated));

// initialize logger using given log level and appender configuration
int init(int level, const std::string& file_path, int rotation = kRotationDaily, bool log_to_stderr = false);

// initialize logger through log4cxx properties file
int init(const std::string& properties_file);

// format the log string using C-style printf function, return the string
char* format(char* buf, size_t n, const char* fmt, ...) __attribute__((format(printf, 3, 4)));

char* vformat(char* buf, size_t n, const char* fmt, va_list vlist);

// this function has to be defined separately since __attribute__ only works on function declarations
inline char* format(char* buf, size_t n, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, n, fmt, args);
    va_end(args);
    return buf;
}

inline char* vformat(char* buf, size_t n, const char* fmt, va_list vlist) {
    vsnprintf(buf, n, fmt, vlist);
    return buf;
}

// 超长的log会被截断。DEBUG log 允许更长，线上一般不会打开 DEBUG 以免影响性能。
const int LOG_MAX_SIZE_INFO = 8192;
const int LOG_MAX_SIZE_DEBUG = 65536;

// log4cxx前向声明
// 使用 log4cxx 时，引用的全局 logger
extern log4cxx::LoggerPtr root_logger;

extern thread_local long log_tid;

} /* namespace log */
} /* namespace cpputil */

// 双打 log 宏，根据全局变量区分 log 方式
// function format() is lazy evaluated
#define LOG_INTERNAL__(logger, level, size, fmt, ...) \
do { \
    char msg_buf[size]; \
    if (logger != nullptr && logger->isEnabledFor(level)) { \
        logger->forcedLog(level, ::cpputil::log::format(msg_buf, size, fmt, ##__VA_ARGS__), LOG4CXX_LOCATION); \
    } \
} while (false)

// function vformat() is lazy evaluated
#define LOG_INTERNAL_VA__(logger, level, size, fmt, vlist) \
do { \
    char msg_buf[size]; \
    if (logger != nullptr && logger->isEnabledFor(level)) { \
        logger->forcedLog(level, ::cpputil::log::vformat(msg_buf, size, fmt, vlist), LOG4CXX_LOCATION); \
    } \
} while (false)

// n <= 0 will disable log; n > 0 will enable log (n == 1 means log every time)
#define LOG_EVERY_N_INTERNAL__(n, logger, level, size, fmt, ...) \
do { \
    if (logger != nullptr && logger->isEnabledFor(level)) { \
        char msg_buf[size]; \
        thread_local int bytelog_occurrences_mod_n__ = 0; \
        int n__ = static_cast<int>(n); \
        if (n__ > 0 && ++bytelog_occurrences_mod_n__ > n__) bytelog_occurrences_mod_n__ -= n__; \
        if (n__ > 0 && bytelog_occurrences_mod_n__ == 1) { \
            logger->forcedLog(level, ::cpputil::log::format(msg_buf, size, fmt, ##__VA_ARGS__), LOG4CXX_LOCATION); \
        } \
    } \
} while (false)

// 全局log宏
#define LOGF_TRACE(fmt, ...) LOG_INTERNAL__(::cpputil::log::root_logger, log4cxx::Level::getTrace(), ::cpputil::log::LOG_MAX_SIZE_DEBUG, fmt, ##__VA_ARGS__)
#define LOGF_DEBUG(fmt, ...) LOG_INTERNAL__(::cpputil::log::root_logger, log4cxx::Level::getDebug(), ::cpputil::log::LOG_MAX_SIZE_DEBUG, fmt, ##__VA_ARGS__)
#define LOGF_INFO( fmt, ...) LOG_INTERNAL__(::cpputil::log::root_logger, log4cxx::Level::getInfo(),  ::cpputil::log::LOG_MAX_SIZE_INFO,  fmt, ##__VA_ARGS__)
#define LOGF_WARN( fmt, ...) LOG_INTERNAL__(::cpputil::log::root_logger, log4cxx::Level::getWarn(),  ::cpputil::log::LOG_MAX_SIZE_INFO,  fmt, ##__VA_ARGS__)
#define LOGF_ERROR(fmt, ...) LOG_INTERNAL__(::cpputil::log::root_logger, log4cxx::Level::getError(), ::cpputil::log::LOG_MAX_SIZE_INFO,  fmt, ##__VA_ARGS__)
#define LOGF_FATAL(fmt, ...) LOG_INTERNAL__(::cpputil::log::root_logger, log4cxx::Level::getFatal(), ::cpputil::log::LOG_MAX_SIZE_INFO,  fmt, ##__VA_ARGS__)

#define LOGF_TRACE_EVERY_N(n, fmt, ...) LOG_EVERY_N_INTERNAL__(n, ::cpputil::log::root_logger, log4cxx::Level::getTrace(), ::cpputil::log::LOG_MAX_SIZE_DEBUG, fmt, ##__VA_ARGS__)
#define LOGF_DEBUG_EVERY_N(n, fmt, ...) LOG_EVERY_N_INTERNAL__(n, ::cpputil::log::root_logger, log4cxx::Level::getDebug(), ::cpputil::log::LOG_MAX_SIZE_DEBUG, fmt, ##__VA_ARGS__)
#define LOGF_INFO_EVERY_N( n, fmt, ...) LOG_EVERY_N_INTERNAL__(n, ::cpputil::log::root_logger, log4cxx::Level::getInfo(),  ::cpputil::log::LOG_MAX_SIZE_INFO,  fmt, ##__VA_ARGS__)
#define LOGF_WARN_EVERY_N( n, fmt, ...) LOG_EVERY_N_INTERNAL__(n, ::cpputil::log::root_logger, log4cxx::Level::getWarn(),  ::cpputil::log::LOG_MAX_SIZE_INFO,  fmt, ##__VA_ARGS__)
#define LOGF_ERROR_EVERY_N(n, fmt, ...) LOG_EVERY_N_INTERNAL__(n, ::cpputil::log::root_logger, log4cxx::Level::getError(), ::cpputil::log::LOG_MAX_SIZE_INFO,  fmt, ##__VA_ARGS__)
#define LOGF_FATAL_EVERY_N(n, fmt, ...) LOG_EVERY_N_INTERNAL__(n, ::cpputil::log::root_logger, log4cxx::Level::getFatal(), ::cpputil::log::LOG_MAX_SIZE_INFO,  fmt, ##__VA_ARGS__)

// 全局log宏 valist版本
#define LOGV_TRACE(fmt, vlist) LOG_INTERNAL_VA__(::cpputil::log::root_logger, log4cxx::Level::getTrace(), ::cpputil::log::LOG_MAX_SIZE_DEBUG, fmt, vlist)
#define LOGV_DEBUG(fmt, vlist) LOG_INTERNAL_VA__(::cpputil::log::root_logger, log4cxx::Level::getDebug(), ::cpputil::log::LOG_MAX_SIZE_DEBUG, fmt, vlist)
#define LOGV_INFO( fmt, vlist) LOG_INTERNAL_VA__(::cpputil::log::root_logger, log4cxx::Level::getInfo(),  ::cpputil::log::LOG_MAX_SIZE_INFO,  fmt, vlist)
#define LOGV_WARN( fmt, vlist) LOG_INTERNAL_VA__(::cpputil::log::root_logger, log4cxx::Level::getWarn(),  ::cpputil::log::LOG_MAX_SIZE_INFO,  fmt, vlist)
#define LOGV_ERROR(fmt, vlist) LOG_INTERNAL_VA__(::cpputil::log::root_logger, log4cxx::Level::getError(), ::cpputil::log::LOG_MAX_SIZE_INFO,  fmt, vlist)
#define LOGV_FATAL(fmt, vlist) LOG_INTERNAL_VA__(::cpputil::log::root_logger, log4cxx::Level::getFatal(), ::cpputil::log::LOG_MAX_SIZE_INFO,  fmt, vlist)

// 局部log宏，以便通过log4cxx的配置文件分别定制，glog 不支持
#define DEFINE_LOGGER(name) log4cxx::LoggerPtr __logger_##name = ::log4cxx::Logger::getLogger(#name)
#define DECLARE_LOGGER(name) extern log4cxx::LoggerPtr __logger_##name
#define GET_LOGGER(name) __logger_##name

#define LOGGER_TRACE(name, fmt, ...) LOG_INTERNAL__(__logger_##name, log4cxx::Level::getTrace(), ::cpputil::log::LOG_MAX_SIZE_DEBUG, fmt, ##__VA_ARGS__)
#define LOGGER_DEBUG(name, fmt, ...) LOG_INTERNAL__(__logger_##name, log4cxx::Level::getDebug(), ::cpputil::log::LOG_MAX_SIZE_DEBUG, fmt, ##__VA_ARGS__)
#define LOGGER_INFO( name, fmt, ...) LOG_INTERNAL__(__logger_##name, log4cxx::Level::getInfo(),  ::cpputil::log::LOG_MAX_SIZE_INFO,  fmt, ##__VA_ARGS__)
#define LOGGER_WARN( name, fmt, ...) LOG_INTERNAL__(__logger_##name, log4cxx::Level::getWarn(),  ::cpputil::log::LOG_MAX_SIZE_INFO,  fmt, ##__VA_ARGS__)
#define LOGGER_ERROR(name, fmt, ...) LOG_INTERNAL__(__logger_##name, log4cxx::Level::getError(), ::cpputil::log::LOG_MAX_SIZE_INFO,  fmt, ##__VA_ARGS__)
#define LOGGER_FATAL(name, fmt, ...) LOG_INTERNAL__(__logger_##name, log4cxx::Level::getFatal(), ::cpputil::log::LOG_MAX_SIZE_INFO,  fmt, ##__VA_ARGS__)

// 缩写的log宏，有可能会和别的库冲突，因此设置控制开关。使用 log4cxx 时，默认打开
#if !defined(LOG_NO_ABBREVIATION) && !defined(LOG_USE_ABBREVIATION)
#define LOG_USE_ABBREVIATION
#endif

// 缩写的log宏，有可能会和别的库冲突，因此设置控制开关
#ifdef LOG_USE_ABBREVIATION

#define TRACE(fmt, ...) LOGF_TRACE(fmt, ##__VA_ARGS__)
#define DEBUG(fmt, ...) LOGF_DEBUG(fmt, ##__VA_ARGS__)
#define INFO( fmt, ...) LOGF_INFO( fmt, ##__VA_ARGS__)
#define WARN( fmt, ...) LOGF_WARN( fmt, ##__VA_ARGS__)
#define ERROR(fmt, ...) LOGF_ERROR(fmt, ##__VA_ARGS__)
#define FATAL(fmt, ...) LOGF_FATAL(fmt, ##__VA_ARGS__)

// 全局log宏 valist版本
#define VTRACE(fmt, vlist) LOGV_TRACE(fmt, vlist)
#define VDEBUG(fmt, vlist) LOGV_DEBUG(fmt, vlist)
#define VINFO( fmt, vlist) LOGV_INFO( fmt, vlist)
#define VWARN( fmt, vlist) LOGV_WARN( fmt, vlist)
#define VERROR(fmt, vlist) LOGV_ERROR(fmt, vlist)
#define VFATAL(fmt, vlist) LOGV_FATAL(fmt, vlist)

// 兼容性支持
#define NOTICE INFO
#define VNOTICE VINFO

#endif // LOG_USE_ABBREVIATION

