// Payload type 27 contains the delta-compressed hit records for a V6 payload

#include <payload-parsing/decode.h>
#include <payload-parsing/utility.h>
#include <payload-parsing/delta_decode.h>
#include <phys-services/I3FileOMKey2MBID.h>

using namespace std;


namespace payload_parsing
{

  template <>
    void do_payload_decode<27>(DecodeTarget<27>::type& target,
        const DecodeConfiguration& config,
        const vector<char>& zlibWaveforms,
        unsigned start)
    {
      log_trace("decoding a type 27");

      // now lets get the DOM Launch time:
      uint64_t DOMLaunchTime = decode<uint32_t>(BIG, start + 6, zlibWaveforms);

      //    cout << DOMLaunchTime << endl;
      uint16_t HitFlags = decode<uint8_t>(BIG, start + 3, zlibWaveforms);
      uint16_t ChannelID = decode<uint16_t>(BIG, start + 4, zlibWaveforms);

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
      delta_decode(start - 40, zlibWaveforms, launch, WhichChargeStamp);
      target.push_back(pair<OMKey,I3DOMLaunch>(decode_OMKey(ChannelID), launch));

      log_debug("payloadType: %u", decode<uint32_t>(BIG, start+4, zlibWaveforms));
    }
};
