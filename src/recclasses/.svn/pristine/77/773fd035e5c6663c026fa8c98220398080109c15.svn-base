#include "recclasses/I3PortiaPulse.h"
#include "wrap.h"
#include <boost/python.hpp>

void register_I3PortiaPulse()
{
  using namespace boost::python;

  // this uses some exceptionally nasty macros to do the registration
  // because of the getters and setters.  When a struct is just a
  // struct, don't code it as a class.

  wrap::I3FrameObject<I3PortiaPulse>()
    GETSET_INTERNAL_REFERENCE(I3PortiaPulse, I3RecoPulse&, RecoPulse)
    GETSET(I3PortiaPulse, double, PositionX)
    GETSET(I3PortiaPulse, double, PositionY)
    GETSET(I3PortiaPulse, double, PositionZ)

    GETSET(I3PortiaPulse, bool, LCBit)
    GETSET(I3PortiaPulse, double, IntegratedCharge)
    GETSET(I3PortiaPulse, double, BaseLine)
    GETSET(I3PortiaPulse, double, PeakBinTime)
    GETSET(I3PortiaPulse, double, EstimatedNPE)
    GETSET(I3PortiaPulse, double, LaunchTime)
    GETSET(I3PortiaPulse, double, LETime)
    GETSET(I3PortiaPulse, double, Time50)
    GETSET(I3PortiaPulse, double, Time80)
    GETSET(I3PortiaPulse, double, TOT)
    GETSET(I3PortiaPulse, int, BinNumber)
    GETSET(I3PortiaPulse, double, BinSize)
    GETSET(I3PortiaPulse, double, Amplitude)
    GETSET(I3PortiaPulse, double, PMTGain)
  ;

  wrap::I3FrameObject<I3PortiaPulseMap>("I3PortiaPulseMap");
}
