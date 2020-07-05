#pragma once

#include <cpputil/log/log_builder.h>

namespace cpputil {
namespace log {

class InfoLog : public LogBuilder<std::mutex> {
 public:
  // keep this interface
  using LogBuilder<std::mutex>::flush;

  // deprecate this interface because the definition of loglevel is not
  // consistent with any other libraries
  void flush(int loglevel) __attribute__((deprecated));
};

} /* namespace log */
} /* namespace cpputil */
