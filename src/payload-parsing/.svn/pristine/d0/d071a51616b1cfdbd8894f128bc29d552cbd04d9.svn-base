#ifndef PAYLOAD_PARSING_DECODE_H
#define PAYLOAD_PARSING_DECODE_H

#include <list>
#include <vector>
#include <utility>


#include <icetray/I3Logging.h>
#include <icetray/OMKey.h>
#include <interfaces/I3OMKey2MBID.h>
#include <dataclasses/physics/I3DOMLaunch.h>
#include <dataclasses/physics/I3TriggerHierarchy.h>
#include <dataclasses/physics/I3Trigger.h>
#include <dataclasses/physics/I3EventHeader.h>
#include <payload-parsing/utility.h>


namespace payload_parsing
{
  typedef std::list<std::pair<uint64_t,I3DOMLaunch> > DOMLaunches;
  //Defining a new struture for the V5 and V6 payloads...
  typedef std::list<std::pair<OMKey,I3DOMLaunch> > NewDOMLaunches;
  /**
   * @brief A class to hold the configuration of how we're decoding.
   */
  struct DecodeConfiguration
  {
    uint64_t globalTime;
    unsigned year;
    bool trigger;
    bool waveforms;

    DecodeConfiguration(uint64_t globalTimeIn,
        unsigned yearIn, bool triggerIn, bool waveformsIn) :
      globalTime(globalTimeIn),
      year(yearIn),
      trigger(triggerIn),
      waveforms(waveformsIn){}
  };


  /**
   * @brief template class which encapsulates the expected stuff to come out
   * of each payload.  The base template is not defined, so it must
   * be specialized to be used.
   */
  template <uint32_t payloadType>
    struct DecodeTarget;

  // For type '13', the whole event
  template<>
    struct DecodeTarget<13>
    {
      struct type
      {
        DOMLaunches& launches;
        I3TriggerHierarchy& triggers;
        I3EventHeader& header;

        type(DOMLaunches& launchesIn,
            I3TriggerHierarchy& triggersIn,
            I3EventHeader& headerIn) : 
          launches(launchesIn),
          triggers(triggersIn),
          header(headerIn){}
      };
    };

  // For Type '19', the whole event for V3 events
  template<>
    struct DecodeTarget<19>
    {
      struct type
      {
        DOMLaunches& launches;
        I3TriggerHierarchy& triggers;
        I3EventHeader& header;

        type(DOMLaunches& launchesIn,
            I3TriggerHierarchy& triggersIn,
            I3EventHeader& headerIn) : 
          launches(launchesIn),
          triggers(triggersIn),
          header(headerIn){}
      };
    };

  // For Type '20', the whole event for V3 events
  template<>
    struct DecodeTarget<20>
    {
      struct type
      {
        DOMLaunches& launches;
        I3TriggerHierarchy& triggers;
        I3EventHeader& header;

        type(DOMLaunches& launchesIn,
            I3TriggerHierarchy& triggersIn,
            I3EventHeader& headerIn) : 
          launches(launchesIn),
          triggers(triggersIn),
          header(headerIn){}
      };
    };

  // For Type '21', the whole event for V5 events
  template<>
    struct DecodeTarget<21>
    {
      struct type
      {
        NewDOMLaunches& launches;
        I3TriggerHierarchy& triggers;
        I3EventHeader& header;

        type(NewDOMLaunches& launchesIn,
            I3TriggerHierarchy& triggersIn,
            I3EventHeader& headerIn) : 
          launches(launchesIn),
          triggers(triggersIn),
          header(headerIn){}
      };
    };

  // For Type '22', the whole event for V6 events
  template<>
    struct DecodeTarget<22>
    {
      struct type
      {
        NewDOMLaunches& launches;
        I3TriggerHierarchy& triggers;
        I3EventHeader& header;

        type(NewDOMLaunches& launchesIn,
            I3TriggerHierarchy& triggersIn,
            I3EventHeader& headerIn) : 
          launches(launchesIn),
          triggers(triggersIn),
          header(headerIn){}
      };
    };


  // for type '10', a single DOM hit
  template<>
    struct DecodeTarget<10>
    {
      typedef DOMLaunches type;
    };

  // light engeneering format record
  template<>
    struct DecodeTarget<26>
    {
      typedef NewDOMLaunches type;
    };

