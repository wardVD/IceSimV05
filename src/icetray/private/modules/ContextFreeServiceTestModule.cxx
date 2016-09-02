/**
 * copyright  (C) 2004
 * the icecube collaboration
 * $Id: ContextFreeServiceTestModule.cxx 2242 2015-08-21 20:20:02Z nega $
 *
 * @version $Revision: 2242 $
 * @date $Date: 2015-08-21 15:20:02 -0500 (Fri, 21 Aug 2015) $
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

struct MyService{};
static boost::shared_ptr<MyService> service_ptr_param;

struct ContextFreeServiceTestModule : I3Module
{
  ContextFreeServiceTestModule(const I3Context& context) : I3Module(context) 
  { 
    AddOutBox("OutBox");

    service_ptr_param = boost::shared_ptr<MyService>();
    AddParameter("service_ptr_param", "pointer to service.",service_ptr_param);
  }

  virtual void Configure() 
  { 
    GetParameter("service_ptr_param",service_ptr_param);
  }

  virtual void Process() { 
    I3FramePtr frame = PopFrame(); 
    PushFrame(frame, "OutBox"); 
}

  virtual void Resume() { ; }
  virtual void Finish() { ; }
  virtual void Abort() { ; }
};

I3_MODULE(ContextFreeServiceTestModule);
