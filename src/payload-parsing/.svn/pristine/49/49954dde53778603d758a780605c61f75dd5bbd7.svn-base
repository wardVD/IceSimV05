// Payload type 26 contains the engeneering records for a V5 payload

#include <payload-parsing/decode.h>
#include <payload-parsing/utility.h>
#include <payload-parsing/dump-raw-engineering.h>
#include <phys-services/I3FileOMKey2MBID.h>

using namespace std;

namespace payload_parsing
{

  template <>
    void do_payload_decode<26>(DecodeTarget<26>::type& target,
        const DecodeConfiguration& config,
        const vector<char>& eventData,
        unsigned start)
    {
      log_trace("decoding a type 26");

      // now lets get the DOM Launch time:
      uint64_t DOMLaunchTime = decode<uint32_t>(BIG, start + 6, eventData);

      double launchTime = 0;
      //Ligther payloads have the time with respect to evelope time
      //no need to subtract the global time here.
      launchTime = DOMLaunchTime ;
      launchTime = launchTime / 10;  // ns

      uint16_t ChannelID = decode<uint16_t>(BIG, start + 4, eventData);

      I3DOMLaunch launch;
      launch.SetStartTime(launchTime);

      dump_raw_engineering(start + 6, eventData, launch);
      target.push_back(pair<OMKey, I3DOMLaunch>(decode_OMKey(ChannelID), launch));

      log_debug("payloadType: %u", decode<uint32_t>(BIG, start+4, eventData));
    }
}
