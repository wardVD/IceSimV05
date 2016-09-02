#include "genie-icetray/I3LoggingAppender.h"

#include "icetray/I3Logging.h"

#include <sstream>

using namespace log4cpp;

I3LoggingAppender::I3LoggingAppender(const std::string& name,
                                     bool stepDownLoggingLevels)
: LayoutAppender(name),
stepDownLoggingLevels_(stepDownLoggingLevels)
{
    
}

I3LoggingAppender::~I3LoggingAppender() 
{
    close();
}


bool I3LoggingAppender::reopen()
{
    return true;
}

void I3LoggingAppender::close()
{
    // empty
}

void I3LoggingAppender::_append(const LoggingEvent& event)
{
    std::ostringstream ostream;
    ostream << _getLayout().format(event);

    const std::string prefix = "GENIE said: ";
    
    switch (event.priority)
    {
        //case Priority::EMERG:
        case Priority::FATAL:
            log_fatal("%s%s", prefix.c_str(), ostream.str().c_str());
            break;
        case Priority::ALERT:
        case Priority::CRIT:
        case Priority::ERROR:
        {
            if (stepDownLoggingLevels_)
                log_warn("%s%s", prefix.c_str(), ostream.str().c_str());
            else
                log_error("%s%s", prefix.c_str(), ostream.str().c_str());
            break;
        }
        case Priority::WARN:
        {
            if (stepDownLoggingLevels_)
                log_info("%s%s", prefix.c_str(), ostream.str().c_str());
            else
                log_warn("%s%s", prefix.c_str(), ostream.str().c_str());
            break;
        }
        case Priority::NOTICE:
        case Priority::INFO:
        {
            if (stepDownLoggingLevels_)
                log_debug("%s%s", prefix.c_str(), ostream.str().c_str());
            else
                log_info("%s%s", prefix.c_str(), ostream.str().c_str());
            break;
        }
        case Priority::DEBUG:
        {
            if (stepDownLoggingLevels_)
                log_trace("%s%s", prefix.c_str(), ostream.str().c_str());
            else
                log_debug("%s%s", prefix.c_str(), ostream.str().c_str());
            break;
        }
        case Priority::NOTSET:
        default:
            log_warn("%s%s", prefix.c_str(), ostream.str().c_str());
            break;
    }
}


