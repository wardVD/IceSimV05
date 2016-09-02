/**
    Copyright  (C) 2006
    The IceCube Collaboration
    @file I3PrescaleFilter.h
    @date September 2006
    @author David Boersma <boersma@icecube.wisc.edu>
*/

#ifndef I3PRESCALEFILTER_H
#define I3PRESCALEFILTER_H

#include "icetray/I3IcePick.h"

/**
 * @brief A filter which blindly selects 1/N events (N= PrescaleFactor) 
 */

class I3PrescaleFilter:public I3IcePick
{
public:
  I3PrescaleFilter (const I3Context & context):I3IcePick (context)
  {
    prescaleFactor_ = 1;
    totEventsProcessed_ = 0;

    AddParameter ("PrescaleFactor",
		  "Prescale factor N for sampling: each Nth event is kept.",
		  prescaleFactor_);
  }

  bool SelectFrame (I3Frame & frame)
  {
    totEventsProcessed_++;
    if (0 == (totEventsProcessed_ % prescaleFactor_))
      {
	log_debug ("%dth event is kept", totEventsProcessed_);
	return true;
      }
    log_trace ("%dth event is discarded", totEventsProcessed_);
    return false;
  }

  void Configure ()
  {
    log_info ("Configuring the I3PrescaleFilter");
    GetParameter ("PrescaleFactor", prescaleFactor_);
    if (prescaleFactor_ <= 0)
      {
	log_fatal ("Prescale factor should be positive, I got %d",
		   prescaleFactor_);
      }
  }

private:
  int prescaleFactor_;
  int totEventsProcessed_;
  SET_LOGGER ("I3PrescaleFilter");
};

#endif /* I3PRESCALEFILTER_H */
