/**
    copyright  (C) 2004
    the icecube collaboration
    $Id$

    @version $Revision$
    @date $Date$
    @author troy d. straszheim <troy@resophonic.com>
*/

#include <I3Test.h>
#include <icetray/I3Logging.h>

#include <string>
using std::string;
using std::cout;
using std::endl;

#include <boost/thread.hpp>
#include <boost/python.hpp>
#include <boost/foreach.hpp>

#include "icetray/python/gil_holder.hpp"

struct logger {
	int id_;
	logger(int i) : id_(i) {}
	void operator()()
	{
		for (int i=0; i < 3; i++) {
			log_info("hello from thread %d", id_);
		}
	}
};

struct restore_logger {
	restore_logger()
	{
		logger_ = GetIcetrayLogger();
	}
	~restore_logger()
	{
		SetIcetrayLogger(logger_);
	}
	I3LoggerPtr logger_;
};

TEST_GROUP(I3LoggingTest2);
TEST(two)
{
  log_trace("here's a trace message");
  log_debug("here's a debug message");
  log_info("here's an info message");
  log_warn("here's a warn message");
  log_error("here's an error message");
  try {
    log_fatal("here's a fatal message");
  } catch (std::exception& e) {
    // we should be here
  } 
}

TEST(threaded_default)
{
	logger l(0);
	boost::thread thready(l);
	thready.join();
}

TEST(threaded_console)
{
	using namespace boost::python;
	
	restore_logger reset;
	object logging = import("icecube.icetray.i3logging");
	logging.attr("console")();
	
	std::vector<boost::thread*> threads;
	for (int i=0; i < 5; i++)
		threads.push_back(new boost::thread(logger(i)));
	
	boost::python::detail::gil_releaser unlock;
	BOOST_FOREACH(boost::thread *t, threads) {
		ENSURE(t->timed_join(boost::posix_time::seconds(5)), "Thread does not lock");
		delete t;
	}
}
