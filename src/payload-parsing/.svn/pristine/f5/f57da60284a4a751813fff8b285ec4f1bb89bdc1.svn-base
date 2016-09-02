/*The Type 11 is the Trigger Header Payload for paylaods of
  Versions 1-4*/

#include <payload-parsing/decode.h>
#include <payload-parsing/utility.h>


using namespace std;


namespace payload_parsing
{
  template<>
    void do_payload_decode<11>(DecodeTarget<11>::type& target,
        const DecodeConfiguration& config,
        const vector<char>& eventData,
        unsigned start)
    {
      log_trace("decoding a type 11");

      if((eventData.size() - start) < 54)
      {
        log_fatal("expected the data to be at least 54, but was only %zd",
            eventData.size() - start);
      }

      uint16_t nComposites = decode<uint16_t>(BIG, start + 52, eventData);

      unsigned compositeStart = start + 54;
      for(unsigned i = 0 ; i < nComposites ; i++)
      {
        uint32_t payloadLength = decode<uint32_t>(BIG, compositeStart, eventData);
        uint32_t payloadType = decode<uint32_t>(BIG, compositeStart + 4, eventData);

        if(payloadType == 10)
        {
          decode_payload<10>(target, config, eventData, compositeStart);
        }
        else if(payloadType == 18)
        {
          decode_payload<18>(target, config, eventData, compositeStart);
        }
        else
        {
          log_fatal("can only decode type 10 here, not type %u", payloadType);
        }

        compositeStart = compositeStart + payloadLength;
      }
    }
};

