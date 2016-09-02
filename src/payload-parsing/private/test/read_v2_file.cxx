#include <I3Test.h>

#include <algorithm>
#include <fstream>
#ifndef __APPLE_CC__
#include <netinet/in.h>
#else
#include <sys/types.h>
#include <machine/endian.h>
#endif
#include <icetray/serialization.h>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/tuple/tuple.hpp>

#include <icetray/I3Frame.h>
#include <payload-parsing/I3PayloadParsingEventDecoder.h>
#include <payload-parsing/decode.h>
#include <dataclasses/physics/I3EventHeader.h>
#include <dataclasses/physics/I3DOMLaunch.h>
#include <phys-services/I3FileOMKey2MBID.h>

TEST_GROUP(read_a_file);

namespace read_a_file_test
{
  struct extract_domlaunch_into_vector
  {
    extract_domlaunch_into_vector(std::vector<std::pair<bool,I3DOMLaunch> >& launchesIn) 
      : launches(launchesIn){}

    void operator()(const std::pair<OMKey,I3DOMLaunchSeries>& series)
    {
      for(I3DOMLaunchSeries::const_iterator iter = series.second.begin() ; 
          iter != series.second.end() ; 
          iter++)
      {
        // split up icetop / inice
        if(series.first.GetOM() > 60)
          launches.push_back(std::pair<bool,I3DOMLaunch>(false,*iter));
        else
          launches.push_back(std::pair<bool,I3DOMLaunch>(true,*iter));
      }
    }

    std::vector<std::pair<bool,I3DOMLaunch> >& launches;
  };

  struct ensure_trigger
  {
    ensure_trigger(I3DOMLaunch::TriggerType trigger_in) : trigger(trigger_in){}

    void operator()(const I3DOMLaunch& launch)
    {
      ENSURE(launch.GetTriggerType() == trigger);
    }

    void operator()(const std::pair<OMKey,I3DOMLaunchSeries>& series)
    {
      for_each(series.second.begin(),
          series.second.end(),
          ensure_trigger(trigger));
    }

    I3DOMLaunch::TriggerType trigger;
  };

