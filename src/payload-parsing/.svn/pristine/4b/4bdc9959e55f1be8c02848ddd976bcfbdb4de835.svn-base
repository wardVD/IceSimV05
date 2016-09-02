// Payload type 20 is the V4 event header

#include <payload-parsing/decode.h>
#include <payload-parsing/utility.h>


using namespace std;


namespace payload_parsing
{
  template<>
    void do_payload_decode<20>(DecodeTarget<20>::type& target,
        const DecodeConfiguration& configuration,
        const vector<char>& eventData,
        unsigned start)
    {
      log_trace("decoding a type 20");

      if(!(eventData.size() > start + 62))
      {
        log_fatal("got event data of size %zd when expected to be at least %d",
            eventData.size(),62);
      }

      // decoding the header
      uint64_t firstDOMTime =  decode<uint64_t>(BIG, 26 + start, eventData);
      uint64_t lastDOMTime = decode<uint64_t>(BIG, 34 + start, eventData);
      uint32_t runNumber = decode<uint32_t>(BIG, 46 + start, eventData);
      uint32_t eventID = decode<uint32_t>(BIG, 18 + start, eventData);
      uint16_t recordType = decode<uint16_t>(BIG, 16 + start, eventData);
      if (recordType != 11)
      {
        log_fatal("This is event payload type 20 and the RecordType is NOT type 11");
      }

      if (configuration.year != 0)
      {
        log_warn("You set the year parameter but you are decoding a type 20"
            " which has the year parameter encoded in the payload."
            " overwriting the set parameter with the value from the"
            " payload");
      }

      uint16_t decodedYear = decode<uint16_t>(BIG, 42 + start, eventData);
      int32_t subRunNumber = decode<uint32_t>(BIG, 50 + start, eventData);

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
      uint16_t nComposite = decode<uint16_t>(BIG, 60 + start, eventData);


      unsigned compositeStart = 62; // this is where the composite payloads start
      for(unsigned i = 0 ; i < nComposite ; i++)
      {
        uint32_t payloadType = decode<uint32_t>(BIG, compositeStart + 4, eventData);
        uint32_t payloadLength = decode<uint32_t>(BIG, compositeStart, eventData);

        DecodeTarget<11>::type& target_11 = target.launches;
        DecodeTarget<9>::type target_9(target.triggers, I3TriggerHierarchy::iterator());

        if(payloadType == 11)
        {
          if(config.waveforms)
          {
            decode_payload<11>(target_11, config, eventData, compositeStart);
          }
          else
          {
            log_debug("waveform is not being decoded");
          }
        }
        else if(payloadType == 9)
        {
          if(config.trigger)
          {
            decode_payload<9>(target_9, config, eventData, compositeStart);
          }
          else
          {
            log_debug("trigger is not being decoded");
          }
        }
        else
        {
          log_fatal("unexpected payload type %u",payloadType);
        }

        compositeStart = compositeStart + payloadLength;
      }
    }
};

