#include <map>

#include <payload-parsing/decode.h>
#include <payload-parsing/I3PayloadParsingEventDecoder.h>
#include <dataclasses/physics/I3EventHeader.h>
#include <icetray/I3Frame.h>
#include <interfaces/I3OMKey2MBID.h>
#include <daq-decode/I3DAQDecoderUtil.h>


using namespace std;
using namespace payload_parsing;


I3PayloadParsingEventDecoder::
I3PayloadParsingEventDecoder(const I3OMKey2MBID& service,
    unsigned year,
    const string& headerID,
    const string& triggerID,
    const vector<OMKey>& specialDataOMs,
    const string& specialDataID,
    const string& inIceID,
    const string& iceTopID,
    const string& speDataID,
    const string& flasherDataID,
    const string& cpuDataID,
    const string& onboardLEDID,
    const string& testDataID,
    const string& minBiasID)
: omkey2mbid(service),
  year_(year),
  headerID_(headerID),
  triggerID_(triggerID),
  specialDataOMs_(specialDataOMs),
  specialDataID_(specialDataID),
  inIceID_(inIceID),
  iceTopID_(iceTopID),
  speDataID_(speDataID),
  flasherDataID_(flasherDataID),
  cpuDataID_(cpuDataID),
  onboardLEDID_(onboardLEDID),
  testDataID_(testDataID),
  trigger_(!triggerID.empty()),
  minBiasID_(minBiasID),
  waveforms_(!cpuDataID_.empty() || !speDataID_.empty()
      || !testDataID_.empty() || !flasherDataID_.empty())
{
  allTriggerTypes_.push_back(I3DOMLaunch::TEST_PATTERN);
  allTriggerTypes_.push_back(I3DOMLaunch::CPU_REQUESTED);
  allTriggerTypes_.push_back(I3DOMLaunch::SPE_DISCRIMINATOR_TRIGGER);
  allTriggerTypes_.push_back(I3DOMLaunch::FLASHER_BOARD_TRIGGER);
  allTriggerTypes_.push_back(I3DOMLaunch::ONBOARD_LED);
  allTriggerModes_.push_back(I3DOMLaunch::MIN_BIAS);
}

I3PayloadParsingEventDecoder::~I3PayloadParsingEventDecoder()
{
}


