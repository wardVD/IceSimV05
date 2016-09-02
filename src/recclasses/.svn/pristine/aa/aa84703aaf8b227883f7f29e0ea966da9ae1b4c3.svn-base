/**
 * (c) 2012 the IceCube Collaboration
 * $Id$
 *
 * @date $Date$
*/

#ifndef RECCLASSES_SHIELD_I3SHIELDEVENTPARAMS_H_INCLUDED
#define RECCLASSES_SHIELD_I3SHIELDEVENTPARAMS_H_INCLUDED

#include <limits>
#include <icetray/I3FrameObject.h>
#include <icetray/OMKey.h>
#include <boost/serialization/access.hpp>
#include <boost/serialization/version.hpp>

static const unsigned i3shieldhitrecord_version_ = 1;

/// @brief Stores veto properties of a single I3RecoPulse with respect to a track (see project shield). 
///
/// This is a simple wrapper for the time residual of a pulse with respect 
/// to the expected arrival time of a track, as well as its distance from that 
/// track. Members of this class are stored using the usual I3Units (meters and
/// nanoseconds, technically). 
class I3ShieldHitRecord : public I3FrameObject{
public:
	/// Constructs null state, time residual and distance are initialized to NaN
	I3ShieldHitRecord():
	  timeResidual(std::numeric_limits<double>::quiet_NaN()),
	  distance(std::numeric_limits<double>::quiet_NaN()),
	  DOMkey()
	{}

	/// Convenience constructor
	/// @param t The time residual of the hit
	/// @param d The distance from the track to the hit
	/// @param key The DOM which recorded the pulse (optional)
	I3ShieldHitRecord(double t, double d, OMKey key = OMKey()):
	timeResidual(t),
	distance(d),
	DOMkey(key){}

	double GetTimeResidual() const{ return(timeResidual); }
	void SetTimeResidual(double t){ timeResidual = t; }
	
	double GetDistance() const{ return(distance); }
	void SetDistance(double d){ distance = d; }
	
	OMKey GetDOMkey() const{ return(DOMkey); }
	void SetDOMkey(OMKey key){ DOMkey = key; }

private:
	/// Difference between when the hit occurred and when the 
	/// reconstructed shower front would have passed the OM (ns)
	double timeResidual;
	
	/// Distance of the OM from the core of the reconstructed 
	/// shower (the reconstructed track) (meter)
	double distance;
	
	/// OM corresponding to the pulse 
	OMKey DOMkey;
	
	friend class boost::serialization::access;
	
	template <class Archive>
	  void serialize(Archive& ar, unsigned version);
};

I3_POINTER_TYPEDEFS(I3ShieldHitRecord);
BOOST_CLASS_VERSION(I3ShieldHitRecord, i3shieldhitrecord_version_);

bool operator==(const I3ShieldHitRecord& r1, const I3ShieldHitRecord& r2);

#endif //SHIELD_I3SHIELDEVENTPARAMS_H_INCLUDED
