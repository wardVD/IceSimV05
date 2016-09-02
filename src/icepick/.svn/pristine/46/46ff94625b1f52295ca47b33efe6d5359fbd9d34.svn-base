#ifndef I3AZIMUTHWINDOWFILTER_H
#define I3AZIMUTHWINDOWFILTER_H

#include "icetray/I3IcePick.h"

#include "dataclasses/I3Direction.h"
#include "dataclasses/physics/I3Particle.h"

/**
 * @brief An I3IcePick which selects events which have
 * a particle with azimuth angle between some parameters.
 * By M.R. Duvoort
 */
class I3AzimuthWindowFilter : public I3IcePick
{
 public:

  I3AzimuthWindowFilter(const I3Context& context) : I3IcePick(context),
    particleKey_("Particle"),
    azimuthmin_(1.22),// > 70
    azimuthmax_(1.75)// < 100(=1.75 rad)
    {
      AddParameter("ParticleKey",
		   "Name of the particle in the frame",
		   particleKey_);
      AddParameter("AzimuthMin",
		   "The minimum azimuth angle that above which events are selected",
		   azimuthmin_);
      AddParameter("AzimuthMax",
		   "The maximum azimuth angle that below which events are selected",
		   azimuthmax_);
    }

  void Configure() {
    GetParameter("ParticleKey",particleKey_);
    GetParameter("AzimuthMin",azimuthmin_);
    GetParameter("AzimuthMax",azimuthmax_);
  }

  bool SelectFrame(I3Frame& frame) {
    if(frame.Has(particleKey_))
      {
	const I3Particle& particle = frame.Get<I3Particle>(particleKey_);
        if ( I3Particle::OK != particle.GetFitStatus() ){
          return false; // no successful fit -> reject event
        }
	const I3Direction& dir = particle.GetDir();
	if( dir.GetAzimuth() < azimuthmax_ && dir.GetAzimuth()>azimuthmin_ )
	  {
	    return true;
	  }
        log_trace("Particle %s has a azimuth of %f and doesn't pass "
             "the threshold of maximum %f and minimum %f",
             particleKey_.c_str(),dir.GetAzimuth(),azimuthmax_,azimuthmin_);
	return false;
      }
    else
      {
	log_trace("Event does not have particleKey_ %s.  Event being filtered.",
		  particleKey_.c_str());
	return false;
      }
  }

 private:
  std::string particleKey_;
  double azimuthmin_;
  double azimuthmax_;

  SET_LOGGER("I3AzimuthWindowFilter");
};

#endif
