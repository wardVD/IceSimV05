/**
 * $Id$
 * copyright  (C) 2012
 * Nathan Whitehorn, Claudio Kopper
 * The Icecube Collaboration: http://www.icecube.wisc.edu
 *
 * @version $Revision: 97707 $
 * @date $LastChangedDate: 2013-01-19 16:11:34 -0500 (Sat, 19 Jan 2013) $
 * @author Nathan Whitehorn
 * @author Claudio Kopper
 */

#ifndef RECCLASSES_I3PARTICLEINTERSECTIONS_H_INCLUDED
#define RECCLASSES_I3PARTICLEINTERSECTIONS_H_INCLUDED

#include <icetray/I3FrameObject.h>
#include <dataclasses/I3Vector.h>
#include <dataclasses/I3Position.h>
#include <dataclasses/physics/I3Particle.h>

#include <ostream>

static const unsigned i3particleintersections_version_ = 0;

/**
 *  @brief Stores a particle and its intersection points with
 *  the fiducial volume (see project VHESelfVeto).
 */
class I3ParticleIntersections : public I3FrameObject
{

public:

  virtual ~I3ParticleIntersections() {}

  inline
  void SetIntersections(const std::vector<I3Position> &intersections)
  {
    intersections_ = intersections;
  }

  inline
  const std::vector<I3Position>& GetIntersections() const
  {
    return intersections_;
  }

  inline
  void SetParticle(const I3Particle &particle) {
    particle_ = particle;
  }
  
  inline
  const I3Particle& GetParticle() const
  {
    return particle_;
  }

  bool operator==(const I3ParticleIntersections& rhs) const;

private:
  I3Particle particle_;
  std::vector<I3Position> intersections_;

  friend class boost::serialization::access;
  template <class Archive> void serialize(Archive & ar, unsigned version);

  friend void register_I3ParticleIntersections();
};

std::ostream& operator<<(std::ostream&, const I3ParticleIntersections&);

BOOST_CLASS_VERSION(I3ParticleIntersections, i3particleintersections_version_);

typedef I3Vector<I3ParticleIntersections> I3ParticleIntersectionsSeries;

I3_POINTER_TYPEDEFS(I3ParticleIntersections);
I3_POINTER_TYPEDEFS(I3ParticleIntersectionsSeries);

#endif //I3PARTICLEINTERSECTIONS_H_INCLUDED
