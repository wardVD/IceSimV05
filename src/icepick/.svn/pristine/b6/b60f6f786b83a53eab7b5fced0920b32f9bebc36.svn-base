#ifndef I3LDIRFILTER_H
#define I3LDIRFILTER_H

#include "icetray/I3IcePick.h"

#include "phys-services/I3CutValues.h"

/**
 * @brief An I3IcePick which selects events which have
 * an I3Cuts object with LDir greater than some threshold.
 */
class I3LDirFilter : public I3IcePick
{
 public:
  I3LDirFilter(const I3Context& context) : I3IcePick(context),
    cutsKey_(""), ldir_(300)
    {
      AddParameter("CutValuesKey","The name of the I3CutValues "
		   "object in the frame", cutsKey_);
      AddParameter("LDir","The LDir threshold that an event's "
		   "must exceed to be kept", ldir_);
    }

  void Configure() {
    GetParameter("CutValuesKey", cutsKey_);
    GetParameter("LDir", ldir_);
  }

  bool SelectFrame(I3Frame& frame) {
    if(frame.Has(cutsKey_))
      {
	const I3CutValues& cuts = frame.Get<I3CutValues>(cutsKey_);
	
	if(cuts.Ldir >= ldir_)
	  {
	    log_debug("Event LDir %f passes threshold of %f.",
		      cuts.Ldir, ldir_);
	    return true;
	  }

	log_debug("Event LDir %f doesn't pass threshold of %f.",
		  cuts.Ldir, ldir_);
	return false;
      }

    log_debug("I3CutValues of name %s missing in the frame.  "
	      "Filtering Event.",cutsKey_.c_str());
    return false;
  }

 private:
  std::string cutsKey_;
  double ldir_;

  SET_LOGGER("I3LDirFilter");
};

#endif
