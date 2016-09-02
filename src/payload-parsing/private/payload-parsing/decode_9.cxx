/* The actual Trigger Hierarchy is stored in the payload of 
   type 9 for Versions 1-4*/

#include <payload-parsing/decode.h>
#include <payload-parsing/utility.h>
#include <daq-decode/I3DAQDecoderUtil.h>


using namespace std;


namespace payload_parsing
{
  // patterned after I3DAQEventDecoder06v1::FillTriggerHierarchy
  template <>
    void do_payload_decode<9>(DecodeTarget<9>::type& target,
        const DecodeConfiguration& config,
        const vector<char>& eventData,
        unsigned start)
    {
      log_trace("decoding a type 9");

      uint64_t firstTime = decode<uint64_t>(BIG, start + 34, eventData);
      uint64_t lastTime = decode<uint64_t>(BIG, start + 42, eventData);

      double trigTime = 0.0;
      if(config.year < 2007)
        trigTime = (double)(firstTime / 10.0); // atto -> nano
      else
        trigTime = (double)((firstTime - config.globalTime) / 10.0); // atto -> nano

      double trigLength = (double)((lastTime - firstTime) / 10.0); // atto -> nano

      int32_t trigType = decode<int32_t>(BIG, start + 22, eventData);
      int32_t trigConfId = decode<int32_t>(BIG, start + 26, eventData);
      uint32_t trigSrcId = decode<uint32_t>(BIG, start + 30, eventData);


      I3Trigger trig;

      I3DAQDecoderUtil::FillTrigger(trig, 
          trigTime, 
          trigLength, 
          trigType, 
          trigConfId, 
          trigSrcId);

      target.iter = 
        I3DAQDecoderUtil::AddTrigger(target.triggers,target.iter,trig);

      if(((int32_t)trigSrcId == I3DAQDecoderUtil::GLOBAL_TRIG_ID) 
          || (trigType == I3DAQDecoderUtil::MERGED_TRIG_TYPE))
      {
        uint32_t nReadoutRequest = decode<uint32_t>(BIG, start + 60, eventData);
        unsigned locationNComposites = 70 + (32 * nReadoutRequest);
        uint16_t nComposite = decode<uint16_t>(BIG, start + locationNComposites, 
            eventData);
        unsigned compositeStart = 72 + (32 * nReadoutRequest) + start;

        for(unsigned i = 0 ; i < nComposite ; i++)
        {
          int32_t payloadLength = decode<uint32_t>(BIG, compositeStart, eventData);
          int32_t payloadType = decode<uint32_t>(BIG, compositeStart + 4, eventData);

          if(payloadType == 9)
          {
            decode_payload<9>(target, config, eventData, compositeStart);
          }
          else if (payloadType == 1)
          {
            log_trace("skipping a payload '1' 'cause I don't care");
          }
          else
          {
            log_fatal("unexpected payload type %u",payloadType);
          }

          compositeStart = compositeStart + payloadLength;
        }
      }
    }
};
