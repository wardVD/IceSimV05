#include <I3Test.h>
#include <icetray/serialization.h>

#include <algorithm>
#include <fstream>
#ifndef __APPLE_CC__
#include <netinet/in.h>
#else
#include <sys/types.h>
#include <machine/endian.h>
#endif
#include <boost/archive/xml_oarchive.hpp>
#include <boost/tuple/tuple.hpp>

#include <icetray/I3Frame.h>
#include <payload-parsing/decode.h>
#include <payload-parsing/I3PayloadParsingEventDecoder.h>
#include <dataclasses/physics/I3EventHeader.h>
#include <dataclasses/physics/I3DOMLaunch.h>
#include <phys-services/I3FileOMKey2MBID.h>
#include <payload-parsing/utility.h>
#include <fstream>

#include <glob.h>
#include <sys/stat.h>

TEST_GROUP(deltaTest)


  using namespace std;


  struct printtrigger
{
  void operator()(const I3Trigger& trigger)
  {
    TriggerKey key = trigger.GetTriggerKey();
  }
};

struct dumpwaveform
{
  dumpwaveform(ostream& o) : ofs_(o),bin(0){}

  void operator()(int i)
  {
    ofs_<<i<<"\n";
    bin++;
  }

  private:
  ostream& ofs_;
  unsigned bin;
};

void dump(const std::vector<int>& in,string tag,unsigned num)
{
  ostringstream filename;
  filename << tag <<"_"<<num<<".dat";
  ofstream fout(filename.str().c_str());
  dumpwaveform d(fout);
  for_each(in.begin(),
      in.end(),
      d);
}

struct printlaunches
{
  printlaunches(map<uint64_t,unsigned>& count) : mbidCount(count){}
  map<uint64_t,unsigned>& mbidCount;

  void operator()(const pair<uint64_t,I3DOMLaunch>& launch)

  {
    mkdir("WAVEFORMDUMPFORDELTACOMPRESSIONTESTFOLDER",0777);


    ostringstream fadcName;
    fadcName<<"WAVEFORMDUMPFORDELTACOMPRESSIONTESTFOLDER/fadc_"<<hex<<launch.first;
    if(launch.second.GetRawFADC().size() > 0)
      dump(launch.second.GetRawFADC(),fadcName.str(),mbidCount[launch.first]);

    ostringstream atwd0Name;
    atwd0Name<<"WAVEFORMDUMPFORDELTACOMPRESSIONTESTFOLDER/atwd_chan0_"<<hex<<launch.first;
    if(launch.second.GetRawATWD(0).size() > 0)
      dump(launch.second.GetRawATWD(0),atwd0Name.str(),mbidCount[launch.first]);

    ostringstream atwd1Name;
    atwd1Name<<"WAVEFORMDUMPFORDELTACOMPRESSIONTESTFOLDER/atwd_chan1_"<<hex<<launch.first;
    if(launch.second.GetRawATWD(1).size() > 0)
      dump(launch.second.GetRawATWD(1),atwd1Name.str(),mbidCount[launch.first]);

    ostringstream atwd2Name;
    atwd2Name<<"WAVEFORMDUMPFORDELTACOMPRESSIONTESTFOLDER/atwd_chan2_"<<hex<<launch.first;
    if(launch.second.GetRawATWD(2).size() > 0)
      dump(launch.second.GetRawATWD(2),atwd2Name.str(),mbidCount[launch.first]);



    mbidCount[launch.first] += 1;
  }
};

struct printV5launches
{
  printV5launches(map<OMKey,unsigned>& count) : OMCount(count){}
  map<OMKey,unsigned>& OMCount;

  void operator()(const pair<OMKey,I3DOMLaunch>& launch)

