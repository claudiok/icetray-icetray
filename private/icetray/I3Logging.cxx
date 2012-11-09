//
// $Id$
//
// @author troy d. straszheim
//
#include <icetray/I3Logging.h>
#include <icetray/I3SimpleLoggers.h>

static I3LoggerPtr icetray_global_logger;

I3LoggerPtr
GetIcetrayLogger()
{
	if (!icetray_global_logger)
		icetray_global_logger = I3LoggerPtr(new I3PrintfLogger);

	return icetray_global_logger;
}

void
SetIcetrayLogger(I3LoggerPtr logger)
{
	//shared_ptr assignment atomic
	icetray_global_logger = logger;
}

I3Logger::I3Logger(I3LogLevel default_level) :
    default_log_level_(default_level) {}
I3Logger::~I3Logger() {}

I3LogLevel
I3Logger::LogLevelForUnit(const char *unit)
{
	std::map<std::string, I3LogLevel>::const_iterator iter =
	    log_levels_.find(unit);
	if (iter == log_levels_.end())
		return default_log_level_;

	return iter->second;
}

void
I3Logger::SetLogLevelForUnit(const char *unit, I3LogLevel level)
{
	log_levels_[unit] = level;
}

void
I3Logger::SetLogLevel(I3LogLevel level)
{
	default_log_level_ = level;
}

void
I3BasicLogger::Log(I3LogLevel level, const char *unit, const char *file,
    int line, const char *func, const char *format, ...)
{
	const char *log_description;

	if (LogLevelForUnit(unit) > level)
		return;

	switch (level) {
	case LOG_TRACE:
		log_description = "TRACE";
		break;
	case LOG_DEBUG:
		log_description = "DEBUG";
		break;
	case LOG_INFO:
		log_description = "INFO";
		break;
	case LOG_WARN:
		log_description = "WARN";
		break;
	case LOG_ERROR:
		log_description = "ERROR";
		break;
	case LOG_FATAL:
		log_description = "FATAL";
		break;
	default:
		log_description = "UNKNOWN";
		break;
	}

	va_list args;
	va_start(args, format);

	int messagesize = 0;
	messagesize += snprintf(NULL, 0, "%s (%s): " " (%s:%d in %s)",
	    log_description, unit, file, line, func);
	messagesize += vsnprintf(NULL, 0, format, args);

	char log_message[messagesize + 1];

	sprintf(log_message, "%s (%s): ", log_description, unit);
	vsprintf(&log_message[strlen(log_message)], format, args);
	sprintf(log_message, "%s (%s:%d in %s)", log_message, file, line, func);

	BasicLog(log_message);
}

