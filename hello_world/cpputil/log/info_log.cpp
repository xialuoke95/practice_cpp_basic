#include "info_log.h"

namespace cpputil {
namespace log {

void InfoLog::flush(int loglevel) {
  std::lock_guard<std::mutex> l(_mu);
  switch (loglevel) {
    case 1:
      LOGF_INFO("%s", _s.c_str());
      break;
    case 2:
      LOGF_WARN("%s", _s.c_str());
      break;
    default:
      LOGF_INFO("%s", _s.c_str());
  }
  _s.clear();
}

} /* namespace log */
} /* namespace cpputil */
