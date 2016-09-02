#include <icetray/serialization.h>

#include <fstream>
#include <boost/archive/xml_oarchive.hpp>
#ifndef __APPLE_CC__
#include <netinet/in.h>
#else
#include <sys/types.h>
#include <machine/endian.h>
#endif

#include <payload-parsing/decode.h>
#include <dataclasses/physics/I3EventHeader.h>
#include <dataclasses/physics/I3DOMLaunch.h>
#include <dataclasses/physics/I3TriggerHierarchy.h>
#include <phys-services/I3FileOMKey2MBID.h>
#include <dataclasses/I3Vector.h>

#include <fstream>

using namespace std;


struct printtrigger
{
  void operator()(const I3Trigger& trigger)
  {
    TriggerKey key = trigger.GetTriggerKey();
    cout<<"  source: "<<TriggerKey::GetSourceString(key.GetSource())<<endl;
    cout<<"  type: "<<TriggerKey::GetTypeString(key.GetType())<<endl;
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

void dump(const I3Vector<int>& in,string tag,unsigned num)
{
  ostringstream filename;
  filename << tag <<"_"<<num<<".dat";
  ofstream fout(filename.str().c_str());
  dumpwaveform d(fout);
  for_each(in.begin(),
           in.end(),
           d);
}

void dump(const std::vector<int>& in,string tag,unsigned num)
{
  I3Vector<int> t;
  std::copy(in.begin(), in.end(), t.begin());
  dump(t, tag, num); 
}

struct printlaunches
{
  printlaunches(map<uint64_t,unsigned>& count) : mbidCount(count){}
  map<uint64_t,unsigned>& mbidCount;

  void operator()(const pair<uint64_t,I3DOMLaunch>& launch)

  {
    ostringstream fadcName;
    fadcName<<"pretz-waveforms/fadc_"<<hex<<launch.first;
    if(launch.second.GetRawFADC().size() > 0)
      dump(launch.second.GetRawFADC(),fadcName.str(),mbidCount[launch.first]);

    ostringstream atwd0Name;
    atwd0Name<<"pretz-waveforms/atwd_chan0_"<<hex<<launch.first;
    if(launch.second.GetRawATWD(0).size() > 0)
      dump(launch.second.GetRawATWD(0),atwd0Name.str(),mbidCount[launch.first]);
    
    ostringstream atwd1Name;
    atwd1Name<<"pretz-waveforms/atwd_chan1_"<<hex<<launch.first;
    if(launch.second.GetRawATWD(1).size() > 0)
      dump(launch.second.GetRawATWD(1),atwd1Name.str(),mbidCount[launch.first]);

    ostringstream atwd2Name;
    atwd2Name<<"pretz-waveforms/atwd_chan2_"<<hex<<launch.first;
    if(launch.second.GetRawATWD(2).size() > 0)
      dump(launch.second.GetRawATWD(2),atwd2Name.str(),mbidCount[launch.first]);
    


    mbidCount[launch.first] += 1;
  }
};

using namespace payload_parsing;

int main(int argc, char** argv)
{
  if(argc != 2)
    {
      cout<<"One arg, the ddd file to parse"<<endl;
      exit(1);
    }
  string eventFile = argv[1];
  ifstream ifs(eventFile.c_str());
  
  uint32_t eventSize;
  vector<char> eventData;
  unsigned int eventCount = 1;
  
  const char* i3work = getenv("I3_SRC");
  if(!i3work)
    {
      cout<<"Gotta have I3_SRC set"<<endl;
      exit(1);
    }
 
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
          cout<<"--------------------------------------------------"<<endl;
	  
          eventCount += 1;
        }

    }

}
