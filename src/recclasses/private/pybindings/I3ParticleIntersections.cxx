/*
 * copyright  (C) 2012
 * Nathan Whitehorn, Claudio Kopper
 * The Icecube Collaboration: http://www.icecube.wisc.edu
 *
 * $Id: I3ParticleIntersections.cxx 97706 2013-01-19 21:10:48Z nwhitehorn $
 *
 * @version $Revision: 97706 $
 * @date $LastChangedDate: 2013-01-19 16:10:48 -0500 (Sat, 19 Jan 2013) $
 * @author $LastChangedBy: nwhitehorn $
 */

#include "recclasses/I3ParticleIntersections.h"
#include "wrap.h"
#include <boost/foreach.hpp>
#include <boost/python.hpp>
#include <vector>

void register_I3ParticleIntersections()
{
  using namespace boost::python;

  wrap::vector<I3Position>();

  wrap::I3FrameObject<I3ParticleIntersections>()
    .def_readwrite("intersections", &I3ParticleIntersections::intersections_)
    .def("GetIntersections", &I3ParticleIntersections::GetIntersections,
         return_value_policy<copy_const_reference>())
    .def("SetIntersections", &I3ParticleIntersections::SetIntersections)
    .def_readwrite("particle", &I3ParticleIntersections::particle_)
    .def("GetParticle", &I3ParticleIntersections::GetParticle,
         return_value_policy<copy_const_reference>())
    .def("SetParticle", &I3ParticleIntersections::SetParticle)
  ;
  
  wrap::I3Vector<I3ParticleIntersections>("I3ParticleIntersectionsSeries")
  ;
}
