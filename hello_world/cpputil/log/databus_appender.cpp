#include "databus_appender.h"

#include <log4cxx/logstring.h>
#include <log4cxx/fileappender.h>
#include <log4cxx/helpers/stringhelper.h>
#include <log4cxx/helpers/loglog.h>
#include <log4cxx/helpers/optionconverter.h>
#include <log4cxx/helpers/synchronized.h>
#include <log4cxx/helpers/pool.h>
#include <log4cxx/helpers/fileoutputstream.h>
#include <log4cxx/helpers/outputstreamwriter.h>
#include <log4cxx/helpers/bufferedwriter.h>
#include <log4cxx/helpers/bytebuffer.h>
#include <log4cxx/helpers/synchronized.h>
#include "databus_writer.h"

using namespace log4cxx;
using namespace log4cxx::helpers;
using namespace log4cxx::spi;
using namespace cpputil::log;

IMPLEMENT_LOG4CXX_OBJECT(DatabusAppender)

DatabusAppender::DatabusAppender() {
    synchronized sync(mutex);
}

DatabusAppender::DatabusAppender(const LayoutPtr& layout, const LogString& channel_name,
        const LogString& key)
: WriterAppender(layout) {
    {
        synchronized sync(mutex);
        channel_name_ = channel_name;
        key_ = key;
    }
    Pool p;
    activateOptions(p);
}


DatabusAppender::~DatabusAppender() {
    finalize();
}

void DatabusAppender::setChannel(const LogString& channel_name) {
    synchronized sync(mutex);
    channel_name_ = channel_name;
}

void DatabusAppender::setKey(const LogString& key) {
    synchronized sync(mutex);
    key_ = key;
}

void DatabusAppender::setOption(const LogString& option, const LogString& value)
{
    if (StringHelper::equalsIgnoreCase(option, LOG4CXX_STR("CHANNEL"), LOG4CXX_STR("channel"))) {
        synchronized sync(mutex);
        channel_name_ = value;
    } else {
        WriterAppender::setOption(option, value);
    }
}

void DatabusAppender::activateOptions(Pool& p)
{
    synchronized sync(mutex);
    int errors = 0;
    if (!channel_name_.empty()) {
        closeWriter();
        WriterPtr newWriter(new DatabusWriter(channel_name_, key_));
        setWriter(newWriter);
    } else {
        errors++;
        LogLog::error(LogString(LOG4CXX_STR("Channel option not set for appender ["))
                +  name + LOG4CXX_STR("]."));
        LogLog::warn(LOG4CXX_STR("Are you using DatabusAppender instead of ConsoleAppender?"));
    }
    if(errors == 0) {
        WriterAppender::activateOptions(p);
    }
}

