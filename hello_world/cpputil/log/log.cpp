#include "log.h"

#include <unistd.h>
#include <sys/syscall.h>
#include <log4cxx/basicconfigurator.h>
#include <log4cxx/consoleappender.h>
#include <log4cxx/dailyrollingfileappender.h>
#include <log4cxx/logger.h>
#include <log4cxx/patternlayout.h>
#include <log4cxx/propertyconfigurator.h>

namespace cpputil {
namespace log {

// depredcated
int init(const std::string& module_name, int) {
    log4cxx::AppenderPtr appender(new log4cxx::DailyRollingFileAppender(
                log4cxx::LayoutPtr(new log4cxx::PatternLayout("%d{ISO8601} %5p %m%n")),
                log4cxx::LogString("/var/log/tiger/" + module_name + ".log"),
                log4cxx::LogString(".yyyy-MM-dd")));
    log4cxx::BasicConfigurator::configure(appender);
    return 0;
}

int init(int level, const std::string& file_path, int rotation, bool log_to_stderr) {
    if (!file_path.empty()) {
        // rotate hourly or daily
        std::string date_pattern = (rotation == kRotationDaily) ? ".yyyy-MM-dd" : ".yyyy-MM-dd_HH";
        log4cxx::AppenderPtr fileAppender(new log4cxx::DailyRollingFileAppender(
                    log4cxx::LayoutPtr(new log4cxx::PatternLayout("%d{ISO8601} %5p %t %m%n")),
                    log4cxx::LogString(file_path),
                    log4cxx::LogString(date_pattern)));
        log4cxx::BasicConfigurator::configure(fileAppender);
    }

    if (log_to_stderr) {
        log4cxx::AppenderPtr stderrAppender(new log4cxx::ConsoleAppender(
                    log4cxx::LayoutPtr(new log4cxx::PatternLayout("%d{ISO8601} %5p %t %m%n")),
                    "System.err"));
      log4cxx::BasicConfigurator::configure(stderrAppender);
    }

    switch (level) {
    case kLevelTrace:
      log4cxx::Logger::getRootLogger()->setLevel(log4cxx::Level::getTrace());
      break;
    case kLevelDebug:
      log4cxx::Logger::getRootLogger()->setLevel(log4cxx::Level::getDebug());
      break;
    case kLevelInfo:
      log4cxx::Logger::getRootLogger()->setLevel(log4cxx::Level::getInfo());
      break;
    case kLevelWarn:
      log4cxx::Logger::getRootLogger()->setLevel(log4cxx::Level::getWarn());
      break;
    case kLevelError:
      log4cxx::Logger::getRootLogger()->setLevel(log4cxx::Level::getError());
      break;
    case kLevelFatal:
      log4cxx::Logger::getRootLogger()->setLevel(log4cxx::Level::getFatal());
      break;
    default:
      log4cxx::Logger::getRootLogger()->setLevel(log4cxx::Level::getInfo());
      break;
    }
    return 0;
}

int init(const std::string& properties_file) {
    log4cxx::PropertyConfigurator::configure(properties_file);
    return 0;
}

log4cxx::LoggerPtr root_logger = ::log4cxx::Logger::getRootLogger();

thread_local long log_tid = syscall(SYS_gettid);

} /* namespace log */
} /* namespace cpputil */

/* vim: set ts=4 sw=4 sts=4 tw=100 */
