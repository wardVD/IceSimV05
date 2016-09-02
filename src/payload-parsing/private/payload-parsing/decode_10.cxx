/*the type 10 payload is a V2 Engeneering Waveform*/

#include <payload-parsing/decode.h>
#include <payload-parsing/utility.h>
#include <payload-parsing/dump-raw-engineering.h>


using namespace std;


namespace payload_parsing
{
  template <>
    void do_payload_decode<10>(DecodeTarget<10>::type& target,
        const DecodeConfiguration& config,
        const vector<char>& eventData,
        unsigned start)
    {
      log_trace("decoding a type 10");

      // decoding mainboard ID
      uint64_t mbid = decode<uint64_t>(BIG, start + 32, eventData);

      // now lets get the DOM Launch time:
      uint64_t DOMLaunchTime = decode<uint64_t>(BIG, start + 48, eventData);

      double launchTime = 0; // launch time - trigger time
      launchTime = DOMLaunchTime - config.globalTime;
      launchTime = launchTime / 10;  // ns

      I3DOMLaunch launch;

      launch.SetStartTime(launchTime);

      //This check is only valid for V4-Payloads and hence was moved
      //from dump-raw-engineering format to the decode_10 since
      //decode_10 is only called for Versions < 5 while dum_raw_engineering
      //is called for all versions.
      uint16_t rawformat = decode<uint16_t>(BIG,start + 58, eventData);

      switch(rawformat) 
      {
        case 0x0100: 
          log_trace("format = 1, le = 0");
          log_fatal("data has wrong endianesss");
          break;
        case 0x0200:       
          log_trace("format = 2, le = 0");
          log_fatal("data has wrong endianesss");
          break;
      }

      dump_raw_engineering(start + 56, eventData, launch);
      target.push_back(pair<uint64_t, I3DOMLaunch>(mbid, launch));
    }
};
