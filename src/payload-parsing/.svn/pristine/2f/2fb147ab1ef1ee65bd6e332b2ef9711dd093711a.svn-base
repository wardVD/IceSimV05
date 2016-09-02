#include <payload-parsing/decode.h>
#include <payload-parsing/utility.h>
#include <payload-parsing/dump-raw-deltacompressed.h>

using namespace std;


namespace payload_parsing
{

  template <>
    void do_payload_decode<17>(DecodeTarget<17>::type& target,
        const DecodeConfiguration& config,
        const vector<char>& eventData,
        unsigned start)
    {
      log_trace("decoding a type 17");


      // decoding mainboard ID
      uint64_t mbid = decode<uint64_t>(BIG, start + 32, eventData);

      // now lets get the DOM Launch time:
      uint64_t DOMLaunchTime = decode<uint64_t>(BIG, start + 48, eventData);

      double launchTime = 0; // launch time - trigger time
      launchTime = DOMLaunchTime - config.globalTime;
      launchTime = launchTime / 10;  // ns

      I3DOMLaunch launch;

      launch.SetStartTime(launchTime);

      dump_delta_compressed(start, eventData, launch);
      target.push_back(pair<uint64_t,I3DOMLaunch>(mbid, launch));

      log_debug("payloadType: %u", decode<uint32_t>(BIG, start+4, eventData));
    }
};
