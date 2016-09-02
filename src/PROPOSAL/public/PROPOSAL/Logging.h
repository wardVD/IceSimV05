
#ifndef LOGGING_H_
#define LOGGING_H_

#ifdef USE_ICETRAY_LOGGING
#include <icetray/I3Logging.h>

#define plog_fatal(msg)
#define plog_error(msg)  log_error_stream(msg)
#define plog_warn(msg)   log_warn_stream(msg)
#define plog_notice(msg) log_notice_stream(msg)
#define plog_info(msg)   log_info_stream(msg)
#define plog_debug(msg)  log_debug_stream(msg)
#define plog_trace(msg)  log_trace_stream(msg)

#else

#define plog_fatal(msg)
#define plog_error(msg)  std::cout << "ERROR: " << msg << std::endl
#define plog_warn(msg)   std::cout << "WARN: " << msg << std::endl
#define plog_notice(msg) std::cout << "NOTICE: " << msg << std::endl
#define plog_info(msg)   std::cout << "INFO: " << msg << std::endl
#define plog_debug(msg)  std::cout << "DEBUG: " << msg << std::endl
#define plog_trace(msg)  std::cout << "TRACE: " << msg << std::endl

#endif

#endif

