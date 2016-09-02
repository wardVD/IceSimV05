// Payload Type 22 is the event header for the V6 payloads.

#include <payload-parsing/decode.h>
#include <payload-parsing/utility.h>
#include <daq-decode/I3DAQDecoderUtil.h>
#include <assert.h>
#include "zlib.h"

using namespace std;

namespace payload_parsing
{
  template<>
    void do_payload_decode<22>(DecodeTarget<22>::type& target,
        const DecodeConfiguration& configuration,
        const vector<char>& eventData,
        unsigned start)
    {
      log_trace("decoding a type 22");

      if(!(eventData.size() > start + 39))
      {
        log_fatal("got event data of size %zd when expected to be at least %d",
            eventData.size(),39);
      }

      // decoding the header
      uint64_t firstDOMTime = decode<uint64_t>(BIG, 8 + start, eventData); 
      uint64_t lastDOMTime = 
        decode<uint32_t>(BIG,16 + start, eventData) + firstDOMTime;

      uint32_t eventID = decode<uint32_t>(BIG, 22 + start, eventData);
      uint32_t runNumber = decode<uint32_t>(BIG, 26 + start, eventData);

      if (configuration.year != 0)
      {
        log_warn("You set the year parameter but you are decoding a type 22"
            " which has the year parameter encoded in the payload."
            " overwriting the set parameter with the value from the"
            " payload");
      }

      uint16_t decodedYear = decode<uint16_t>(BIG, 20 + start, eventData);
      int32_t subRunNumber = decode<uint32_t>(BIG, 30 + start, eventData);

      DecodeConfiguration config(configuration.globalTime, decodedYear,
          configuration.trigger, configuration.waveforms);

      unsigned runInTransition;

      if (subRunNumber < 0)
      {
        runInTransition = I3EventHeader::CONFIG_IN_TRANSITION;
        target.header.SetState
          (static_cast<I3EventHeader::State>(runInTransition));
      }

      else if (subRunNumber > (-1))
      {
        runInTransition = I3EventHeader::OK;
        target.header.SetState
          (static_cast<I3EventHeader::State>(runInTransition));
      }

      target.header.SetStartTime(I3Time(config.year, firstDOMTime));
      target.header.SetEndTime(I3Time(config.year, lastDOMTime));
      target.header.SetRunID(runNumber);
      // set SubRun ID ... use 0, since it is not supported by the payload system so far
      // 0 will be the only SubRun ID for physics run
      target.header.SetSubRunID(abs(subRunNumber));    
      target.header.SetEventID(eventID);  

      uint8_t zlibCompressionFlag = decode<uint8_t>(BIG, 34 + start, eventData);


      if (zlibCompressionFlag == 0)
      {
        // number of composite payloads
        uint32_t nrOfHitRecords = decode<uint32_t>(BIG, 35 + start, eventData);

        unsigned compositeStart = start + 39;
        for(unsigned i = 0 ; i < nrOfHitRecords ; i++)
        {
          uint16_t payloadLength = 
            decode<uint16_t>(BIG, start + compositeStart, eventData);
          //Hit record Type (=1 for delta compressed hits, =0 for engeneering)
          uint8_t hitRecordType = 
            decode<uint8_t>(BIG,start + compositeStart + 2, eventData);

          DecodeTarget<25>::type& target_25 = target.launches;
          DecodeTarget<26>::type& target_26 = target.launches;

          if (config.waveforms)
          {

            if (hitRecordType == 1)
            {
              decode_payload<25>(target_25, config, eventData, compositeStart);
            }
            else if (hitRecordType == 0)
            {
              decode_payload<26>(target_26, config, eventData, compositeStart);
            }
            else
            {
              log_fatal("this decoder can only decode delta compressed payloads"
                  "or engineering payloads. Hit record has to be 0 or 1"
                  "but was found to be %u",
                  hitRecordType);
            }
          }
          compositeStart = compositeStart + payloadLength;

        }


        uint32_t nrTriggerRecords = 
          decode<uint32_t>(BIG, compositeStart + start, eventData);

        compositeStart = compositeStart + 4;

        if(config.trigger)
        {
          for (unsigned i = 0 ; i < nrTriggerRecords ; i++)
          {

            DecodeTarget<91>::type target_91(target.triggers,
                I3TriggerHierarchy::iterator());
            decode_payload<91>(target_91, config, eventData, compositeStart);

            uint32_t numberOfHitIndices = decode<uint32_t>
              (BIG, start + compositeStart + 20, eventData);
            compositeStart = compositeStart + 24 + numberOfHitIndices * 4;
          }
        }
      }
      else if(zlibCompressionFlag == 1)
      {
        //The Trigger Records are compressed using zlib...
        uint32_t numberOfCompressedBytes =
          decode<uint32_t>(BIG, start + 35, eventData);
        if (config.waveforms)
        {
          unsigned char in[numberOfCompressedBytes];
          // the zlib uncompression algorithm expects an array of compressed bytes
          for(unsigned i = 0 ; i < numberOfCompressedBytes  ; i++)
          {
            in[i] = eventData[i + start + 39];
          }

          //setting up the zlib decompression algorithm.
          //for details please see: www.zlib.com
          int ret;
          unsigned have;
          z_stream strm;
          /* allocate inflate state */
          strm.zalloc = Z_NULL;
          strm.zfree = Z_NULL;
          strm.opaque = Z_NULL;
          strm.avail_in = 0;
          strm.next_in = Z_NULL;

          //passing the option -15 to inflateInit2 does 2 things.
          //1: the minus sign tells the inflater that there is no
          //zlib header. Dave wrote the deflation algortihm without
          //the header to save 2 bytes.
          //2: the 15 tells the the inflation algorithm that both
          //header bytes were left out of the deflation algorithm
          //(start counting at 0).
          ret = inflateInit2(&strm, -15);
          if (ret != Z_OK)
          {
            log_warn("Inflation State did not initialize to OK!");
          }

          // We need a place to put the decompressed data. Here
          // we make the output buffer 4 times as large as the
          // input buffer. This ensures that the buffer is big
          // enough to hold all of the output data.
          int CHUNK = numberOfCompressedBytes * 4;
          unsigned char out[CHUNK];
          int totalsize = 0;

          strm.avail_in = numberOfCompressedBytes;
          strm.next_in = in;
          /* run inflate() on input until output buffer not full */
          do {
            strm.avail_out = CHUNK;
            strm.next_out = out;
            ret = inflate(&strm, Z_NO_FLUSH);
            assert(ret != Z_STREAM_ERROR);  /* state not clobbered */
            switch (ret) {
              case Z_NEED_DICT:
              case Z_DATA_ERROR:
              case Z_MEM_ERROR:
                (void)inflateEnd(&strm);
            }
            have = CHUNK - strm.avail_out;
            totalsize += have;
          } while (strm.avail_out == 0);
          (void)inflateEnd(&strm);
          /* done when inflate() says it's done */

          vector<char> uncompressedWaveforms;
          uncompressedWaveforms.resize(have);

          for (unsigned int m = 0 ; m < have ; m++)
          {
            uncompressedWaveforms[m] = out[m];
          }

          // Zlib decompression complete. Now to decode the waveforms...
          uint32_t unCompressedStart = 4;
          uint32_t nrOfHitRecords = 
            decode<uint32_t>(BIG, 0, uncompressedWaveforms);

          for(unsigned i = 0 ; i < nrOfHitRecords ; i++)
          {
            uint16_t payloadLength = 
              decode<uint16_t>(BIG,unCompressedStart , uncompressedWaveforms);
            //Hit record Type (=1 for delta compressed hits, =0 for engeneering)
            uint8_t hitRecordType = 
              decode<uint8_t>(BIG,unCompressedStart + 2, uncompressedWaveforms);

            DecodeTarget<26>::type& target_26 = target.launches;
            DecodeTarget<27>::type& target_27 = target.launches;

            if (hitRecordType == 1)
            {
              decode_payload<27>(target_27,
                  config,
                  uncompressedWaveforms,
                  unCompressedStart);
            }
            else if (hitRecordType == 0)
            {
              decode_payload<26>(target_26,
                  config,
                  uncompressedWaveforms,
                  unCompressedStart);
            }
            else
            {
              log_fatal("this decoder can only decode delta compressed payloads"
                  "or engineering payloads. Hit record has to be 0 or 1"
                  "but was found to be %u",
                  hitRecordType);
            }

            unCompressedStart = unCompressedStart + payloadLength;
          }
        }

        uint32_t nrTriggerRecords = 
          decode<uint32_t>(BIG, 
              start + 39 + numberOfCompressedBytes,
              eventData);
        uint32_t compositeStart = numberOfCompressedBytes + 43;
        if (config.trigger)
        {
          for (unsigned i = 0 ; i < nrTriggerRecords ; i++)
          {

            DecodeTarget<91>::type target_91(target.triggers,
                I3TriggerHierarchy::iterator());

            decode_payload<91>(target_91,
                config,
                eventData,
                compositeStart + start);

            uint32_t numberOfHitIndices = 
              decode<uint32_t>(BIG,
                  start + compositeStart + 20,
                  eventData);
            compositeStart = compositeStart + 24 + numberOfHitIndices * 4;
          }
        }
      }
    }
};