  {
    mkdir("WAVEFORMDUMPFORDELTACOMPRESSIONTESTFOLDER",0777);


    ostringstream fadcName;
    fadcName<<"WAVEFORMDUMPFORDELTACOMPRESSIONTESTFOLDER/fadc_"<<hex<<launch.first;
    if(launch.second.GetRawFADC().size() > 0)
      dump(launch.second.GetRawFADC(),fadcName.str(),OMCount[launch.first]);

    ostringstream atwd0Name;
    atwd0Name<<"WAVEFORMDUMPFORDELTACOMPRESSIONTESTFOLDER/atwd_chan0_"<<hex<<launch.first;
    if(launch.second.GetRawATWD(0).size() > 0)
      dump(launch.second.GetRawATWD(0),atwd0Name.str(),OMCount[launch.first]);

    ostringstream atwd1Name;
    atwd1Name<<"WAVEFORMDUMPFORDELTACOMPRESSIONTESTFOLDER/atwd_chan1_"<<hex<<launch.first;
    if(launch.second.GetRawATWD(1).size() > 0)
      dump(launch.second.GetRawATWD(1),atwd1Name.str(),OMCount[launch.first]);

    ostringstream atwd2Name;
    atwd2Name<<"WAVEFORMDUMPFORDELTACOMPRESSIONTESTFOLDER/atwd_chan2_"<<hex<<launch.first;
    if(launch.second.GetRawATWD(2).size() > 0)
      dump(launch.second.GetRawATWD(2),atwd2Name.str(),OMCount[launch.first]);



    OMCount[launch.first] += 1;
  }
};



using namespace payload_parsing;

void appendInputFiles(string directory,vector<string>& toAppend)
{
  glob_t g;
  if(directory[directory.size() - 1 ] == '/')
    glob(string(directory + "*").c_str(),0,0,&g);
  else
    glob(string(directory + "/*").c_str(),0,0,&g);

  for(unsigned i = 0 ; i < g.gl_pathc ; i++)
  {
    toAppend.push_back(g.gl_pathv[i]);
  }

  globfree(&g);
}

int clearDirectory(const string& directory)
{
  vector<string> files;
  appendInputFiles(directory,files);
  for(vector<string>::iterator iter = files.begin() ;
      iter != files.end() ;
      iter++)
  {
    unlink(iter->c_str());
  }
  return remove(directory.c_str());
}

bool fileExists(const string& file)
{
  if(access(file.c_str(),F_OK) == 0)
    return true;
  return false;
}

// end glob stuff

void DeltaDecode(int argc, string eventFile)
{
  if(argc != 2)
  {
    cout<<"One arg, the ddd file to parse"<<endl;
    exit(1);
  }
  ifstream ifs(eventFile.c_str());

  uint32_t eventSize;
  vector<char> eventData;
  unsigned int eventCount = 1;

  if(ifs.eof())
  {
    cout<<"Empty file"<<endl;
  }

  int nEvents = 0;

  map<uint64_t,unsigned> mbidCounts;


  while(!ifs.eof() && nEvents < 100)
  {
    nEvents += 1;
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
      ifs.seekg(-4, ios::cur);
      ifs.read(&(eventData[0]), eventData.size());
      if(ifs.eof() || !ifs.good())
        printf("cannot read event from file %s\n", eventFile.c_str());

      uint32_t payloadType = decode<uint32_t>(BIG,4,eventData);
      uint64_t globalTime = decode<uint64_t>(BIG,8,eventData);

      if(payloadType != 13)
      {
        log_fatal("expected type 13 and got %d",payloadType);
      }

      DecodeConfiguration config(globalTime,2007,true,true);

      // setup target data structures
      DOMLaunches launches;
      I3TriggerHierarchyPtr triggers(new I3TriggerHierarchy);
      I3EventHeaderPtr header(new I3EventHeader());

      DecodeTarget<13>::type target(launches,*triggers,*header);

      // do the decoding ...
      decode_payload<13>(target,
          config,
          eventData,
          0);

      for_each(triggers->begin(),
          triggers->end(),
          printtrigger());

      for_each(launches.begin(),
          launches.end(),
          printlaunches(mbidCounts));

      eventCount += 1;
    }

  }

}


