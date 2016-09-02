/* This payload contains the information for the V2 event header */

#include <payload-parsing/decode.h>
#include <payload-parsing/utility.h>


using namespace std;


namespace payload_parsing
{
  template<>
    void do_payload_decode<13>(DecodeTarget<13>::type& target,
        const DecodeConfiguration& configuration,
        const vector<char>& eventData,
        unsigned start)
    {
      log_trace("decoding a type 13");

      DecodeConfiguration config(configuration.globalTime, configuration.year,
          configuration.trigger, configuration.waveforms);

      if(!(eventData.size() > start + 62))
      {
        log_fatal("got event data of size %zd when expected to be at least %d",
            eventData.size(),62);
      }

      if (config.year == 0)
      {
        log_fatal("You have to set the year for this type of payload!");
      }

      // decoding the header
      uint64_t firstDOMTime =  decode<uint64_t>(BIG, 26 + start, eventData);
      uint64_t lastDOMTime = decode<uint64_t>(BIG, 34 + start, eventData);
      uint32_t runNumber = decode<uint32_t>(BIG, 50 + start, eventData);
      uint32_t eventID = decode<uint32_t>(BIG, 18 + start, eventData);

      target.header.SetStartTime(I3Time(config.year, firstDOMTime));
      target.header.SetEndTime(I3Time(config.year, lastDOMTime));
      target.header.SetRunID(runNumber);
      // set SubRun ID ... use 0, since it is not supported by the payload system so far
      // 0 will be the only SubRun ID for physics run
      target.header.SetSubRunID(0u);    
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

