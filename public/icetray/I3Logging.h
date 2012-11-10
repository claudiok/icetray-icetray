/**
 *    $Id$
 *
 *    Copyright (C) 2007   Troy D. Straszheim  <troy@icecube.umd.edu>
 *    and the IceCube Collaboration <http://www.icecube.wisc.edu>
 *    
 *    This file is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 3 of the License, or
 *    (at your option) any later version.
 *    
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *    
 *    You should have received a copy of the GNU General Public License
 *    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef ICETRAY_I3LOGGING_H_INCLUDED
#define ICETRAY_I3LOGGING_H_INCLUDED

typedef enum {
	LOG_TRACE,
	LOG_DEBUG,
	LOG_INFO,
	LOG_WARN,
	LOG_ERROR,
	LOG_FATAL
} I3LogLevel;

#if defined(__cplusplus)

#include <icetray/I3PointerTypedefs.h>
#include <stdexcept>
#include <map>

class I3Logger {
public:
	I3Logger(I3LogLevel default_level = LOG_INFO);
	virtual ~I3Logger();

	virtual void Log(I3LogLevel level, const char *unit, const char *file,
	    int line, const char *func, const char *format, ...) = 0;

	virtual I3LogLevel LogLevelForUnit(const char *unit);
	virtual void SetLogLevelForUnit(const char *unit, I3LogLevel level);

	virtual void SetLogLevel(I3LogLevel level);
private:
	std::map<std::string, I3LogLevel> log_levels_;
	I3LogLevel default_log_level_;
};

class I3BasicLogger : public I3Logger {
public:
	I3BasicLogger(I3LogLevel default_level = LOG_INFO);

	virtual void Log(I3LogLevel level, const char *unit, const char *file,
	    int line, const char *func, const char *format, ...);
	virtual void BasicLog(const char *string) = 0;
};

I3_POINTER_TYPEDEFS(I3Logger);

// Root logger. If class/namespace has a method with the same name in scope,
// it can have its own logger.
I3LoggerPtr GetIcetrayLogger();
void SetIcetrayLogger(I3LoggerPtr);

#define I3_LOGGER GetIcetrayLogger()->Log
#else // __cplusplus
void i3_clogger(I3LogLevel level, const char *unit, const char *file,
	    int line, const char *func, const char *format, ...);
#define I3_LOGGER i3_clogger
#endif

#define SET_LOGGER(X) \
	static inline const char *__icetray_logger_id(void) { return X; }

// Set default logger in global namespace
SET_LOGGER("icetray");

#ifndef NDEBUG
#define log_trace(format, ...) I3_LOGGER(LOG_TRACE, \
    __icetray_logger_id(), __FILE__, __LINE__, __PRETTY_FUNCTION__, format, \
    ##__VA_ARGS__)
#define log_debug(format, ...) I3_LOGGER(LOG_DEBUG, \
    __icetray_logger_id(), __FILE__, __LINE__, __PRETTY_FUNCTION__, format, \
    ##__VA_ARGS__)
#define log_info(format, ...) I3_LOGGER(LOG_INFO, \
    __icetray_logger_id(), __FILE__, __LINE__, __PRETTY_FUNCTION__, format, \
    ##__VA_ARGS__)
#else
#define log_trace(format, ...)
#define log_debug(format, ...)
#define log_info(format, ...)
#endif

#define log_warn(format, ...) I3_LOGGER(LOG_WARN, \
    __icetray_logger_id(), __FILE__, __LINE__, __PRETTY_FUNCTION__, format, \
    ##__VA_ARGS__)
#define log_error(format, ...) I3_LOGGER(LOG_ERROR, \
    __icetray_logger_id(), __FILE__, __LINE__, __PRETTY_FUNCTION__, format, \
    ##__VA_ARGS__)

#ifdef __cplusplus
#define log_fatal(format, ...) I3_LOGGER(LOG_FATAL, \
    __icetray_logger_id(), __FILE__, __LINE__, __PRETTY_FUNCTION__, format, \
    ##__VA_ARGS__), throw std::runtime_error("log_fatal")
#else
#define log_fatal(format, ...) I3_LOGGER(LOG_FATAL, \
    __icetray_logger_id(), __FILE__, __LINE__, __PRETTY_FUNCTION__, format, \
    ##__VA_ARGS__), exit(1)
#endif

#endif //ifndef ICETRAY_I3LOGGING_H_INCLUDED
