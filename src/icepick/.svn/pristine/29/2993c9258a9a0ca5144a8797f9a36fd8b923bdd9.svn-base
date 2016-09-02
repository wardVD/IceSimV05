#ifndef I3SIMPLEFILTER_H
#define I3SIMPLEFILTER_H

#include "icetray/I3IcePick.h"

/**
 * @brief An I3IcePick which takes a "decision" (bool) from the frame and 
 * filters the event if that decision is TRUE.
 */
class I3SimpleFilter : public I3IcePick
{
 public:
  I3SimpleFilter(const I3Context& context) : I3IcePick(context),
    inputDecision_("")
    {
      AddParameter("InputDecisionName","The name of the decision "
		   "in the frame on which to filter."
		   ,inputDecision_);
    }

  void Configure()
    {
      GetParameter("InputDecisionName", inputDecision_);
    }

  bool SelectFrame(I3Frame& frame) 
    {
      if ( frame.Has(inputDecision_) ) {
	bool inputbool = frame.Get<I3Bool>(inputDecision_).value;
        if ( !inputbool ) {
	  log_debug("(%s) %s rejects event.",
		    GetName().c_str(), inputDecision_.c_str() );
	  return false;
	} else {
	  log_debug("(%s) %s keeps event.",
		    GetName().c_str(), inputDecision_.c_str() );
	  return true;
	}
      }

      log_debug("(%s) Input decision missing in the frame. Rejecting event.",
		GetName().c_str());
      return false;
    }

 private:
  std::string inputDecision_;

  SET_LOGGER("I3SimpleFilter");
};

#endif
