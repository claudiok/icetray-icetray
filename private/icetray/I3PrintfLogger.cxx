//
// $Id$
//
// @author troy d. straszheim
//
#include <icetray/I3Logging.h>
#include <icetray/I3SimpleLoggers.h>

I3PrintfLogger::I3PrintfLogger(I3LogLevel level)
    : I3Logger(level)
{
	tty_ = isatty(1);
}

void
I3PrintfLogger::Log(I3LogLevel level, const char *unit, const char *file,
    int line, const char *func, const char *format, ...)
{
	const char *log_description;
	const char *log_prolog = "", *file_prolog = "", *log_epilog = "";

	if (LogLevelForUnit(unit) > level)
		return;

	if (tty_) {
		log_prolog = "\x1b[1m";
		file_prolog = "\x1b[1m";
		log_epilog = "\x1b[0m";
	}

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
		if (tty_)
			log_prolog = "\x1b[1;31m";
		break;
	case LOG_FATAL:
		log_description = "FATAL";
		if (tty_)
			log_prolog = "\x1b[1;31m";
		break;
	default:
		log_description = "UNKNOWN";
		break;
	}

	va_list args;
	va_start(args, format);

	int messagesize = 0;
	messagesize += snprintf(NULL, 0,
	    "%s%s (%s):%s " " (%s%s:%d%s in %s%s%s)",
	    log_prolog, log_description, unit, log_epilog, file_prolog, file,
	    line, log_epilog, file_prolog, func, log_epilog);
	messagesize += vsnprintf(NULL, 0, format, args);

	char log_message[messagesize + 1];

	sprintf(log_message, "%s%s (%s):%s ", log_prolog, log_description, unit,
	    log_epilog);
	va_start(args, format);
	vsprintf(&log_message[strlen(log_message)], format, args);
	sprintf(log_message, "%s (%s%s:%d%s in %s%s%s)", log_message,
	    file_prolog, file, line, log_epilog, file_prolog, func, log_epilog);

	puts(log_message);
}

