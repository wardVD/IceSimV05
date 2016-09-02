/**
    Copyright  (C) 2006
    The IceCube Collaboration
    $Id$
    @version 00-00-00
    @date $Date$
    @author Seon-Hee Seo <shseo@phys.psu.edu>
*/

#ifndef I3SKIPNEVENTFILTER_H
#define I3SKIPNEVENTFILTER_H

#include "icetray/I3IcePick.h"

/**
 * @brief A filter which skips the first SkipNEvents in a tray run.
 */


class I3SkipNEventFilter : public I3IcePick
{
 public:
  I3SkipNEventFilter(const I3Context& context) : I3IcePick(context) {
      skipNevents_ = 0;
      totEventsProcessed_ = 0;

      AddParameter("SkipNevents",
                   "Number of events you want to skip with this filter",
                   skipNevents_);
  }

  bool SelectFrame(I3Frame& frame){
      totEventsProcessed_++;
      if(skipNevents_ < totEventsProcessed_) return true;
      return false;
  }

  void Configure(){
      log_info("Configuring the I3SkipNEventFilter");
      GetParameter("SkipNevents", skipNevents_);
  }

private:
  int skipNevents_;
  int totEventsProcessed_;
  SET_LOGGER("I3SkipNEventFilter");
};

#endif
