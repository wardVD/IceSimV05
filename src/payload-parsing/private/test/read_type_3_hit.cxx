#include <I3Test.h>
#include <fstream>
#include <payload-parsing/decode.h>
#include <arpa/inet.h>

TEST_GROUP(read_type_3);

TEST(read_type_3_payloads){
  ENSURE(getenv("I3_TESTDATA"));
  std::string i3testdata = getenv("I3_TESTDATA");
  std::string hitsFile = i3testdata + "/payloads-unpacker/HitPayloads_type3";

  std::ifstream ifs(hitsFile.c_str());
  ENSURE(ifs.good());

  uint32_t eventSize, eventType=0;
  std::vector<char> eventData;
  I3Map<uint64_t,std::vector<I3DOMLaunch> > launches;

  while(ifs.peek() != EOF)
  {
    ifs.read((char *) &eventSize, 4);
    ifs.read((char *) &eventType, 4);
    eventSize = ntohl(eventSize);
    eventType = ntohl(eventType);
    if(!ifs.eof() && eventType==3)
    {
      if(!eventSize)
        log_fatal("event with size 0?");

      eventData.resize(eventSize);
      ifs.seekg(-8, std::ios::cur);
      ifs.read(&(eventData[0]), eventData.size());
      if(ifs.fail())
        log_fatal("Unexpected read failure");

      payload_parsing::DecodeTarget<3>::type target;
      payload_parsing::DecodeConfiguration config(0,0,true,true);
      payload_parsing::decode_payload<3>(target,config,eventData,0);

      launches[target.mbid].push_back(target.launch);
    }
    else
      break;
  }

  const uint64_t Monopoly = 0x60FBC4B4E69EULL;
  const uint64_t Rensa = 0xFEF2DCC41B6AULL;
  const uint64_t Trouble = 0x12B78D1CEA33ULL;
  //There should be one launch from each of three DOMs
  ENSURE(launches.size()==3);
  ENSURE(launches[Monopoly].size()==1);
  ENSURE(launches[Rensa].size()==1);
  ENSURE(launches[Trouble].size()==1);
  const I3DOMLaunch& slcLaunch = launches[Monopoly].front();
  const I3DOMLaunch& hlcLaunch = launches[Rensa].front();
  const I3DOMLaunch& beaconLaunch = launches[Trouble].front();

  ENSURE(slcLaunch.GetTriggerType()==I3DOMLaunch::SPE_DISCRIMINATOR_TRIGGER);
  ENSURE(slcLaunch.GetTriggerMode()==I3DOMLaunch::SLC_READOUT);
  ENSURE(slcLaunch.GetIsPedestalSub());
  ENSURE(!slcLaunch.GetLCBit());
  ENSURE(slcLaunch.GetRawATWDs().size()==4);
  ENSURE(slcLaunch.GetRawATWDs()[0].size()==0);
  ENSURE(slcLaunch.GetRawATWDs()[1].size()==0);
  ENSURE(slcLaunch.GetRawATWDs()[2].size()==0);
  ENSURE(slcLaunch.GetRawATWDs()[3].size()==0);
  ENSURE(slcLaunch.GetRawFADC().size()==0);

  ENSURE(hlcLaunch.GetTriggerType()==I3DOMLaunch::SPE_DISCRIMINATOR_TRIGGER);
  ENSURE(hlcLaunch.GetTriggerMode()==I3DOMLaunch::LC_UPPER);
  ENSURE(hlcLaunch.GetIsPedestalSub());
  ENSURE(hlcLaunch.GetLCBit());
  ENSURE(hlcLaunch.GetRawATWDs().size()==4);
  ENSURE(hlcLaunch.GetRawATWDs()[0].size()==128);
  ENSURE(hlcLaunch.GetRawATWDs()[1].size()==0);
  ENSURE(hlcLaunch.GetRawATWDs()[2].size()==0);
  ENSURE(hlcLaunch.GetRawATWDs()[3].size()==0);
  ENSURE(hlcLaunch.GetRawFADC().size()==256);

  ENSURE(beaconLaunch.GetTriggerType()==I3DOMLaunch::CPU_REQUESTED);
  ENSURE(beaconLaunch.GetTriggerMode()==I3DOMLaunch::UNDEFINED);
  ENSURE(beaconLaunch.GetIsPedestalSub());
  ENSURE(!beaconLaunch.GetLCBit());
  ENSURE(beaconLaunch.GetRawATWDs().size()==4);
  ENSURE(beaconLaunch.GetRawATWDs()[0].size()==128);
  ENSURE(beaconLaunch.GetRawATWDs()[1].size()==128);
  ENSURE(beaconLaunch.GetRawATWDs()[2].size()==128);
  ENSURE(beaconLaunch.GetRawATWDs()[3].size()==0);
  ENSURE(beaconLaunch.GetRawFADC().size()==256);

}
