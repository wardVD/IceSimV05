#include "recclasses/I3PortiaEvent.h"
#include "../recclasses/converter/I3PortiaEventConverter.h"
#include "wrap.h"
#include <tableio/converter/pybindings.h>
#include <boost/python.hpp>

void register_I3PortiaEvent()
{
  using namespace boost::python;

  // this uses some exceptionally nasty macros to do the registration
  // because of the getters and setters.  When a struct is just a
  // struct, don't code it as a class.

  wrap::I3FrameObject<I3PortiaEvent>()
    GETSET(I3PortiaEvent, OMKey, FirstPulseOMKey)
    GETSET(I3PortiaEvent, OMKey, FirstPulseOMKeybtw)
    GETSET(I3PortiaEvent, OMKey, LastPulseOMKey)
    GETSET(I3PortiaEvent, OMKey, LastPulseOMKeybtw)
    GETSET(I3PortiaEvent, OMKey, LargestNPEOMKey)
    GETSET(I3PortiaEvent, double, TotalAtwdNPE)
    GETSET(I3PortiaEvent, double, TotalAtwdNPEbtw)
    GETSET(I3PortiaEvent, double, TotalFadcNPE)
    GETSET(I3PortiaEvent, double, TotalFadcNPEbtw)
    GETSET(I3PortiaEvent, double, TotalBestNPE)
    GETSET(I3PortiaEvent, double, TotalBestNPEbtw)
    GETSET(I3PortiaEvent, double, TotalNch)
    GETSET(I3PortiaEvent, double, TotalNchbtw)
    GETSET(I3PortiaEvent, double, AtwdNch)
    GETSET(I3PortiaEvent, double, AtwdNchbtw)
    GETSET(I3PortiaEvent, double, FadcNch)
    GETSET(I3PortiaEvent, double, FadcNchbtw)
  ;
  
  I3CONVERTER_NAMESPACE(recclasses);
  I3CONVERTER_EXPORT_DEFAULT(I3PortiaEventConverter,
                             "Dumps PortiaEvent observable");
}
