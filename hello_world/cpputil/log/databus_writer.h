#pragma once

#include <memory>
#include <cpputil/databusclient/include/client.h>
#include <log4cxx/logstring.h>
#include <log4cxx/helpers/writer.h>

namespace cpputil {
namespace log {

using namespace log4cxx;
using namespace log4cxx::helpers;

// 向Databus写日志的log4cxx writer
class LOG4CXX_EXPORT DatabusWriter : public Writer
{
public:
    DatabusWriter(const std::string& channel_name,
            const std::string& key);
    ~DatabusWriter();

    virtual void close(Pool& p);
    virtual void flush(Pool& p);
    virtual void write(const LogString& str, Pool& p);

private:
    DatabusWriter(const DatabusWriter&);
    DatabusWriter& operator=(const DatabusWriter&);

    std::shared_ptr<DatabusCollector> databus_;
    std::shared_ptr<DatabusChannel> channel_;
    std::string key_;
};

} // namespace log
} // namespace cpputil

