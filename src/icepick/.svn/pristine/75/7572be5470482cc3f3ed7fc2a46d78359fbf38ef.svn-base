#ifndef I3RUNIDEVENTIDFILTER_H
#define I3RUNIDEVENTIDFILTER_H

#include "icetray/I3IcePick.h"

/**
 * @brief An IcePick that selects events with  
 *  specific RunIDs and EventIds provided in 
 *  a RunID vector and corresponding EventID vector.
 *
 * @author $Seon-Hee Seo (seo@physto.se)$
 * @date $Date$
 */

class I3RunIdEventIdFilter : public I3IcePick {

public:
  I3RunIdEventIdFilter(const I3Context& context) : I3IcePick(context) {
    AddParameter("RunIds", "Vector of RunIDs of the events to be selected.", vecRunId_);
    AddParameter("EventIds", "Vector EventIDs of the event to be selected.", vecEventId_);
  }

  void Configure() {
    GetParameter("RunIds", vecRunId_);
    GetParameter("EventIds", vecEventId_);
  }

  bool SelectFrame(I3Frame& frame) {

    const I3EventHeader& header = frame.Get<I3EventHeader>();
    unsigned long currentRunId = header.GetRunID();
    unsigned long currentEventId = header.GetEventID();
    log_trace("Current RunID = %ld,  Event ID = %ld", currentRunId, currentEventId);

    //---Loop for RunID, EventID---------
    for(unsigned int k=0; k < vecRunId_.size(); k++){
        bool foundRunId = (currentRunId == (vecRunId_[k]));
        bool foundEventId = (currentEventId == (vecEventId_[k]));
        if(foundRunId && foundEventId){
            log_trace("Found Run ID = %ld,  Event ID = %ld", vecRunId_[k], vecEventId_[k]);
            return true;
        }
    }//---End Loop for RunID, EventID
    return false;
  }

private:
  std::vector<unsigned long> vecRunId_;
  std::vector<unsigned long> vecEventId_;

  SET_LOGGER("I3RunIdEventIdFilter");
};

#endif
