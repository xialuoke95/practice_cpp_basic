#include "databus_writer.h"

#include <stdio.h>
#include <log4cxx/helpers/transcoder.h>
#if !defined(LOG4CXX)
#define LOG4CXX 1
#endif
#include <log4cxx/private/log4cxx_private.h>

using namespace log4cxx;
using namespace log4cxx::helpers;
using namespace cpputil::log;

DatabusWriter::DatabusWriter(const std::string& channel_name,
        const std::string& key)
: key_(key) {
    databus_ = std::make_shared<DatabusCollector>();
    channel_ = databus_->get_channel(channel_name.c_str());
}

DatabusWriter::~DatabusWriter() {
}

void DatabusWriter::close(Pool& /* p */ ) {
}

void DatabusWriter::flush(Pool& /* p */ ) {
}

void DatabusWriter::write(const LogString& str, Pool& /* p */ ) {
    if (key_.empty()) {
        channel_->send(str, 1);
    } else {
        channel_->send(str, key_, 1);
    }
}

