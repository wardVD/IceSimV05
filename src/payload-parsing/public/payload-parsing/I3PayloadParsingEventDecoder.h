#ifndef I3PAYLOADPARSINGEVENTDECODER_H
#define I3PAYLOADPARSINGEVENTDECODER_H

#include <string>
#include <vector>

#include <icetray/I3Logging.h>
#include <icetray/OMKey.h>
#include <icetray/I3PointerTypedefs.h>
#include <dataclasses/I3Time.h>
#include <dataclasses/physics/I3DOMLaunch.h>
#include <daq-decode/I3DAQEventDecoder.h>

class I3OMKey2MBID;

class I3PayloadParsingEventDecoder : public I3DAQEventDecoder
{
  public:
    I3PayloadParsingEventDecoder(const I3OMKey2MBID&,
        unsigned year,
        const std::string& headerID,
        const std::string& triggerID,
        const std::vector<OMKey>& specialDataOMs,
        const std::string& specialDataID,
        const std::string& inIceID,
        const std::string& iceTopID,
        const std::string& speDataID,
        const std::string& flasherDataID,
        const std::string& cpuDataID,
        const std::string& onboardLEDID,
        const std::string& testDataID,
        const std::string& minBiasID);

    virtual ~I3PayloadParsingEventDecoder();

    I3Time FillEvent(I3Frame& frame,
        const std::vector<char>& data) const;

  private:
    std::vector<I3DOMLaunch::TriggerType> allTriggerTypes_;
    std::vector<I3DOMLaunch::TriggerMode> allTriggerModes_;

    const std::string& GetTriggerMode(I3DOMLaunch::TriggerMode mode) const
    {
      switch(mode)
      {
        case I3DOMLaunch::MIN_BIAS:
          return minBiasID_;
          break;
        default:
          log_fatal("Unkown Trigger Mode! %d",mode);
          break;
      }
    }

    const std::string& GetTriggerID(I3DOMLaunch::TriggerType type) const
    {
      switch(type)
      {
        case I3DOMLaunch::CPU_REQUESTED:
          return cpuDataID_;
          break;
        case I3DOMLaunch::SPE_DISCRIMINATOR_TRIGGER:
          return speDataID_;
          break;
        case I3DOMLaunch::TEST_PATTERN:
          return testDataID_;
          break;
        case I3DOMLaunch::FLASHER_BOARD_TRIGGER:
          return flasherDataID_;
          break;
        case I3DOMLaunch::ONBOARD_LED:
          return onboardLEDID_;
          break;
        default:
          log_fatal("Unknown trigger type %d",type);                            
      }
    }

    const I3OMKey2MBID& omkey2mbid;
    unsigned year_;
    std::string headerID_;
    std::string triggerID_;
    std::vector<OMKey> specialDataOMs_;
    std::string specialDataID_;
    std::string inIceID_;
    std::string iceTopID_;
    std::string speDataID_;
    std::string flasherDataID_;
    std::string cpuDataID_;
    std::string onboardLEDID_;
    std::string testDataID_;
    bool trigger_;
    std::string minBiasID_;
    bool waveforms_;

    SET_LOGGER("I3PayloadParsingEventDecoder");
};

typedef boost::shared_ptr<I3PayloadParsingEventDecoder> I3PayloadParsingEventDecoderPtr;

#endif
