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
#include <icetray/serialization.h>
#include <boost/foreach.hpp>

template <class Archive>
void I3ParticleIntersections::serialize (Archive &ar, unsigned version)
{
  if (version > i3particleintersections_version_)
    log_fatal("Attempting to read version %u from file but running "
              "version %u of I3ParticleIntersections class.",
              version, i3particleintersections_version_);

  ar & make_nvp("I3FrameObject", base_object<I3FrameObject>(*this));
  ar & make_nvp("particle", particle_);
  ar & make_nvp("intersections", intersections_);
}     

I3_SERIALIZABLE(I3ParticleIntersections);
I3_SERIALIZABLE(I3ParticleIntersectionsSeries);

bool I3ParticleIntersections::operator==(const I3ParticleIntersections& rhs) const {
  if (!(particle_ == rhs.particle_))
    return false;
  if (intersections_.size() != rhs.intersections_.size())
    return false;
  
  for (std::size_t i=0; i<intersections_.size(); ++i)
    if (!(intersections_[i] == rhs.intersections_[i]))
      return false;

  return true;
}

std::ostream& operator<<(std::ostream& os, const I3ParticleIntersections& x)
{
  const std::vector<I3Position>& vpos = x.GetIntersections();
  os << "[ I3ParticleIntersections::"
        "\n  intersections: [";
  if (!vpos.empty()) {
    for (unsigned i = 0, n = vpos.size() - 1; i < n; ++i)
      os << vpos[i] << ", ";
    os << vpos.back();
  }
  os << "]"
        "\n  particle: " << x.GetParticle() <<
        " ]";
  return os;
}
