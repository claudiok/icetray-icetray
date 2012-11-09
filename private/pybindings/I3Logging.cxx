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

void register_I3Logging()
{
	class_<I3Logger, boost::shared_ptr<I3Logger>, boost::noncopyable>
	    ("I3Logger", "Logging base class", no_init)
		.add_static_property("global_logger", &GetIcetrayLogger, &SetIcetrayLogger)
	;

}