void V5Decode(int argc, string eventFile)
{
  if(argc != 2)
  {
    cout<<"One arg, the ddd file to parse"<<endl;
    exit(1);
  }
  ifstream ifs(eventFile.c_str());

  uint32_t eventSize;
  vector<char> eventData;
  unsigned int eventCount = 1;

  if(ifs.eof())
  {
    cout<<"Empty file"<<endl;
  }

  int nEvents = 0;

  map<uint64_t,unsigned> mbidCounts;
  map<OMKey,unsigned> OMCounts;

  while(!ifs.eof() && nEvents < 10000)
  {
    nEvents += 1;
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
      ifs.seekg(-4, ios::cur);
      ifs.read(&(eventData[0]), eventData.size());
      if(ifs.eof() || !ifs.good())
        printf("cannot read event from file %s\n", eventFile.c_str());

      uint32_t payloadType = decode<uint32_t>(BIG,4,eventData);
      uint64_t globalTime = decode<uint64_t>(BIG,8,eventData);

      if(payloadType != 21)
      {
        log_fatal("expected type 21 and got %d",payloadType);
      }

      DecodeConfiguration config(globalTime,0,true,true);

      // setup target data structures
      NewDOMLaunches launches;
      I3TriggerHierarchyPtr triggers(new I3TriggerHierarchy);
      I3EventHeaderPtr header(new I3EventHeader());

      DecodeTarget<21>::type target(launches,*triggers,*header);

      // do the decoding ...
      decode_payload<21>(target,
          config,
          eventData,
          0);

      for_each(triggers->begin(),
          triggers->end(),
          printtrigger());

      for_each(launches.begin(),
          launches.end(),
          printV5launches(OMCounts));

      eventCount += 1;
    }

  }

}


TEST(Delta)
{
  ENSURE(getenv("I3_TESTDATA"));
  string i3testdata = getenv("I3_TESTDATA");

  string eventFile = i3testdata + "/DeltaWaveforms/input/physics_912_0_0_914.dat";

  vector<char> eventData;

  DeltaDecode(2, eventFile);

  vector<string> waveform_data;
  vector<string> truewaveform_data;

  string waveforms = "./WAVEFORMDUMPFORDELTACOMPRESSIONTESTFOLDER";
  string truewaveforms = i3testdata + "/DeltaWaveforms/true";

  appendInputFiles(waveforms, waveform_data);

  long long int value = 0;

  for(vector<string>::const_iterator iter = waveform_data.begin() ; 
      iter != waveform_data.end() ; 
      iter++)
  {  
    int j;
    ifstream file;
    string in;
    in = *iter;

    file.open(in.c_str());
    while (file >> j) {
      value += j;
    }
  }


  appendInputFiles(truewaveforms, truewaveform_data);

  long long value1 = 0;

  for(vector<string>::const_iterator iter = truewaveform_data.begin() ; 
      iter != truewaveform_data.end() ; 
      iter++)
  {  
    int j;

    ifstream file;
    string in;
    in = *iter;

    file.open(in.c_str());
    while (file >> j) {
      value1 += j;
    }
  }

  clearDirectory(waveforms);
  ENSURE(value == value1);

}

TEST(Version5Waveforms)
{
  ENSURE(getenv("I3_TESTDATA"));
  string i3testdata = getenv("I3_TESTDATA");

  string eventFile = i3testdata + "/V5Waveforms/physics_22964_0_0_2053.dat";

  vector<char> eventData;

  V5Decode(2, eventFile);

  vector<string> waveform_data;
  vector<string> truewaveform_data;

  string waveforms = "./WAVEFORMDUMPFORDELTACOMPRESSIONTESTFOLDER";
  string truewaveforms = i3testdata + "/V5Waveforms/true";
  appendInputFiles(waveforms, waveform_data);

  long long int value = 0;

  for(vector<string>::const_iterator iter = waveform_data.begin() ; 
      iter != waveform_data.end() ; 
      iter++)
  {  
    int j;

    ifstream file;
    string in;
    in = *iter;

    file.open(in.c_str());
    while (file >> j) {
      value += j;
    }
  }


  appendInputFiles(truewaveforms, truewaveform_data);

  long long value1 = 0;

  for(vector<string>::const_iterator iter = truewaveform_data.begin() ; 
      iter != truewaveform_data.end() ; 
      iter++)
  {  
    int j;

    ifstream file;
    string in;
    in = *iter;

    file.open(in.c_str());
    while (file >> j) {
      value1 += j;
    }
  }

  clearDirectory(waveforms);
  cout << waveform_data.size() << endl;
  cout << truewaveform_data.size() << endl;

  ENSURE(value == value1);
}