  boost::tuple<unsigned,unsigned,unsigned,unsigned> 
  check_v2_file(I3Frame& frame,unsigned eventNumber)
  {
    const I3EventHeader& header = frame.Get<I3EventHeader>();
    ENSURE(eventNumber == header.GetEventID(),"Event Number Correct");
    ENSURE(header.GetRunID() == 87531);
    ENSURE(header.GetStartTime() != header.GetEndTime(),
        "Start does not equal endTime");

    std::cout<<header.GetStartTime().GetUTCYear()<<std::endl;
    std::cout<<header.GetStartTime().GetUTCDaqTime()<<std::endl;
    std::cout<<header.GetEndTime().GetUTCDaqTime()<<std::endl;

    // should have _both_ of these
    const I3DOMLaunchSeriesMap& inIceLaunches = 
      frame.Get<I3DOMLaunchSeriesMap>("InIceRawData");
    const I3DOMLaunchSeriesMap& iceTopLaunches = 
      frame.Get<I3DOMLaunchSeriesMap>("IceTopRawData");
    const I3DOMLaunchSeriesMap& inIceBeacons = 
      frame.Get<I3DOMLaunchSeriesMap>("InIceBeacon");
    const I3DOMLaunchSeriesMap& iceTopBeacons = 
      frame.Get<I3DOMLaunchSeriesMap>("IceTopBeacon");

    if(eventNumber == 1)
    {

      ENSURE(header.GetStartTime().GetUTCDaqTime() == 107380171314960773LL);
      ENSURE(header.GetStartTime().GetUTCYear() == 2006);
      ENSURE(header.GetEndTime().GetUTCDaqTime() == 107380171315162243LL);
      ENSURE(header.GetEndTime().GetUTCYear() == 2006);

      // checking that we get exactly the DOMs we expect
      ENSURE(inIceLaunches.size() == 15);
      ENSURE(iceTopLaunches.size() == 0);
      ENSURE(inIceLaunches.find(OMKey(30,6)) != inIceLaunches.end());
      ENSURE(inIceLaunches.find(OMKey(30,7)) != inIceLaunches.end());
      ENSURE(inIceLaunches.find(OMKey(30,8)) != inIceLaunches.end());
      ENSURE(inIceLaunches.find(OMKey(30,12)) != inIceLaunches.end());
      ENSURE(inIceLaunches.find(OMKey(30,14)) != inIceLaunches.end());
      ENSURE(inIceLaunches.find(OMKey(30,15)) != inIceLaunches.end());
      ENSURE(inIceLaunches.find(OMKey(30,16)) != inIceLaunches.end());
      ENSURE(inIceLaunches.find(OMKey(40,19)) != inIceLaunches.end());
      ENSURE(inIceLaunches.find(OMKey(40,22)) != inIceLaunches.end());
      ENSURE(inIceLaunches.find(OMKey(40,23)) != inIceLaunches.end());
      ENSURE(inIceLaunches.find(OMKey(50,26)) != inIceLaunches.end());
      ENSURE(inIceLaunches.find(OMKey(50,30)) != inIceLaunches.end());
      ENSURE(inIceLaunches.find(OMKey(50,38)) != inIceLaunches.end());
      ENSURE(inIceLaunches.find(OMKey(50,39)) != inIceLaunches.end());
      ENSURE(inIceLaunches.find(OMKey(59,58)) != inIceLaunches.end());

      ENSURE(inIceLaunches.find(OMKey(30,6))->second.size() == 1);
      const I3DOMLaunch& onelaunch = 
        inIceLaunches.find(OMKey(30,6))->second[0];
      ENSURE_DISTANCE(onelaunch.GetStartTime(),8067.9,0.0001)

        // checking that they are all non-beacon hits (in this event)
        for(I3DOMLaunchSeriesMap::const_iterator iter = inIceLaunches.begin();
            iter != inIceLaunches.end() ;
            iter++)
        {
          const I3DOMLaunchSeries& series = iter->second;
          for(I3DOMLaunchSeries::const_iterator launch = series.begin(); 
              launch != series.end(); 
              launch++)
          {
            ENSURE(launch->GetTriggerType() == 
                I3DOMLaunch::SPE_DISCRIMINATOR_TRIGGER);
          }
        }

      const std::vector<int>& checkedatwd = onelaunch.GetRawATWD(0);

      const int atwd[] = { 134, 141, 141, 137, 
        134, 140, 141, 135,
        136, 175, 183, 175,
        156, 153, 145};

      for(unsigned i = 0 ; i < 15 ; i++)
      {
        std::cout<<checkedatwd[i]<<" "<<atwd[i]<<std::endl;
      }

      for(unsigned i = 0 ; i < 15 ; i++)
      {
        ENSURE(checkedatwd[i] == atwd[i]);
      }
    }

    if(eventNumber == 17)
    {
      // checking that we get more than one DOM launch when we expect it
      ENSURE(inIceLaunches.find(OMKey(29,10)) != inIceLaunches.end());
      ENSURE(inIceLaunches.find(OMKey(29,10))->second.size() == 2);

      ENSURE(inIceLaunches.find(OMKey(29,11)) != inIceLaunches.end());
      ENSURE(inIceLaunches.find(OMKey(29,11))->second.size() == 2);

      ENSURE(inIceLaunches.find(OMKey(38,21)) != inIceLaunches.end());
      ENSURE(inIceLaunches.find(OMKey(38,21))->second.size() == 2);
    }

    // checking that we found some beacons ok
    if(eventNumber == 19)
    {
      ENSURE(inIceBeacons.find(OMKey(38,26)) != inIceBeacons.end());
      ENSURE(inIceBeacons.find(OMKey(38,26))->second.size() == 1);
      ENSURE(inIceBeacons.find(OMKey(38,26))->second[0].GetTriggerType() 
          == I3DOMLaunch::CPU_REQUESTED);
    }
    if(eventNumber == 22)
    {
      ENSURE(inIceBeacons.find(OMKey(38,10)) != inIceBeacons.end());
      ENSURE(inIceBeacons.find(OMKey(38,10))->second.size() == 1);
      ENSURE(inIceBeacons.find(OMKey(38,10))->second[0].GetTriggerType() 
          == I3DOMLaunch::CPU_REQUESTED);
    }
    if(eventNumber == 83)
    {
      ENSURE(inIceBeacons.find(OMKey(29,24)) != inIceBeacons.end());
      ENSURE(inIceBeacons.find(OMKey(29,24))->second.size() == 1);
      ENSURE(inIceBeacons.find(OMKey(29,24))->second[0].GetTriggerType() 
          == I3DOMLaunch::CPU_REQUESTED);
    }

    if(iceTopLaunches.size() > 0)
      std::cout<<"LOOK: iceTopEvent:"<<eventNumber<<std::endl;

    // an IceTop event
    if(eventNumber == 23)
    {
      std::cout<<"LOOK: "<<inIceLaunches.size()<<std::endl;
      ENSURE(inIceLaunches.size() == 0);
      ENSURE(iceTopLaunches.size() == 6);
      ENSURE(iceTopLaunches.find(OMKey(39,63)) != iceTopLaunches.end());
      ENSURE(iceTopLaunches.find(OMKey(40,61)) != iceTopLaunches.end());
      ENSURE(iceTopLaunches.find(OMKey(40,63)) != iceTopLaunches.end());
      ENSURE(iceTopLaunches.find(OMKey(49,61)) != iceTopLaunches.end());
      ENSURE(iceTopLaunches.find(OMKey(49,63)) != iceTopLaunches.end());

    }

    bool foundFirstLaunch = false;

    for_each(inIceBeacons.begin() , 
        inIceBeacons.end() , 
        ensure_trigger(I3DOMLaunch::CPU_REQUESTED));
    for_each(iceTopBeacons.begin() , 
        iceTopBeacons.end() , 
        ensure_trigger(I3DOMLaunch::CPU_REQUESTED));
    for_each(inIceLaunches.begin() , 
        inIceLaunches.end() , 
        ensure_trigger(I3DOMLaunch::SPE_DISCRIMINATOR_TRIGGER));
    for_each(iceTopLaunches.begin() , 
        iceTopLaunches.end() , 
        ensure_trigger(I3DOMLaunch::SPE_DISCRIMINATOR_TRIGGER));

    std::vector<std::pair<bool,I3DOMLaunch> > allLaunches;
    for_each(inIceLaunches.begin(),
        inIceLaunches.end(),
        extract_domlaunch_into_vector(allLaunches));
    for_each(iceTopLaunches.begin(),
        iceTopLaunches.end(),
        extract_domlaunch_into_vector(allLaunches));
    for_each(inIceBeacons.begin(),
        inIceBeacons.end(),
        extract_domlaunch_into_vector(allLaunches));
    for_each(iceTopBeacons.begin(),
        iceTopBeacons.end(),
        extract_domlaunch_into_vector(allLaunches));

    for(std::vector<std::pair<bool,I3DOMLaunch> >::const_iterator iter = 
        allLaunches.begin() ; 
        iter != allLaunches.end() ; 
        iter++)
    {
      const I3DOMLaunch& launch = iter->second;
      if(fabs(launch.GetStartTime() - 8000.0) < 0.01)
        foundFirstLaunch = true;

      // should be one of these two
      ENSURE(launch.GetTriggerType() == 
          I3DOMLaunch::SPE_DISCRIMINATOR_TRIGGER || 
          launch.GetTriggerType() == 
          I3DOMLaunch::CPU_REQUESTED);

      if(iter->first)
      {
        ENSURE(launch.GetRawATWD(0).size() == 128);
        ENSURE(launch.GetRawATWD(1).size() == 32);
        ENSURE(launch.GetRawATWD(2).size() == 32);
        ENSURE(launch.GetRawFADC().size() == 50);
      }
      else
      {
        ENSURE(launch.GetRawATWD(0).size() == 128);
        ENSURE(launch.GetRawATWD(1).size() == 128);
        ENSURE(launch.GetRawATWD(2).size() == 128);
        ENSURE(launch.GetRawFADC().size() == 0);
      }

    }

    // checking that at least one DOM has time near 8000, the one that
    // leads in the trigger.
    ENSURE(foundFirstLaunch); 
    return 
      boost::tuple<unsigned,unsigned,unsigned,unsigned>(inIceLaunches.size(),
          inIceBeacons.size(),
          iceTopLaunches.size(),
          iceTopBeacons.size());
  }
};

