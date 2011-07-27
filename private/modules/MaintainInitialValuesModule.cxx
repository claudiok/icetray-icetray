/**
 * copyright  (C) 2004
 * the icecube collaboration
 * $Id: ServicesAtDestruction.cxx 5468 2005-03-30 14:48:26Z pretz $
 *
 * @version $Revision: 1.3 $
 * @date $Date: 2005-03-30 16:48:26 +0200 (Wed, 30 Mar 2005) $
 * @author troy d. straszheim
 *
 * This tests that the global GetService<> works; that the underlying
 * context-switching is done correctly.
 */

#include <icetray/I3Tray.h>
#include <icetray/I3Frame.h>
#include <icetray/I3Module.h>
#include <icetray/OMKey.h>
#include <boost/assign/std/vector.hpp>

using namespace boost::assign;

static bool bool_param;
static unsigned char uchar_param;
static int int_param;
static long long_param;
static double double_param;
static std::string string_param;
static OMKey omkey_param;

struct MyService{};
static shared_ptr<MyService> service_ptr_param;

struct MaintainInitialValuesModule : I3Module
{
  MaintainInitialValuesModule(const I3Context& context) : I3Module(context) 
  { 
    AddOutBox("OutBox");

    bool_param = true;
    AddParameter("bool_param", "description of bool", bool_param);

    uchar_param = std::numeric_limits<unsigned char>::max();
    AddParameter("uchar_param", "description of uchar", uchar_param);

    int_param = std::numeric_limits<int>::max();
    AddParameter("int_param", "description of int", int_param);

    long_param = std::numeric_limits<long>::max();
    AddParameter("long_param", "description of long", long_param);

    double_param = 3.1415926535897932;
    AddParameter("double_param", "description of double", double_param);

    string_param = "We can't stop here.  This is Bat Country!";
    AddParameter("string_param", "description of string", string_param);
    
    omkey_param = OMKey(-666,666);
    AddParameter("omkey_param", "OMKey!", omkey_param);

    service_ptr_param = shared_ptr<MyService>( new MyService);
    AddParameter("service_ptr_param", "pointer to service.",service_ptr_param);
  }

  virtual void Configure() 
  { 
    GetParameter("bool_param", bool_param);
    GetParameter("uchar_param", uchar_param);
    GetParameter("int_param", int_param);
    GetParameter("long_param", long_param);
    GetParameter("double_param", double_param);
    GetParameter("string_param", string_param);
    GetParameter("omkey_param", omkey_param);
    GetParameter("service_ptr_param",service_ptr_param);

    assert( bool_param == true);
    assert( uchar_param == std::numeric_limits<unsigned char>::max() );
    assert( int_param == std::numeric_limits<int>::max());
    assert( long_param == std::numeric_limits<long>::max());
    assert( double_param == 3.1415926535897932);
    assert( string_param == "We can't stop here.  This is Bat Country!");
    assert( omkey_param == OMKey(-666,666));
    assert( service_ptr_param ); 
  }

  virtual void Reconfigure() { ; }

  virtual void Process() { 
    I3FramePtr frame = PopFrame(); 
    PushFrame(frame, "OutBox"); 
  }

  virtual void Resume() { ; }
  virtual void Finish() { ; }
  virtual void Abort() { ; }
};

I3_MODULE(MaintainInitialValuesModule);