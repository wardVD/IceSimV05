/**
 *  copyright  (C) 2004
 *  the icecube collaboration
 *  $Id: I3FiniteCuts.cxx 38584 2009-10-20 18:53:37Z huelss $
 *
 *  @version $Revision: 38584 $
 *  @date $Date: 2009-10-20 13:53:37 -0500 (Tue, 20 Oct 2009) $
 *  @author Sebastian Euler <sebastian.euler@icecube.wisc.edu>
 *
 */

#include "recclasses/I3FiniteCuts.h"
#include "wrap.h"
#include <string>
#include <sstream>
#include <boost/python.hpp>

std::string i3finitecuts_repr(const I3FiniteCuts &fc){
  std::ostringstream s;
  s << "I3FiniteCuts(" << fc.Length << "," << fc.Lend << "," << fc.Lstart <<
       "," << fc.Sdet << "," << fc.finiteCut << "," << fc.DetectorLength << ")";
  return s.str();
}

void register_I3FiniteCuts()
{
  using namespace boost::python;

  wrap::I3FrameObject<I3FiniteCuts>()
    .def_readwrite("Length", &I3FiniteCuts::Length)
    .def_readwrite("endFraction", &I3FiniteCuts::Lend)
    .def_readwrite("startFraction", &I3FiniteCuts::Lstart)
    .def_readwrite("Sdet", &I3FiniteCuts::Sdet)
    .def_readwrite("finiteCut", &I3FiniteCuts::finiteCut)
    .def_readwrite("DetectorLength", &I3FiniteCuts::DetectorLength)
    .def("__repr__", i3finitecuts_repr)
    ;
}
