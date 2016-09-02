#include <payload-parsing/decode.h>
#include <payload-parsing/utility.h>
#include <payload-parsing/delta_decode.h>

using namespace std;


namespace payload_parsing
{

  template <>
    void do_payload_decode<18>(DecodeTarget<18>::type& target,
        const DecodeConfiguration& config,
        const vector<char>& eventData,
        unsigned start)
    {
      log_trace("decoding a type 18");


      // decoding mainboard ID
      uint64_t mbid = decode<uint64_t>(BIG, start + 28, eventData);

      // cout << "MBIDINDECIMAL: " << mbid << endl;

      // now lets get the DOM Launch time:
      uint64_t DOMLaunchTime = decode<uint64_t>(BIG, start + 8, eventData);

      uint16_t HitFlags = decode<uint16_t>(BIG, start + 40, eventData);

      //Pedestal subtraction done ('1') or not done ('0') in DOM
      unsigned pedestal = ((HitFlags >> 0) & 0x01);
      //ChargeStamp is in-ice ('0') or IceTop ('1')
      unsigned WhichChargeStamp = ((HitFlags >> 1) & 0x01);

      double launchTime = 0; // launch time - trigger time
      launchTime = DOMLaunchTime - config.globalTime;
      launchTime = launchTime / 10;  // ns


      I3DOMLaunch launch;

      launch.SetStartTime(launchTime);

      if(pedestal == 1)
      {
        launch.SetIsPedestalSub(true);
      }

      delta_decode(start, eventData, launch, WhichChargeStamp);
      target.push_back(pair<uint64_t, I3DOMLaunch>(mbid, launch));

      log_debug("payloadType: %u", decode<uint32_t>(BIG, start+4, eventData));
    }
};
