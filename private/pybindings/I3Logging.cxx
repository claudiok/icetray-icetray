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
 
class I3LoggerWrapper : public I3Logger, public wrapper<I3Logger> {
public:
	void Log(I3LogLevel level, const std::string &unit,
	    const std::string &file, int line, const std::string &func,
	    const std::string &log_message)
	{
		if (override f = this->get_override("log")) {
			f(level, unit, file, line, func, log_message);
		} else {
			PyErr_SetString(PyExc_NotImplementedError,
			    "I3LoggerBase subclasses must implement log()");
			throw error_already_set();
		}
	}
	
	I3LogLevel LogLevelForUnit(const std::string &unit)
	{
		if (override f = this->get_override("getLevelForUnit")) {
			return f(unit);
		} else {
			return I3Logger::LogLevelForUnit(unit);
		}
	}
	
	void SetLogLevelForUnit(const std::string &unit, I3LogLevel level)
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
	enum_<I3LogLevel>("I3LogLevel")
		.value("LOG_TRACE", LOG_TRACE)
		.value("LOG_DEBUG", LOG_DEBUG)
		.value("LOG_INFO",  LOG_INFO)
		.value("LOG_WARN",  LOG_WARN)
		.value("LOG_ERROR", LOG_ERROR)
		.value("LOG_FATAL", LOG_FATAL)
	;

	class_<I3Logger, boost::shared_ptr<I3Logger>, boost::noncopyable>("I3LoggerBase", "C++ logging abstract base class", no_init);
	class_<I3LoggerWrapper, boost::shared_ptr<I3LoggerWrapper>, boost::noncopyable>
	    ("I3Logger", "Logging base class")
		.add_static_property("global_logger", &GetIcetrayLogger, &SetIcetrayLogger)
		.def("getLevelForUnit", &I3Logger::LogLevelForUnit)
		.def("setLevelForUnit", &I3Logger::SetLogLevelForUnit)
		.def("setLevel", &I3Logger::SetLogLevel)
	;
}


