/**
    copyright  (C) 2004
    the icecube collaboration
    $Id: I3LoggingTest.cxx 15877 2006-02-16 14:00:11Z troy $

    @version $Revision: 1.2 $
    @date $Date: 2006-02-16 09:00:11 -0500 (Thu, 16 Feb 2006) $
    @author troy d. straszheim <troy@resophonic.com>
*/

#include <I3Test.h>
#include <icetray/I3Logging.h>

#include <string>
using std::string;
using std::cout;
using std::endl;

TEST_GROUP(I3LoggingTest);

TEST(one)
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


