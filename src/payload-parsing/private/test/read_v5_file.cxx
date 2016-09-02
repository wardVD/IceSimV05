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
//#include <phys-services/I3FileOMKey2MBID.h>
#include <phys-services/I3XMLOMKey2MBID.h>

TEST_GROUP(read_v5_file);

namespace read_v5_file_test
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
  check_v5_file(I3Frame& frame,unsigned eventNumber)
  {
    const I3EventHeader& header = frame.Get<I3EventHeader>();
    ENSURE(eventNumber == header.GetEventID(),"Event Number Correct");
    ENSURE(header.GetRunID() == 147349);
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
    const I3DOMLaunchSeriesMap& inIceMinBias = 
      frame.Get<I3DOMLaunchSeriesMap>("InIceMin_Bias");
    const I3DOMLaunchSeriesMap& iceTopMinBias = 
      frame.Get<I3DOMLaunchSeriesMap>("IceTopMin_Bias");

    const bool isMinBias (inIceMinBias.size () > 0 or iceTopMinBias.size () > 0);
    if (isMinBias)
      std::cout << "MINBIAS EVENT" << std::endl;

    if(eventNumber == 2086)
    {

      ENSURE(header.GetStartTime().GetUTCDaqTime() == 82773118448891482LL);
      ENSURE(header.GetStartTime().GetUTCYear() == 2016);
      ENSURE(header.GetEndTime().GetUTCDaqTime() == 82773118449287158LL);
      ENSURE(header.GetEndTime().GetUTCYear() == 2016);

      // checking that all non-beacon hits are ok (in this event)
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
    }

    if(eventNumber == 2097)
    {

      ENSURE(header.GetStartTime().GetUTCDaqTime() == 82773118475927498LL);
      ENSURE(header.GetStartTime().GetUTCYear() == 2016);
      ENSURE(header.GetEndTime().GetUTCDaqTime() == 82773118476267343LL);
      ENSURE(header.GetEndTime().GetUTCYear() == 2016);

      // checking that all beacon hits are ok (in this event)
      for(I3DOMLaunchSeriesMap::const_iterator iter = inIceBeacons.begin();
          iter != inIceBeacons.end() ;
          iter++)
      {
        const I3DOMLaunchSeries& series = iter->second;
        for(I3DOMLaunchSeries::const_iterator launch = series.begin(); 
            launch != series.end(); 
            launch++)
        {
          ENSURE(launch->GetTriggerType() == I3DOMLaunch::CPU_REQUESTED);
        }
      }
    }

    if(eventNumber == 2111)
    {

      ENSURE(header.GetStartTime().GetUTCDaqTime() == 82773118536601603LL);
      ENSURE(header.GetStartTime().GetUTCYear() == 2016);
      ENSURE(header.GetEndTime().GetUTCDaqTime() == 82773118536815208LL);
      ENSURE(header.GetEndTime().GetUTCYear() == 2016);
    }

    if(eventNumber == 2137)
    {
      // Lot's of IceTop Hits
      ENSURE(iceTopLaunches.size() == 10);

      ENSURE(header.GetStartTime().GetUTCDaqTime() == 82773118613028496LL);
      ENSURE(header.GetStartTime().GetUTCYear() == 2016);
      ENSURE(header.GetEndTime().GetUTCDaqTime() == 82773118613408186LL);
      ENSURE(header.GetEndTime().GetUTCYear() == 2016);
    }

    bool foundFirstLaunch = false;

    for_each(inIceBeacons.begin() , 
        inIceBeacons.end() , 
        ensure_trigger(I3DOMLaunch::CPU_REQUESTED));
    for_each(iceTopBeacons.begin() , 
        iceTopBeacons.end() , 
        ensure_trigger(I3DOMLaunch::CPU_REQUESTED));

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
    for_each(inIceMinBias.begin(),
        inIceMinBias.end(),
        extract_domlaunch_into_vector(allLaunches));
    for_each(iceTopMinBias.begin(),
        iceTopMinBias.end(),
        extract_domlaunch_into_vector(allLaunches));

    for(std::vector<std::pair<bool,I3DOMLaunch> >::const_iterator iter = 
        allLaunches.begin() ; 
        iter != allLaunches.end() ; 
        iter++)
    {
      const I3DOMLaunch& launch = iter->second;
      if(fabs(launch.GetStartTime() - 10000.0) < 0.01)
        foundFirstLaunch = true;
      else if (isMinBias and fabs(launch.GetStartTime() - 1000.0) < 0.01)
        foundFirstLaunch = true;

      //std::cout << launch.GetStartTime() << std::endl;

      // should be one of these three
      ENSURE(
          launch.GetTriggerType() == 
          I3DOMLaunch::SPE_DISCRIMINATOR_TRIGGER || 
          launch.GetTriggerType() == 
          I3DOMLaunch::MPE_DISCRIMINATOR_TRIGGER || 
          launch.GetTriggerType() == 
          I3DOMLaunch::CPU_REQUESTED);

    }

    // checking that at least one DOM has time near 10000, the one that
    // leads in the trigger. OR one near 1000 if a min-bias trigger.
    ENSURE(foundFirstLaunch); 

    return 
      boost::tuple<unsigned,unsigned,unsigned,unsigned>(
          inIceLaunches.size(),
          inIceBeacons.size(),
          iceTopLaunches.size(),
          iceTopBeacons.size());
  }
};

