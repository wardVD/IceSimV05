/** $Id: CompactTrack.h 128654 2015-02-04 18:34:51Z jvansanten $
 * @file
 * @author Jakob van Santen <vansanten@wisc.edu>
 *
 * $Revision: 128654 $
 * $Date: 2015-02-04 12:34:51 -0600 (Wed, 04 Feb 2015) $
 */

#ifndef MUONGUN_COMPACTTRACK_H_INCLUDED
#define MUONGUN_COMPACTTRACK_H_INCLUDED

#include <dataclasses/physics/I3Particle.h>
#include <dataclasses/I3Vector.h>

namespace I3MuonGun {

/**
 * @brief A compressed representation of a muon in a bundle
 */
class CompactTrack {
public:
	CompactTrack() {};
	CompactTrack(const I3Particle &track);
	
	void SetRadius(double r) { radius_=r; };
	double GetRadius() const { return radius_; };
	void SetEnergy(double r) { energy_=r; };
	double GetEnergy() const { return energy_; };
	void SetTime(double r) { time_=r; };
	double GetTime() const { return time_; };
	void SetType(I3Particle::ParticleType t) { type_=t; };
	I3Particle::ParticleType GetType() const { return type_; };
	
	bool operator==(const CompactTrack &) const;
private:
	double radius_, energy_, time_;
	I3Particle::ParticleType type_;
	
	friend class boost::serialization::access;
	template <typename Archive>
	void serialize(Archive&, unsigned);
};

/**
 * @brief The state of a muon bundle at a set of vertical depths
 */
struct TrackBundle : public I3FrameObject, std::map<double, std::vector<CompactTrack> > {
	virtual ~TrackBundle();
	friend class boost::serialization::access;
	template <typename Archive>
	void serialize(Archive&, unsigned);
};

I3_POINTER_TYPEDEFS(TrackBundle);

}

#endif // MUONGUN_COMPACTTRACK_H_INCLUDED
