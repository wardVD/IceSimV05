/* This payload is not found in the actual DAQ. There it is
   known as the Triggers payload. I call it payload type 91 to
   keep with the templated scheme of decoding payloads.
   This payload contains all of the Trigger information for
   V5 and V6 events */

#include <payload-parsing/decode.h>
#include <payload-parsing/utility.h>
#include <daq-decode/I3DAQDecoderUtil.h>


using namespace std;


namespace payload_parsing
{
  // patterned after I3DAQEventDecoder06v1::FillTriggerHierarchy
  template <>
    void do_payload_decode<91>(DecodeTarget<91>::type& target,
        const DecodeConfiguration& config,
        const vector<char>& eventData,
        unsigned start)
    {
      log_trace("decoding a type 91");

      uint32_t firstTime = decode<uint32_t>(BIG, start + 12, eventData);
      uint32_t lastTime = decode<uint32_t>(BIG, start + 16, eventData);

      double trigTime = 0.0;
      if(config.year < 2007)
        trigTime = (double)(firstTime / 10.0); // atto -> nano
      else
        trigTime = (double)((firstTime) / 10.0); // atto -> nano

      double trigLength = (double)((lastTime - firstTime) / 10.0); // atto -> nano

      int32_t trigType = decode<int32_t>(BIG, start, eventData);
      int32_t trigConfId = decode<int32_t>(BIG, start + 4, eventData);
      uint32_t trigSrcId = decode<uint32_t>(BIG, start + 8, eventData);

      //cout << trigTime << trigLength << trigType << trigConfId << trigSrcId << endl;

      I3Trigger trig;
      I3DAQDecoderUtil::FillTrigger(trig, trigTime, trigLength, trigType, 
          trigConfId, trigSrcId);

      target.iter = 
        I3DAQDecoderUtil::AddTrigger(target.triggers, target.iter, trig);

    }
};