using namespace read_v5_file_test;
using namespace payload_parsing;

TEST(fails_when_not_type_21)
{
  std::vector<char> buffer;

  buffer.resize(50);

  unsigned payload_type = 34; // don't know how to decode these
  payload_type = endian_swap(payload_type);

  unsigned length = 50;
  length = endian_swap(length);

  *((unsigned*)(&buffer[0])) = payload_type;

  *((unsigned*)(&buffer[4])) = length;


  NewDOMLaunches launches;
  I3TriggerHierarchy triggers;
  I3EventHeader header;

  payload_parsing::DecodeConfiguration config(0,2016,true, true);
  payload_parsing::DecodeTarget<21>::type target(launches,triggers,header);

  try
  {
    payload_parsing::decode_payload<21>(target,
        config,
        buffer,
        0);
    ENSURE(0,"That was supposed to throw");
  }
  catch(std::exception& e)
  {
  }
}


TEST(v5file)
{
  ENSURE(getenv("I3_TESTDATA"));
  std::string i3testdata = getenv("I3_TESTDATA");
  std::string eventFile = i3testdata + "/payload_testdata/physics-v5.dat";

  std::ifstream ifs(eventFile.c_str());

  uint32_t eventSize;
  std::vector<char> eventData;
  unsigned int eventCount = 2086; // starting event count

  const char* i3work = getenv("I3_SRC");
  if(!i3work)
  {
    std::cout<<"Gotta have I3_SRC set"<<std::endl;
    exit(1);
  }

  std::string xmldomfile = std::string (i3work) + "/phys-services/resources/mainboard_ids.xml.gz";
  I3XMLOMKey2MBID xmlomkey2mbid (xmldomfile);

  I3PayloadParsingEventDecoder decode_21 (xmlomkey2mbid,
      0,
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

      decode_21.FillEvent(frame, eventData);

      log_trace("Dumping contents of frame #: %d ", eventCount);

      const I3DOMLaunchSeriesMap& InIcelaunches =
        frame.Get<I3DOMLaunchSeriesMap>("InIceRawData");
      const I3DOMLaunchSeriesMap& IceToplaunches =
        frame.Get<I3DOMLaunchSeriesMap>("IceTopRawData");
      const I3EventHeader& header = frame.Get<I3EventHeader>();

      std::cout<<"EventID: " <<header.GetEventID()<<std::endl;
      std::cout<<"RunID: " <<header.GetRunID()<<std::endl;
      std::cout<<"N inice hit DOM's:  "<<InIcelaunches.size()<<std::endl;
      std::cout<<"N icetop hit DOM's: " << IceToplaunches.size() << std::endl;
      std::cout<<frame<<std::endl;
      std::cout<<"--------------------------------------------------"<<std::endl;

      boost::tuple<unsigned,unsigned,unsigned,unsigned> out = 
        check_v5_file(frame, eventCount);

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

  ENSURE(hitCounts.get<0>() == 152995);
  ENSURE(hitCounts.get<1>() == 250);
  ENSURE(hitCounts.get<2>() == 11352);
  ENSURE(hitCounts.get<3>() == 67);


}
