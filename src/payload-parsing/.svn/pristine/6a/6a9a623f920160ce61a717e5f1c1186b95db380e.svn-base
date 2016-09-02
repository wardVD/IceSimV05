#include <payload-parsing/I3PayloadParsingEventDecoderFactory.h>
#include <payload-parsing/I3PayloadParsingEventDecoder.h>
#include <interfaces/I3OMKey2MBID.h>
#include <dataclasses/physics/I3EventHeader.h>
#include <dataclasses/physics/I3TriggerHierarchy.h>


I3_SERVICE_FACTORY(I3PayloadParsingEventDecoderFactory);


I3PayloadParsingEventDecoderFactory::I3PayloadParsingEventDecoderFactory(
    const I3Context& context)
  : I3ServiceFactory(context),
  year_(0),
  headerID_(I3DefaultName<I3EventHeader>::value()), 
  triggerID_(I3DefaultName<I3TriggerHierarchy>::value()),
  inIceID_("InIce"), 
  iceTopID_("IceTop"), 
  speDataID_("RawData")
{
  AddParameter("Year",
      "The year the data to decode is from.",
      year_);
  AddParameter("HeaderID",
      "Name of the I3EventHeader.",
      headerID_);
  AddParameter("TriggerID",
      "Name of the I3TriggerHierarchy.",
      triggerID_);               
  AddParameter("SpecialDataOMs",
      "List of optical modules whose DOM "
      "launches are assigned to the "
      "special data I3DOMLaunchSeriesMap.",
      specialDataOMs_);               
  AddParameter("SpecialDataID",
      "Name of the special data I3DOMLaunchSeriesMap.",
      specialDataID_);               
  AddParameter("InIceID",
      "Name affix for IceCube I3DOMLaunchSeriesMaps.",
      inIceID_);
  AddParameter("IceTopID",
      "Name affix for IceTop I3DOMLaunchSeriesMaps.",
      iceTopID_);
  AddParameter("SPEDataID",
      "Name of I3DOMLaunchSeriesMaps holding "
      "DOM launches with trigger mode "
      "SPE_DISCRIMINATOR_TRIGGER.",
      speDataID_);
  AddParameter("FlasherDataID",
      "Name of I3DOMLaunchSeriesMaps holding "
      "DOM launches with trigger mode "
      "FLASHER_BOARD_TRIGGER.",
      flasherDataID_);
  AddParameter("CPUDataID",
      "Name of I3DOMLaunchSeriesMaps holding DOM "
      "launches with trigger mode "
      "CPU_REQUESTED.",
      cpuDataID_);
  AddParameter("OnboardLEDID",
      "Name of the I3DOMLaunchSeriesMaps holding DOM"
      "launches with trigger type "
      "ONBOARD_LED",
      onboardLEDID_);
  AddParameter("TestDataID",
      "Name of I3DOMLaunchSeriesMaps holding DOM "
      "launches with trigger mode "
      "TEST_PATTERN.",
      testDataID_);
  AddParameter("MinBiasID",
      "Name of I3DOMLaunchSeriesMap holding MinBias "
      "Trigger mode",
      minBiasID_);
}


I3PayloadParsingEventDecoderFactory::~I3PayloadParsingEventDecoderFactory()
{
}


bool I3PayloadParsingEventDecoderFactory::InstallService(I3Context& services)
{	
  if(!decoder_){
    I3OMKey2MBID& omkey2mbid = context_.Get<I3OMKey2MBID>();

    decoder_ = 
      I3PayloadParsingEventDecoderPtr
      (new I3PayloadParsingEventDecoder(omkey2mbid,
                                        year_,
                                        headerID_,
                                        triggerID_,
                                        specialDataOMs_,
                                        specialDataID_,
                                        inIceID_,
                                        iceTopID_,
                                        speDataID_,
                                        flasherDataID_,
                                        cpuDataID_,
                                        onboardLEDID_,
                                        testDataID_,
                                        minBiasID_)
      );
  }
  return services.Put(decoder_);
}


void I3PayloadParsingEventDecoderFactory::Configure()
{
  GetParameter("Year", year_);
  GetParameter("HeaderID", headerID_);
  GetParameter("TriggerID", triggerID_);
  GetParameter("SpecialDataOMs", specialDataOMs_);
  GetParameter("SpecialDataID", specialDataID_);
  GetParameter("InIceID", inIceID_);
  GetParameter("IceTopID", iceTopID_);
  GetParameter("SPEDataID", speDataID_);
  GetParameter("FlasherDataID", flasherDataID_);
  GetParameter("CPUDataID", cpuDataID_);
  GetParameter("OnboardLEDID", onboardLEDID_);
  GetParameter("TestDataID", testDataID_);
  GetParameter("MinBiasID", minBiasID_);
}
