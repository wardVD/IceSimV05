/**
 *  copyright  (C) 2004
 *  the icecube collaboration
 *  $Id: I3StartStopParams.cxx 38584 2009-10-20 18:53:37Z huelss $
 *
 *  @version $Revision: 38584 $
 *  @date $Date: 2009-10-20 13:53:37 -0500 (Tue, 20 Oct 2009) $
 *  @author Sebastian Euler <sebastian.euler@icecube.wisc.edu>
 *
 */

#include "recclasses/I3StartStopParams.h"
#include "wrap.h"
#include <string>
#include <sstream>
#include <boost/python.hpp>

std::string
i3startstopparams_repr(const I3StartStopParams &ssp){
  std::ostringstream s;
  s << "I3StartStopParams("
    << ssp.LLHStartingTrack << ","
    << ssp.LLHStoppingTrack << ","
    << ssp.LLHInfTrack << ")";
  return s.str();
}

void register_I3StartStopParams()
{
  using namespace boost::python;

  wrap::I3FrameObject<I3StartStopParams>()
    .def_readwrite("LLHStartingTrack", &I3StartStopParams::LLHStartingTrack)
    .def_readwrite("LLHStoppingTrack", &I3StartStopParams::LLHStoppingTrack)
    .def_readwrite("LLHInfTrack", &I3StartStopParams::LLHInfTrack)
    .def("__repr__", i3startstopparams_repr)
    ;
}
