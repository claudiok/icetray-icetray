//
// $Id: I3Logging.cxx 50381 2008-11-07 13:50:33Z troy $
//
// @author troy d. straszheim
//
#include <icetray/I3Logging.h>

#if !defined(I3_PRINTF_LOGGING) && !defined(I3_PYTHON_LOGGING)

#include <string>
using std::string;
#include <iostream>
using std::cerr;

#include <log4cplus/configurator.h>
#include <log4cplus/consoleappender.h>
#include <log4cplus/layout.h>
#include <log4cplus/logger.h>
#include <log4cplus/loglevel.h>
#include <log4cplus/ndc.h>
#include <log4cplus/helpers/property.h>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

namespace fs = boost::filesystem;

// 
//  Used only in this file to announce configuration steps
//
#define LOG(MSG) ::log4cplus::Logger::getRoot().log(::log4cplus::DEBUG_LOG_LEVEL,     \
                                                    MSG,__FILE__,__LINE__);

namespace I3Logging 
{
  namespace detail
  {
    void failsafe_configuration()
    {
      log4cplus::helpers::Properties properties;
  
      // here's what you get if you use all-default configuration
      properties.setProperty("log4cplus.appender.default", "log4cplus::ConsoleAppender");
      properties.setProperty("log4cplus.appender.default.layout", "log4cplus::PatternLayout");
      properties.setProperty("log4cplus.appender.default.layout.ConversionPattern", "%F:%L: %m%n");
      //properties.setProperty("log4cplus.logger.default", "DEBUG, default");
  
      // no appender for root logger. it duplicates output.
      properties.setProperty("log4cplus.rootLogger", "WARN, default");
  
      log4cplus::PropertyConfigurator configurator(properties);
      configurator.configure();
      cerr << "Default logging configured (no log4cplus.conf is present).  Default threshold is WARN.\n";
    }
  
    void file_configuration(const string& propertyFile)
    {
      log4cplus::PropertyConfigurator configurator(propertyFile);
      configurator.configure();
      cerr << "Logging configured from file " << propertyFile << "\n";
    }
  }

  Configurator::Configurator() 
  {    
    // try the config file that I3LOGGING_CONFIG is pointing to
    const char* buffer = getenv("I3LOGGING_CONFIG");
    if (buffer)
    {
      fs::path propertyFile(buffer);
      if (!fs::exists(propertyFile) || fs::is_directory(propertyFile))
	{
	  cerr << "Environment variable I3LOGGING_CONFIG which should point\n"
	       << "to a log4cplus configuration file points to a directory or\n"
	       << "something nonexistent.\n"
	       << "value is \"" << buffer << "\".\n"
	       << "Going to default configuration...\n";
	  detail::failsafe_configuration();
	  return;
	}
      detail::file_configuration(propertyFile.string());
      return;
    }
    
    // try the local config file
    {
      fs::path propertyFile("./log4cplus.conf");
      if (fs::exists(propertyFile) && !fs::is_directory(propertyFile))
      {
        detail::file_configuration(propertyFile.string());
        return;
      }
    }
    
    // try the local file within the workspace
    buffer = getenv("I3_BUILD");
    if(buffer)
    {
      fs::path propertyFile(buffer);
      propertyFile /= "log4cplus.conf";
      if(fs::exists(propertyFile) && !fs::is_directory(propertyFile))
      {
        detail::file_configuration(propertyFile.string());
        return;
      }
    }

    detail::failsafe_configuration();
    LOG("Logging configured.");
  }
  
#ifdef I3_ONLINE
  boost::mutex Log4CPlusLogger::mtx_;
#endif
  char Log4CPlusLogger::buffer_[Log4CPlusLogger::BUFFER_SIZE];
}

I3Logging::Log4CPlusLogger get_logger() 
{
  // this gets constructed only the first time this function is called
  static I3Logging::Configurator configurator;
   
  static I3Logging::Log4CPlusLogger global_logger;
  
  return global_logger;
}

#include <stdio.h>

extern "C"
{
  void i3_clog_trace(const char* loggername, const char* msg)
  {
    log4cplus::Logger logger(log4cplus::Logger::getInstance(loggername));
    logger.log(::log4cplus::TRACE_LOG_LEVEL, msg);
  };
  void i3_clog_debug(const char* loggername, const char* msg)
  {
    log4cplus::Logger logger(log4cplus::Logger::getInstance(loggername));
    logger.log(::log4cplus::DEBUG_LOG_LEVEL, msg);
  };
  void i3_clog_info(const char* loggername, const char* msg)
  {
    log4cplus::Logger logger(log4cplus::Logger::getInstance(loggername));
    logger.log(::log4cplus::INFO_LOG_LEVEL, msg);
  };
  void i3_clog_warn(const char* loggername, const char* msg)
  {
    log4cplus::Logger logger(log4cplus::Logger::getInstance(loggername));
    logger.log(::log4cplus::WARN_LOG_LEVEL, msg);
  };
  void i3_clog_error(const char* loggername, const char* msg)
  {
    log4cplus::Logger logger(log4cplus::Logger::getInstance(loggername));
    logger.log(::log4cplus::ERROR_LOG_LEVEL, msg);
  };
  void i3_clog_fatal(const char* loggername, const char* msg)
  {
    log4cplus::Logger logger(log4cplus::Logger::getInstance(loggername));
    logger.log(::log4cplus::FATAL_LOG_LEVEL, msg);
  };
}

#else // !defined(I3_PRINTF_LOGGING) && !defined(I3_PYTHON_LOGGING)

#include <stdio.h>

extern "C" {
  void i3_clog_trace(const char* loggername, const char* msg)
  {
    printf(msg);
  };
  void i3_clog_debug(const char* loggername, const char* msg)
  {
    printf(msg);
  };
  void i3_clog_info(const char* loggername, const char* msg)
  {
    printf(msg);
  };
  void i3_clog_warn(const char* loggername, const char* msg)
  {
    printf(msg);
  };
  void i3_clog_error(const char* loggername, const char* msg)
  {
    printf(msg);
  };
  void i3_clog_fatal(const char* loggername, const char* msg)
  {
    printf(msg);
  };
}

#endif