using namespace read_a_file_test;
using namespace payload_parsing;

TEST(endian_swapping)
{
  std::cout<<"Hi, Peter"<<std::endl;
  unsigned i = 0x12345678;
  unsigned out = endian_swap(i);
  ENSURE(out == 0x78563412);
}

TEST(craps_out_with_not_type_13)
{
  std::vector<char> buffer;

  buffer.resize(50);

  unsigned payload_type = 34; // don't know how to decode these
  payload_type = endian_swap(payload_type);

  unsigned length = 50;
  length = endian_swap(length);

  *((unsigned*)(&buffer[0])) = payload_type;

  *((unsigned*)(&buffer[4])) = length;


  std::list<std::pair<uint64_t,I3DOMLaunch> > launches;
  I3TriggerHierarchy triggers;
  I3EventHeader header;

  payload_parsing::DecodeConfiguration config(0,2006,true, true);
  payload_parsing::DecodeTarget<13>::type target(launches,triggers,header);

  try
  {
    payload_parsing::decode_payload<13>(target,
        config,
        buffer,
        0);
    ENSURE(0,"That was supposed to throw");
  }
  catch(std::exception& e)
  {
  }
}

TEST(v2file)
{
  ENSURE(getenv("I3_TESTDATA"));
  std::string i3testdata = getenv("I3_TESTDATA");
  std::string eventFile = i3testdata + "/2006data/physics_20060505_064628-0-trimmed.ddd";

  std::ifstream ifs(eventFile.c_str());

  uint32_t eventSize;
  std::vector<char> eventData;
  unsigned int eventCount = 1;

  const char* i3work = getenv("I3_SRC");
  if(!i3work)
  {
    std::cout<<"Gotta have I3_SRC set"<<std::endl;
    exit(1);
  }

  std::string domfile = std::string(i3work) + "/phys-services/resources/doms.txt";
  I3FileOMKey2MBID omkey2mbid(domfile);

  I3PayloadParsingEventDecoder decode_13(omkey2mbid,
      2006,
      "I3EventHeader",
      "I3TriggerHierarchy",
      std::vector<OMKey>(),
      "Special",
      "InIce",
      "IceTop",
      "RawData",
      "Flasher",
      "Beacon",
      "LED",
      "Test",
      "Min_Bias");

  if(ifs.eof())
    {
      std::cout<<"Empty file"<<std::endl;
    }

  boost::tuple<unsigned,unsigned,unsigned,unsigned> hitCounts;

  std::set<unsigned> checkedEvents;

  while(!ifs.eof())
  {
    ifs.read((char *) &eventSize, 4);
    if(!ifs.eof())
    {
      if(!ifs.good())
        printf("cannot read event size from file %s\n",
            eventFile.c_str());

      eventSize = ntohl(eventSize);
      if(!eventSize)
        printf("event size is 0!?!\n");

      eventData.resize(eventSize);
      ifs.seekg(-4, std::ios::cur);
      ifs.read(&(eventData[0]), eventData.size());
      if(ifs.eof() || !ifs.good())
        printf("cannot read event from file %s\n", eventFile.c_str());

      I3Frame frame(I3Frame::DAQ);

      decode_13.FillEvent(frame,eventData);

      log_trace("Dumping contents of frame #: %d ", eventCount);

      const I3DOMLaunchSeriesMap& InIcelaunches =
        frame.Get<I3DOMLaunchSeriesMap>("InIceRawData");
      const I3DOMLaunchSeriesMap& IceToplaunches =
        frame.Get<I3DOMLaunchSeriesMap>("IceTopRawData");
      const I3EventHeader& header = frame.Get<I3EventHeader>();

      checkedEvents.insert(header.GetEventID());

      std::cout<<"EventID: " <<header.GetEventID()<<std::endl;
      std::cout<<"RunID: " <<header.GetRunID()<<std::endl;
      std::cout<<"N inice hit DOM's:  "<<InIcelaunches.size()<<std::endl;
      std::cout<<"N icetop hit DOM's: " << IceToplaunches.size() << std::endl;
      std::cout<<frame<<std::endl;
      std::cout<<"--------------------------------------------------"<<std::endl;

      boost::tuple<unsigned,unsigned,unsigned,unsigned> out = 
        check_v2_file(frame,eventCount);

      hitCounts = 
        boost::tuples::make_tuple(out.get<0>() + hitCounts.get<0>(),
            out.get<1>() + hitCounts.get<1>(),
            out.get<2>() + hitCounts.get<2>(),
            out.get<3>() + hitCounts.get<3>());

      eventCount += 1;
    }

  }

  // checking hits in each category
  std::cout<<hitCounts.get<0>()<<std::endl;
  std::cout<<hitCounts.get<1>()<<std::endl;
  std::cout<<hitCounts.get<2>()<<std::endl;
  std::cout<<hitCounts.get<3>()<<std::endl;

  ENSURE(hitCounts.get<0>() == 1168);
  ENSURE(hitCounts.get<1>() == 3);
  ENSURE(hitCounts.get<2>() == 28);
  ENSURE(hitCounts.get<3>() == 0);

  // checking that we in fact saw all the important events
  ENSURE(checkedEvents.find(1) != checkedEvents.end());
  ENSURE(checkedEvents.find(17) != checkedEvents.end());
  ENSURE(checkedEvents.find(19) != checkedEvents.end());
  ENSURE(checkedEvents.find(22) != checkedEvents.end());
  ENSURE(checkedEvents.find(23) != checkedEvents.end());
  ENSURE(checkedEvents.find(83) != checkedEvents.end());

  ENSURE(checkedEvents.size() == 99);
}
