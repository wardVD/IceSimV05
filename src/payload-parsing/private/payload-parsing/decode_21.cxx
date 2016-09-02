// Payload Type 21 is the event header for the V5 payloads.

#include <payload-parsing/decode.h>
#include <payload-parsing/utility.h>
#include <daq-decode/I3DAQDecoderUtil.h>

using namespace std;


namespace payload_parsing
{
  template<>
    void do_payload_decode<21>(DecodeTarget<21>::type& target,
        const DecodeConfiguration& configuration,
        const vector<char>& eventData,
        unsigned start)
    {
      log_trace("decoding a type 21");

      if(!(eventData.size() > start + 38))
      {
        log_fatal("got event data of size %zd when expected to be at least %d",
            eventData.size(),38);
      }

      // decoding the header
      uint64_t firstDOMTime =  decode<uint64_t>(BIG, 8 + start, eventData); 
      uint64_t lastDOMTime = decode<uint32_t>(BIG,16 + start, eventData) 
        + firstDOMTime;

      uint32_t eventID = decode<uint32_t>(BIG, 22 + start, eventData);
      uint32_t runNumber = decode<uint32_t>(BIG, 26 + start, eventData);

      if (configuration.year != 0)
      {
        log_warn("You set the year parameter but you are decoding a type 21"
            " which has the year parameter encoded in the payload."
            " overwriting the set parameter with the value from the"
            " payload");
      }

      uint16_t decodedYear = decode<uint16_t>(BIG, 20 + start, eventData);
      int32_t subRunNumber = decode<uint32_t>(BIG, 30 + start, eventData);

      DecodeConfiguration config(configuration.globalTime, decodedYear,
          configuration.trigger, configuration.waveforms);

      unsigned runInTransition;

      if (subRunNumber < 0)
      {
        runInTransition = I3EventHeader::CONFIG_IN_TRANSITION;
        target.header.SetState
          (static_cast<I3EventHeader::State>(runInTransition));
      }

      else if (subRunNumber > (-1))
      {
        runInTransition = I3EventHeader::OK;
        target.header.SetState
          (static_cast<I3EventHeader::State>(runInTransition));
      }

      target.header.SetStartTime(I3Time(config.year, firstDOMTime));
      target.header.SetEndTime(I3Time(config.year, lastDOMTime));
      target.header.SetRunID(runNumber);
      // set SubRun ID ... use 0, since it is not supported by the payload system so far
      // 0 will be the only SubRun ID for physics run
      target.header.SetSubRunID(abs(subRunNumber));    
      target.header.SetEventID(eventID);  

      // number of composite payloads
      uint32_t nrOfHitRecords = decode<uint32_t>(BIG, 34 + start, eventData);

      unsigned compositeStart = start + 38;

      for(unsigned i = 0 ; i < nrOfHitRecords ; i++)
      {
        uint16_t payloadLength = 
          decode<uint16_t>(BIG,start + compositeStart,eventData);
        //Hit record Type (=1 for delta compressed hits, =0 for engeneering)
        uint8_t hitRecordType = 
          decode<uint8_t>(BIG,start + compositeStart + 2, eventData);

        DecodeTarget<25>::type& target_25 = target.launches;
        DecodeTarget<26>::type& target_26 = target.launches;

        if (config.waveforms)
        {
          if (hitRecordType == 1)
          {
            decode_payload<25>(target_25, config, eventData, compositeStart);
          }
          else if (hitRecordType == 0)
          {
            decode_payload<26>(target_26, config, eventData, compositeStart);
          }
          else
          {
            log_fatal("this decoder can only decode delta compressed payloads"
                "or engineering payloads. Hit record has to be 0 or 1"
                "but was found to be %u",
                hitRecordType);
          }
        }
        compositeStart = compositeStart + payloadLength;
      }

      uint32_t nrTriggerRecords = 
        decode<uint32_t>(BIG, compositeStart + start, eventData);

      compositeStart = compositeStart + 4;
      if (config.trigger)
      {
        for (unsigned i = 0 ; i < nrTriggerRecords ; i++)
        {
          DecodeTarget<91>::type target_91(target.triggers,
              I3TriggerHierarchy::iterator());
          decode_payload<91>(target_91, config, eventData, compositeStart);

          uint32_t numberOfHitIndices = decode<uint32_t>(BIG,
                start + compositeStart + 20, eventData);
          compositeStart = compositeStart + 24 + numberOfHitIndices * 4;
        }
      }
    }
};

