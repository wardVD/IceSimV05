#ifndef I3_EVENTTIME_FILTER_H
#define I3_EVENTTIME_FILTER_H

#include "icetray/I3IcePick.h"
#include <fstream>

/**
 * @brief An IcePick that ignores events with  
 *    specific Event start times from a vector (parameter) of EventTimes.
 *
 * Modified: Steve Movit
 */
class I3EventTimeFilter : public I3IcePick {

private:
  std::string eventTimeFile_;
  std::vector<int64_t> vecEventTime_;

public:
  I3EventTimeFilter(const I3Context& context) : I3IcePick(context) {
    AddParameter("EventTimes", "The event times to be deselected.", eventTimeFile_);
  }

  void Configure() {
    GetParameter("EventTimes", eventTimeFile_);

    std::ifstream times;
    int64_t this_time;
    times.open(eventTimeFile_.c_str(),std::ifstream::in);
    while(!times.eof()){
      times >> this_time;
      vecEventTime_.push_back(this_time);
    }
  }

  bool SelectFrame(I3Frame& frame) {

    const I3EventHeader& header = frame.Get<I3EventHeader>();
    int64_t thisEvent = header.GetStartTime().GetUTCDaqTime();
    log_trace("Event Start Time %lld", (long long)thisEvent);

    for(unsigned int k = 0; k < vecEventTime_.size(); k++){
        if(thisEvent == vecEventTime_[k]) {
          log_trace("Found Event Time %lld - skipping it", (long long)vecEventTime_[k]);
          return false;
        }
    }
    return true;
  }

  SET_LOGGER("I3EventTimeFilter");
};

#endif
