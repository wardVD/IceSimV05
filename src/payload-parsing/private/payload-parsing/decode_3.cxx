#include <getopt.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include <payload-parsing/decode.h>
#include <payload-parsing/utility.h>
#include <payload-parsing/delta_decode.h>
#include <payload-parsing/dump-raw-deltacompressed.h>

using namespace std;


namespace payload_parsing
{
  template <>
    void do_payload_decode<3>(DecodeTarget<3>::type& target,
        const DecodeConfiguration& config,
        const vector<char>& eventData,
        unsigned start)
    {
      log_trace("decoding a type 3");

      uint32_t size = decode<uint32_t>(BIG, start, eventData);
      log_debug("payloadType: %u", decode<uint32_t>(BIG, start + 4, eventData));
      target.mbid = decode<uint64_t>(BIG, start + 8, eventData);
      uint64_t DOMLaunchTime = decode<uint64_t>(BIG, start + 24, eventData);

      uint16_t byteOrder = decode<uint16_t>(BIG, start + 32, eventData);
      if(byteOrder != 1) //Oops, apparently everything is wrong
        log_fatal("Attempted decode payload of type 3 with invalid BOM: %i", 
            byteOrder);

      uint16_t version = decode<uint16_t>(BIG, start + 34, eventData);
      if(version != 2)
        log_fatal("Attempted decode payload of type 3 with version number other than 2: %i", 
            version);

      uint16_t HitFlags = decode<uint16_t>(BIG, start + 36, eventData);
      //Pedestal subtraction done ('1') or not done ('0') in DOM
      bool pedestal = ((HitFlags >> 0) & 0x01);
      target.launch.SetIsPedestalSub(pedestal);
      //ChargeStamp is in-ice ('0') or IceTop ('1')
      unsigned WhichChargeStamp = ((HitFlags >> 1) & 0x01);
      double launchTime = 0; // launch time - trigger time
      launchTime = DOMLaunchTime - config.globalTime;
      launchTime = launchTime / 10;  // ns
      target.launch.SetStartTime(launchTime);

      //this is very dumb, but delta_decode expects to find header word 1
      //50 bytes after the start it is given, but in type 3 payloads it is located at offset 46
      //When already at an offset larger than 4 we can just subtract 4, but otherwise we need a
      //whole dummy buffer
      if(start<4){
        vector<char> shiftedEventData(4 - start + size);
        shiftedEventData.resize(4 - start); //prepend up to 4 bytes of padding
        shiftedEventData.insert(shiftedEventData.end(), eventData.begin(), 
            eventData.begin() + size);
        delta_decode(0, shiftedEventData, target.launch, WhichChargeStamp);
      }
      else{
        delta_decode(start - 4, eventData, target.launch, WhichChargeStamp);
      }
      //delta_decode sets the LC flag on the launches incorrectly, namely 
      //based on whether the ATWD and FADC were read out, rather than based on 
      //whether the DOM actually had LC. Try to clean up the mess here. 
      uint32_t word1 = decode<uint32_t>(BIG, start + 46, eventData);
      const uint32_t lcMask = 0x30000;
      target.launch.SetLCBit(word1 & lcMask);
    }
};
