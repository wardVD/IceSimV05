#ifndef I3_EVENT_ID_FILTER_H
#define I3_EVENT_ID_FILTER_H

#include "icetray/I3IcePick.h"

/**
 * @brief An IcePick that selects an event with a 
 *    specific EventID from a vector (parameter) of EventIds.
 *
 * Modified: Seon-Hee Seo (shseo@phys.psu.edu)
 */
class I3EventIdFilter : public I3IcePick {

public:
  I3EventIdFilter(const I3Context& context) : I3IcePick(context) {
    AddParameter("EventIds", "The event to be selected.", vecEventId_);
  }

  void Configure() {
    GetParameter("EventIds", vecEventId_);
  }

  bool SelectFrame(I3Frame& frame) {

    const I3EventHeader& header = frame.Get<I3EventHeader>();
    unsigned long thisEvent = header.GetEventID();
    log_trace("Event ID %ld", thisEvent);

    for(unsigned int k=0; k < vecEventId_.size(); k++){
        if (thisEvent == vecEventId_[k]) {
          log_trace("Found Event ID %ld", vecEventId_[k]);
          return true;
        }
    }
    return false;
  }

private:
  std::vector<unsigned long> vecEventId_;

  SET_LOGGER("I3EventIdFilter");
};

#endif
