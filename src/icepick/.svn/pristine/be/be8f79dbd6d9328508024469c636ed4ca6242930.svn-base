#ifndef I3_EVENT_ID_RANGE_FILTER_H
#define I3_EVENT_ID_RANGE_FILTER_H

#include "icetray/I3IcePick.h"

/**
 * @brief An IcePick that selects all events with 
 *    EventIDs between or equal to FirstEventId and LastEventId.
 *
 * @author $Author$
 * @date $Date$
 */
class I3EventIdRangeFilter : public I3IcePick {

public:
  I3EventIdRangeFilter(const I3Context& context) : I3IcePick(context) , firsteventId_(0){
    firsteventId_ = 0;
    AddParameter("FirstEventId", "The first event to be selected.", firsteventId_);
    lasteventId_ = 0;
    AddParameter("LastEventId", "The last event to be selected.", lasteventId_);
  }

  void Configure() {
    GetParameter("FirstEventId", firsteventId_);
    GetParameter("LastEventId", lasteventId_);
  }

  bool SelectFrame(I3Frame& frame) {

    const I3EventHeader& header = frame.Get<I3EventHeader>();
    unsigned long thisEvent = header.GetEventID();
    log_trace("Event ID %ld", thisEvent);
    bool filter = true;

    filter = thisEvent >= static_cast<unsigned long>(firsteventId_) && 
             thisEvent <= static_cast<unsigned long>(lasteventId_);
    if (filter) {
      log_debug("Found Event ID %ld", thisEvent);
    }
    return filter;
  }

private:
  //unsigned long eventId_;
  long firsteventId_;
  long lasteventId_;

  SET_LOGGER("I3EventIdRangeFilter");
};

#endif
