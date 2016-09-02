#ifndef I3PAYLOADPARSINGEVENTDECODERFACTORY_H
#define I3PAYLOADPARSINGEVENTDECODERFACTORY_H

class I3Context;
class OMKey;


#include <string>
#include <vector>

#include <icetray/I3Logging.h>
#include <icetray/I3ServiceFactory.h>
#include <daq-decode/I3DAQEventDecoder.h>


class I3PayloadParsingEventDecoderFactory : public I3ServiceFactory
{
  public:
    explicit I3PayloadParsingEventDecoderFactory(const I3Context& context);

    virtual ~I3PayloadParsingEventDecoderFactory();

    bool InstallService(I3Context& services);

    void Configure();

  private:
    I3DAQEventDecoderPtr decoder_;
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
    std::string minBiasID_;
};

#endif 