I3Time 
I3PayloadParsingEventDecoder::FillEvent(I3Frame& frame,
    const vector<char>& data) const
{  
  if(data.size() < 16)
  {
    log_fatal("expected at least 16 bytes for event, got %zd instead",
        data.size());
  }

  uint32_t payloadType = decode<uint32_t>(BIG, 4, data);
  uint64_t globalTime = decode<uint64_t>(BIG, 8, data);

  if((payloadType != 13) && (payloadType != 19) && (payloadType != 20) && 
      (payloadType != 21) && (payloadType != 22))
  {
    log_fatal("expected type 13, 19, 20, 21, 22 and got %d",payloadType);
  }

  DecodeConfiguration configuration(globalTime, year_, trigger_, waveforms_);

  // setup target data structures
  DOMLaunches launches;
  NewDOMLaunches newlaunches;
  I3TriggerHierarchyPtr triggers(new I3TriggerHierarchy);
  I3EventHeaderPtr header(new I3EventHeader());

  if (payloadType == 13)
  {
    DecodeTarget<13>::type target(launches, *triggers, *header);

    // do the decoding ...
    decode_payload<13>(target, configuration, data, 0);
  }
  else if (payloadType == 19)
  {
    DecodeTarget<19>::type target(launches, *triggers, *header);

    //do the decoding ...
    decode_payload<19>(target, configuration, data, 0);
  }
  else if (payloadType == 20)
  {
    DecodeTarget<20>::type target(launches, *triggers, *header);

    //do the decoding ...
    decode_payload<20>(target, configuration, data, 0);
  }
  else if (payloadType == 21)
  {
    DecodeTarget<21>::type target(newlaunches, *triggers, *header);

    //do the decoding ...
    decode_payload<21>(target, configuration, data, 0);
  }
  else if (payloadType == 22)
  {
    DecodeTarget<22>::type target(newlaunches, *triggers, *header);

    //do the decoding ...
    decode_payload<22>(target, configuration, data, 0);
  }


  // the DOM launches are organized into the frame
  map<I3DOMLaunch::TriggerType,I3DOMLaunchSeriesMapPtr> allInIceLaunches;
  map<I3DOMLaunch::TriggerType,I3DOMLaunchSeriesMapPtr> allIceTopLaunches;

  map<I3DOMLaunch::TriggerType,I3DOMLaunchSeriesMapPtr> allInIceMinBiasLaunches;
  map<I3DOMLaunch::TriggerType,I3DOMLaunchSeriesMapPtr> allIceTopMinBiasLaunches;

  I3DOMLaunchSeriesMapPtr specialDOMs(new I3DOMLaunchSeriesMap());

  for(vector<I3DOMLaunch::TriggerType>::const_iterator iter =
        allTriggerTypes_.begin() ;
      iter != allTriggerTypes_.end() ; 
      iter++)
  {
    allInIceLaunches[*iter] = 
      I3DOMLaunchSeriesMapPtr(new I3DOMLaunchSeriesMap());
    allIceTopLaunches[*iter] = 
      I3DOMLaunchSeriesMapPtr(new I3DOMLaunchSeriesMap());

    allInIceMinBiasLaunches[*iter] = 
      I3DOMLaunchSeriesMapPtr(new I3DOMLaunchSeriesMap());
    allIceTopMinBiasLaunches[*iter] = 
      I3DOMLaunchSeriesMapPtr(new I3DOMLaunchSeriesMap());
  }

  allInIceLaunches[I3DOMLaunch::MPE_DISCRIMINATOR_TRIGGER] = 
    allInIceLaunches[I3DOMLaunch::SPE_DISCRIMINATOR_TRIGGER];

  allIceTopLaunches[I3DOMLaunch::MPE_DISCRIMINATOR_TRIGGER] = 
    allIceTopLaunches[I3DOMLaunch::SPE_DISCRIMINATOR_TRIGGER];

  allInIceMinBiasLaunches[I3DOMLaunch::MPE_DISCRIMINATOR_TRIGGER] = 
    allInIceMinBiasLaunches[I3DOMLaunch::SPE_DISCRIMINATOR_TRIGGER] = 
    allInIceMinBiasLaunches[I3DOMLaunch::CPU_REQUESTED] = 
    allInIceMinBiasLaunches[I3DOMLaunch::FLASHER_BOARD_TRIGGER] = 
    allInIceMinBiasLaunches[I3DOMLaunch::ONBOARD_LED] =
    allInIceMinBiasLaunches[I3DOMLaunch::TEST_PATTERN];

  allIceTopMinBiasLaunches[I3DOMLaunch::MPE_DISCRIMINATOR_TRIGGER] = 
    allIceTopMinBiasLaunches[I3DOMLaunch::SPE_DISCRIMINATOR_TRIGGER] =
    allIceTopMinBiasLaunches[I3DOMLaunch::CPU_REQUESTED] = 
    allIceTopMinBiasLaunches[I3DOMLaunch::FLASHER_BOARD_TRIGGER] = 
    allIceTopMinBiasLaunches[I3DOMLaunch::ONBOARD_LED] =
    allIceTopMinBiasLaunches[I3DOMLaunch::TEST_PATTERN];
  if ((payloadType == 22) || (payloadType == 21))
  {
    for(NewDOMLaunches::const_iterator iter = newlaunches.begin() ; 
        iter != newlaunches.end() ; 
        iter++)
    {
      OMKey key = iter->first;

      I3DOMLaunch::TriggerMode mode = iter->second.GetTriggerMode();

      I3DOMLaunch::TriggerType trigger = iter->second.GetTriggerType();

      //cout << key << mode << trigger << endl;
      if (!(I3DOMLaunch::MIN_BIAS & mode))
      {
        if(find(specialDataOMs_.begin(),specialDataOMs_.end(),key) 
            != specialDataOMs_.end())
        {
          (*specialDOMs)[key].push_back(iter->second);
        }
        else if(key.GetOM() > I3DAQDecoderUtil::MAX_IN_ICE_OM_NUMBER)
        {
          map<I3DOMLaunch::TriggerType,I3DOMLaunchSeriesMapPtr>::iterator 
            found = allIceTopLaunches.find(trigger);
          if(found == allIceTopLaunches.end())
            log_fatal("unknown trigger type %d",trigger);
          I3DOMLaunchSeries& series = (*(found->second))[key];
          series.push_back(iter->second);
        }
        else
        {

          map<I3DOMLaunch::TriggerType,I3DOMLaunchSeriesMapPtr>::iterator 
            found = allInIceLaunches.find(trigger);
          if(found == allInIceLaunches.end())
            log_fatal("unknown trigger type %d",trigger);
          I3DOMLaunchSeries& series = (*(found->second))[key];
          series.push_back(iter->second);
        }
      }
      else
      {
        if(find(specialDataOMs_.begin(),specialDataOMs_.end(),key) 
            != specialDataOMs_.end())
        {
          (*specialDOMs)[key].push_back(iter->second);
        }
        else if(key.GetOM() > I3DAQDecoderUtil::MAX_IN_ICE_OM_NUMBER)
        {
          map<I3DOMLaunch::TriggerType,I3DOMLaunchSeriesMapPtr>::iterator 
            found = allIceTopMinBiasLaunches.find(trigger);
          if(found == allIceTopMinBiasLaunches.end())
            log_fatal("unknown trigger type %d",trigger);
          I3DOMLaunchSeries& series = (*(found->second))[key];
          series.push_back(iter->second);
        }
        else
        {
          map<I3DOMLaunch::TriggerType,I3DOMLaunchSeriesMapPtr>::iterator 
            found = allInIceMinBiasLaunches.find(trigger);
          if(found == allInIceMinBiasLaunches.end())
            log_fatal("unknown trigger type %d",trigger);
          I3DOMLaunchSeries& series = (*(found->second))[key];
          series.push_back(iter->second);

        }
      }
    }
  }
  else
  {
    for(DOMLaunches::const_iterator iter = launches.begin() ; 
        iter != launches.end() ; 
        iter++)
    {
      uint64_t mbid = iter->first;
      OMKey key = omkey2mbid.GetOMKey(mbid);
      I3DOMLaunch::TriggerMode mode = iter->second.GetTriggerMode();
      I3DOMLaunch::TriggerType trigger = iter->second.GetTriggerType();

      //cout << key << mode << trigger << endl;

      if (!(I3DOMLaunch::MIN_BIAS & mode))
      {
        if(find(specialDataOMs_.begin(),specialDataOMs_.end(),key) 
            != specialDataOMs_.end())
        {
          (*specialDOMs)[key].push_back(iter->second);
        }
        else if(key.GetOM() > I3DAQDecoderUtil::MAX_IN_ICE_OM_NUMBER)
        {
          map<I3DOMLaunch::TriggerType,I3DOMLaunchSeriesMapPtr>::iterator 
            found = allIceTopLaunches.find(trigger);
          if(found == allIceTopLaunches.end())
            log_fatal("unknown trigger type %d",trigger);
          I3DOMLaunchSeries& series = (*(found->second))[key];
          series.push_back(iter->second);
        }
        else
        {

          map<I3DOMLaunch::TriggerType,I3DOMLaunchSeriesMapPtr>::iterator 
            found = allInIceLaunches.find(trigger);
          if(found == allInIceLaunches.end())
            log_fatal("unknown trigger type %d",trigger);
          I3DOMLaunchSeries& series = (*(found->second))[key];
          series.push_back(iter->second);
        }
      }
      else
      {
        if(find(specialDataOMs_.begin(),specialDataOMs_.end(),key) 
            != specialDataOMs_.end())
        {
          (*specialDOMs)[key].push_back(iter->second);
        }
        else if(key.GetOM() > I3DAQDecoderUtil::MAX_IN_ICE_OM_NUMBER)
        {
          map<I3DOMLaunch::TriggerType,I3DOMLaunchSeriesMapPtr>::iterator 
            found = allIceTopMinBiasLaunches.find(trigger);
          if(found == allIceTopMinBiasLaunches.end())
            log_fatal("unknown trigger type %d",trigger);
          I3DOMLaunchSeries& series = (*(found->second))[key];
          series.push_back(iter->second);
        }
        else
        {
          map<I3DOMLaunch::TriggerType,I3DOMLaunchSeriesMapPtr>::iterator 
            found = allInIceMinBiasLaunches.find(trigger);
          if(found == allInIceMinBiasLaunches.end())
            log_fatal("unknown trigger type %d",trigger);
          I3DOMLaunchSeries& series = (*(found->second))[key];
          series.push_back(iter->second);

        }
      }
    }
  }

  if(configuration.waveforms)
  {
    bool k = false;
    for(vector<I3DOMLaunch::TriggerType>::const_iterator iter = 
        allTriggerTypes_.begin() ; 
        iter != allTriggerTypes_.end() ; 
        iter++)	
    {
      string triggerID = GetTriggerID(*iter);
      for(vector<I3DOMLaunch::TriggerMode>::const_iterator liter =
            allTriggerModes_.begin();
          liter != allTriggerModes_.end();
          liter++)
      {
        string triggerMode = GetTriggerMode(*liter);
        if(!triggerID.empty())
        {
          frame.Put(inIceID_ + triggerID, allInIceLaunches[*iter]);
          frame.Put(iceTopID_ + triggerID, allIceTopLaunches[*iter]);
        }
        if(!triggerMode.empty() && !triggerID.empty() && !k)
        {
          frame.Put(inIceID_ + triggerMode, allInIceMinBiasLaunches[*iter]);
          frame.Put(iceTopID_  + triggerMode, allIceTopMinBiasLaunches[*iter]);
          k = true;
        }
      }
    }
    if(!specialDataID_.empty())
    {
      frame.Put(specialDataID_,specialDOMs);

    }
  }
  else
  {
    log_debug("not putting DOM launches in frame we are not decoding");
  }

  if(configuration.trigger)
  {
    frame.Put(triggerID_,triggers);
  }
  else
  {
    log_debug("not creating a trigger hierarchy because trigger decoding is turned off");
  }

  if(!headerID_.empty())
  {
    frame.Put(headerID_,header);
  }
  else
  {
    log_debug("not putting event header in frame");
  }
  return header->GetStartTime();
}
