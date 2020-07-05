#pragma once

#include <log4cxx/logger.h>
#include <log4cxx/logstring.h>
#include <log4cxx/writerappender.h>
#include <log4cxx/helpers/pool.h>

namespace cpputil {
namespace log {

using namespace log4cxx;
using namespace log4cxx::helpers;

// 向Databus写日志的log4cxx appender
class LOG4CXX_EXPORT DatabusAppender : public WriterAppender
{
protected:
    LogString channel_name_;
    LogString key_;

public:
    DECLARE_LOG4CXX_OBJECT(DatabusAppender)
    BEGIN_LOG4CXX_CAST_MAP()
        LOG4CXX_CAST_ENTRY(DatabusAppender)
        LOG4CXX_CAST_ENTRY_CHAIN(WriterAppender)
    END_LOG4CXX_CAST_MAP()

    /**
      The default constructor does not do anything.
      */
    DatabusAppender();

    DatabusAppender(const LayoutPtr& layout, const LogString& channel_name,
            const LogString& key);

    ~DatabusAppender();

    /**
      <p>Note: Actual opening of the file is made when
#activateOptions is called, not when the options are set.  */
    virtual void setChannel(const LogString& channel_name);

    /** Returns the value of the <b>File</b> option. */
    inline LogString getChannel() const { return channel_name_; }

    /**
      <p>Note: Actual opening of the file is made when
#activateOptions is called, not when the options are set.  */
    virtual void setKey(const LogString& key);

    /** Returns the value of the <b>File</b> option. */
    inline LogString getKey() const { return key_; }

    /**
      <p>Sets and <i>opens</i> the file where the log output will
      go. The specified file must be writable.

      <p>If there was already an opened file, then the previous file
      is closed first.*/
    void activateOptions(log4cxx::helpers::Pool& p);
    void setOption(const LogString& option,
            const LogString& value);

private:
    DatabusAppender(const DatabusAppender&);
    DatabusAppender& operator=(const DatabusAppender&);

}; // class DatabusAppender

LOG4CXX_PTR_DEF(DatabusAppender);

}  // namespace log
}  // namespace cpputil

