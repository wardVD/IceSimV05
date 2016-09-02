#ifndef I3LOGICALORFILTER_H
#define I3LOGICALORFILTER_H

#include "icetray/I3IcePick.h"

/**
 * @brief An I3IcePick which takes two previouslly run IcePick
 * decisions and filters with a logical or operating on both
 * those decisions.
 */
class I3LogicalOrFilter : public I3IcePick
{
 public:
  I3LogicalOrFilter(const I3Context& context) : I3IcePick(context),
    fdec_(""), sdec_("")
    {
      AddParameter("FirstDecisionName","The name of the first decision "
		   "in the frame.", fdec_);
      AddParameter("SecondDecisionName","The name of the second decision "
		   "in the frame.", sdec_);
    }

  void Configure() {
    GetParameter("FirstDecisionName", fdec_);
    GetParameter("SecondDecisionName", sdec_);
  }

  bool SelectFrame(I3Frame& frame) {

    if(frame.Has(fdec_) && frame.Has(sdec_))
      {
	const I3Bool& firstbool = frame.Get<I3Bool>(fdec_);
	const I3Bool& secondbool = frame.Get<I3Bool>(sdec_);

	log_trace("first decision: %i ,  second decision: %i",
		  firstbool.value,secondbool.value);
	if(firstbool.value || secondbool.value)
	  {
	    log_info("One or both decisions keep event.  Keeping event");
	    return true;
	  }

	log_debug("Neither decision kept the event.  Filtering event.");
	return false;
      }

    log_debug("One or both decisions missing in the frame.  Filtering event.");
    return false;
  }

 private:
  std::string fdec_;
  std::string sdec_;

  SET_LOGGER("I3LogicalOrFilter");
};

#endif
