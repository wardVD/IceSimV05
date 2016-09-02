// Payload type 25 contains the V5 Hit records including OMKeys, and the HitFlags

#include <payload-parsing/decode.h>
#include <payload-parsing/utility.h>
#include <payload-parsing/delta_decode.h>
#include <string> 
#include <phys-services/I3FileOMKey2MBID.h>

using namespace std;


namespace payload_parsing
{

  template <>
    void do_payload_decode<25>(DecodeTarget<25>::type& target,
        const DecodeConfiguration& config,
        const vector<char>& eventData,
        unsigned start)
    {
      log_trace("decoding a type 25");
      uint8_t hitRecordType = decode<uint8_t>(BIG, start + 2, eventData);

      if (hitRecordType != 1)
      {
        log_fatal("the hitRecordType Flag should be 1 for delta compression");
      }

      // now lets get the DOM Launch time:
      uint64_t DOMLaunchTime = decode<uint32_t>(BIG, start + 6, eventData);
      uint8_t HitFlags = decode<uint8_t>(BIG, start + 3, eventData);
      uint16_t ChannelID = decode<uint16_t>(BIG, start + 4, eventData);

      //Pedestal subtraction done ('1') or not done ('0') in DOM
      unsigned pedestal = ((HitFlags >> 0) & 0x01);

      //ChargeStamp is in-ice ('0') or IceTop ('1')
      unsigned WhichChargeStamp = ((HitFlags >> 1) & 0x01);

      double launchTime = 0; // launch time - trigger time
      //for the lighter paylaods the DOMLaunch time is with respect
      //to the global time
      //already so here we don't have to subtract the global time!
      launchTime = DOMLaunchTime;
      launchTime = launchTime / 10;  // ns

      I3DOMLaunch launch;
      launch.SetStartTime(launchTime);
      if(pedestal == 1)
      {
        launch.SetIsPedestalSub(true);
      }

      //The new lean header is 40 bytes shorter then the old header.
      //This means that the waveforms are found 38 bytes sooner than
      //before. To reuse the code already written we have to look
      //40 bytes earlier. hence: start - 40.
      delta_decode(start - 40, eventData, launch, WhichChargeStamp);
      target.push_back(pair<OMKey,I3DOMLaunch>(decode_OMKey(ChannelID), launch));

      log_debug("payloadType: %u", decode<uint32_t>(BIG, start + 4, eventData));
    }
};
