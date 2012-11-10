/**
 *  Copyright (C) 2012
 *  Nathan Whitehorn <nwhitehorn@icecube.wisc.edu>
 *  and the IceCube Collaboration <http://www.icecube.wisc.edu>
 *  
 */

#include <boost/python.hpp>
#include <boost/preprocessor.hpp>

#include <icetray/I3Logging.h>

using namespace boost::python;
 
struct I3LoggerWrapper : public I3Logger, public wrapper<I3Logger> {
	void Log(I3LogLevel level, const char *unit, const char *file,
	    int line, const char *func, const char *format, ...)
	{
		va_list args;
		
		va_start(args, format);
		int messagesize = vsnprintf(NULL, 0, format, args);
		
		char log_message[messagesize + 1];
		va_start(args, format);
		vsprintf(log_message, format, args);
		
		if (override f = this->get_override("log")) {
			f(level, unit, file, line, func, std::string(log_message));
		} else {
			PyErr_SetString(PyExc_NotImplementedError, "I3LoggerBase subclasses must implement log()");
			throw error_already_set();
		}
	}
	
	I3LogLevel LogLevelForUnit(const char *unit)
	{
		if (override f = this->get_override("getLevelForUnit")) {
			return f(unit);
		} else {
			return I3Logger::LogLevelForUnit(unit);
		}
	}
	
	void SetLogLevelForUnit(const char *unit, I3LogLevel level)
	{
		if (override f = this->get_override("setLevelForUnit")) {
			f(unit, level);
		} else {
			I3Logger::SetLogLevelForUnit(unit, level);
		}
	}

	void SetLogLevel(I3LogLevel level)
	{
		if (override f = this->get_override("setLevel")) {
			f(level);
		} else {
			I3Logger::SetLogLevel(level);
		}
	}
};

void register_I3Logging()
{
	class_<I3Logger, boost::shared_ptr<I3Logger>, boost::noncopyable>
	    ("I3Logger", "Logging base class", no_init)
		.add_static_property("global_logger", &GetIcetrayLogger, &SetIcetrayLogger)
		.def("getLevelForUnit", &I3Logger::LogLevelForUnit)
		.def("setLevelForUnit", &I3Logger::SetLogLevelForUnit)
		.def("setLevel", &I3Logger::SetLogLevel)
	;
	
	enum_<I3LogLevel>("I3LogLevel")
		.value("LOG_TRACE", LOG_TRACE)
		.value("LOG_DEBUG", LOG_DEBUG)
		.value("LOG_INFO",  LOG_INFO)
		.value("LOG_WARN",  LOG_WARN)
		.value("LOG_ERROR", LOG_ERROR)
		.value("LOG_FATAL", LOG_FATAL)
	;
	
	class_<I3LoggerWrapper, boost::shared_ptr<I3LoggerWrapper>, boost::noncopyable>
	    ("I3LoggerBase", "Base class for Python-side logging implementations")
		.def("getLevelForUnit", &I3LoggerWrapper::LogLevelForUnit)
		.def("setLevelForUnit", &I3LoggerWrapper::SetLogLevelForUnit)
		.def("setLevel", &I3LoggerWrapper::SetLogLevel)
	;

}


