#ifndef I3NDIRFILTER_H
#define I3NDIRFILTER_H

#include "icetray/I3IcePick.h"

#include "phys-services/I3CutValues.h"

/**
 * @brief An I3IcePick which selects events which have
 * an I3Cuts object with NDir greater than some threshold.
 */
class I3NDirFilter : public I3IcePick
{
 public:
  I3NDirFilter(const I3Context& context) : I3IcePick(context),
    cutsKey_(""), ndir_(5)
    {
      AddParameter("CutValuesKey","The name of the I3CutValues "
		   "object in the frame", cutsKey_);
      AddParameter("NDir","The number of direct hits that an event "
		   "must have to be kept", ndir_);
    }

  void Configure() {
    GetParameter("CutValuesKey", cutsKey_);
    GetParameter("NDir", ndir_);
  }

  bool SelectFrame(I3Frame& frame) {
    if(frame.Has(cutsKey_))
      {
	const I3CutValues& cuts = frame.Get<I3CutValues>(cutsKey_);
	
	if(cuts.Ndir >= ndir_)
	  {
	    log_debug("Event has %i direct hits and passes the threshold"
		      " of %i direct hits.",cuts.Ndir, ndir_);
	    return true;
	  }

	log_debug("Event has %i direct hits, and does not pass the threshold "
		  "of %i direct hits.",cuts.Ndir, ndir_);
	return false;
      }

    log_debug("I3CutValues of name %s missing in the frame.  "
	      "Filtering Event.",cutsKey_.c_str());
    return false;
  }

 private:
  std::string cutsKey_;
  int ndir_;

  SET_LOGGER("I3NDirFilter");
};

#endif
