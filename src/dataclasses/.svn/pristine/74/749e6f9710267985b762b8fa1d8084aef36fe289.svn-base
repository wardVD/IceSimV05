#include <icetray/serialization.h>
#include <dataclasses/physics/I3EventHeader.h>

#include <limits>


I3EventHeader::I3EventHeader()
  : runID_(std::numeric_limits<unsigned int>::max()),
    subRunID_(std::numeric_limits<unsigned int>::max()),
    eventID_(std::numeric_limits<unsigned int>::max()),
    state_(OK)
{
}


I3EventHeader::~I3EventHeader()
{
}


template <class Archive>
void 
I3EventHeader::serialize(Archive& ar, unsigned version)
{
  if(version > i3eventheader_version_)
    log_fatal("Attempting to read version %u from file but running version %u of I3EventHeader class.",version,i3eventheader_version_);

  ar & make_nvp("I3FrameObject", base_object<I3FrameObject>(*this));
  ar & make_nvp("RunID", runID_);
  if(version > 0)
    {
      ar & make_nvp("SubRunID", subRunID_);
    }
  else
    {
      // in version 0 there was no sub run ID
      // ... using 0 as a default value
      subRunID_ = 0;
    }
  ar & make_nvp("EventID", eventID_);
  if(version > 1)
    {
      ar & make_nvp("State", state_);
    }
  else
    {
      // in version <= 1 there was no state
      // ... using OK as a default value
      state_ = OK;
    }
   if (version > 2) {
     ar & make_nvp("SubEventID", subEventID_);
     ar & make_nvp("SubEventStream", subEventStream_);
   } else {
     subEventID_ = 0;
     subEventStream_ = "";
   }
   ar & make_nvp("StartTime", startTime_);
   ar & make_nvp("EndTime", endTime_);
}

std::ostream& operator<<(std::ostream& oss, const I3EventHeader& eh){
  oss << "[ I3EventHeader  :: " << std::endl
      << "           StartTime: " << eh.GetStartTime().GetUTCString() << std::endl
      << "            EndTime : " << eh.GetEndTime().GetUTCString() << std::endl
      << "              RunID : " << eh.GetRunID() << std::endl
      << "           SubrunID : " << eh.GetSubRunID() << std::endl
      << "            EventID : " << eh.GetEventID() << std::endl
      << "         SubEventID : " << eh.GetSubEventID() << std::endl
      << "     SubEventStream : " << eh.GetSubEventStream() << std::endl
      << "]" ;
  return oss;
}

  
I3_SERIALIZABLE(I3EventHeader);