  // for type '11', a bunch of DOM hits
  template<>
    struct DecodeTarget<11>
    {
      typedef DOMLaunches type;
    };

//   template<>
//     struct DecodeTarget<23>
//     {
//       typedef DOMLaunches type;
//     };

// for type '9' representing the trigger
  template<>
    struct DecodeTarget<9>
    {
      struct type
      {
        I3TriggerHierarchy& triggers;
        I3TriggerHierarchy::iterator iter;

        type(I3TriggerHierarchy& triggersIn,
            I3TriggerHierarchy::iterator iterIn) : 
          triggers(triggersIn),
          iter(iterIn) {}
      };
    };

  // for type '91' representing the trigger new light
  template<>
    struct DecodeTarget<91>
    {
      struct type
      {
        I3TriggerHierarchy& triggers;
        I3TriggerHierarchy::iterator iter;

        type(I3TriggerHierarchy& triggersIn,
            I3TriggerHierarchy::iterator iterIn) : 
          triggers(triggersIn),
          iter(iterIn) {}
      };
    };

  // for type '17' 
  template<>
    struct DecodeTarget<17>
    {
      typedef DOMLaunches type;
    };

  // for type '18', delta-compressed payloads 
  template<>
    struct DecodeTarget<18>
    {
      typedef DOMLaunches type;
    };

  // light delta-compressed hit record
  template<>
    struct DecodeTarget<25>
    {
      typedef NewDOMLaunches type;
    };


  // zlib compressed data decompression:
  template<>
    struct DecodeTarget<27>
    {
      typedef NewDOMLaunches type;
    };

  // for type '3', the raw capture from the string processor or testdaq
  // I'm not sure which
  template<>
    struct DecodeTarget<3>
    {
      typedef struct{
        uint64_t mbid;
        I3DOMLaunch launch;
      } type;
    };


  /**
   * @brief decodes a payload of type payloadType into the given
   * target with the given configuration, from the eventData.  
   * The extraction should start at 'start'.  This function must 
   * be specialized.
   */
  template <uint32_t payloadType>
    void do_payload_decode(typename DecodeTarget<payloadType>::type& target,
        const DecodeConfiguration& config,
        const std::vector<char>& eventData,
        unsigned start);

  /**
   * This template is what you should call in your code to decode a payload.
   * It calls do_payload_decode after some error checking.
   */
  template <unsigned payloadType>
    void decode_payload(typename DecodeTarget<payloadType>::type& target,
        const DecodeConfiguration& config,
        const std::vector<char> eventData,
        unsigned start)
    {
      // error checking
      if(eventData.size() - start < 16)  // all payloads have 16 byte header
      {
        log_fatal("The payload expected to be of type %d is not long enough "
            "to actually be a payload.  Remaining bytes is %zd",
            payloadType,eventData.size() - start);
      }

      uint32_t payloadLength = decode<uint32_t>(BIG,start,eventData);

      if (payloadType != 25 && payloadType != 26 && payloadType != 91 && 
          payloadType != 27)
      {
        if(eventData.size() - start < payloadLength)
        {
          log_fatal("The payload of type %d is expected to be %d long and "
              "there are only %zd bytes left in the buffer",
              payloadType,payloadLength,eventData.size() - start);
        }

        uint32_t payloadTypeRead = decode<uint32_t>(BIG,start + 4,eventData);

        if(payloadType != payloadTypeRead)
        {
          log_fatal("Asked to decode a payload of type %d and found payload "
              "type %d instead",payloadType,payloadTypeRead);
        }
      }

      // call the specialized decoding
      do_payload_decode<payloadType>(target, config, eventData, start);

    }

  /**
   * @brief A helper to decode an OM key from a DAQ channel ID.
   */
  inline OMKey decode_OMKey(uint16_t channelID)
  {
  	// The DAQ uses the lower 6 bits of channelID to address the
  	// OM within a string. 6 bits (0-63) are mapped to OM 1 to 64
  	// by adding +1. The upper 10 bits address the string.
  	// However, we'll add scintillator stations to IceTop at
  	// OM position 65 and 66... This needs a hack:
    if(channelID >=6000)
      return(OMKey((((channelID - 6000) >> 1) & 0x3ff) + 1,
      	           (channelID & 0x1) + 65));

    return OMKey((channelID >> 6) & 0x3ff,
                 ((channelID >> 0) & 0x3f) + 1);
  }
};

#endif
