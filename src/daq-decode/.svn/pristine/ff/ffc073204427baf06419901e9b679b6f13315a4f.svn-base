#ifndef DAQ_DECODE_EVENTDECODINGSTRESSTEST_H
#define DAQ_DECODE_EVENTDECODINGSTRESSTEST_H

#include <I3Test.h>

#include <string>
#include <sstream>
#include <fstream>
#include <vector>

#include <sys/types.h>
#include <unistd.h>
       
#ifndef __APPLE_CC__
#include <netinet/in.h>
#else
#include <sys/types.h>
#include <machine/endian.h>
#endif

#include "phys-services/I3FileOMKey2MBID.h"
#include "icetray/I3Frame.h"
#include "dataclasses/I3Time.h"

#include "payloads-unpacker/service/I3DAQEventDecoderPXX.h"

namespace daq_decode_test
{
  
  static const double PERCENTAGE = 0.03;
  
  
#ifdef __APPLE_CC__
  
  class StatMInfo
    {
    public:
      StatMInfo() {}
      double GetMemoryUsage() const { return 1.0; }
    };
  
#else
  
  class StatMInfo
    {
    private:
      double pageSize_;
      pid_t pid_;
      string src_;
      
      void Initialize(pid_t pid)
        {
          ostringstream oss;
          oss << "/proc/" << pid << "/statm";
          src_ = oss.str();
        }
      
    public:
      StatMInfo(pid_t pid) : pageSize_(getpagesize()), pid_(pid)
        {
          Initialize(pid_);
        }
      
      StatMInfo() : pageSize_(getpagesize()), pid_(getpid())
        {
          Initialize(pid_);
        }
      
      double GetMemoryUsage() const
        {
          ifstream ifs(src_.c_str());
          
          if(!ifs.is_open() || !ifs.good())
            log_fatal("cannot open %s", src_.c_str());
          
          unsigned long long int buffer;
          bool done;
          vector<unsigned long long int> statMInfo;
          do
            {
              ifs >> buffer;
              if(!(done = !(ifs.good() && !ifs.eof())))
                statMInfo.push_back(buffer);
            }while(!done);
          
          ifs.close();
          
          return statMInfo.at(1) * pageSize_;
        }
      
      pid_t GetPID() const{ return pid_; }
      
      double GetPageSize() const{ return pageSize_; }
    };
  
#endif
  
  void
    initI3DAQEventDecoderTest(I3OMKey2MBIDPtr& omkey2mbid, string& eventFile)
    {
      ENSURE(getenv("I3_SRC"));
      string omkey2mbidFile(getenv("I3_SRC"));
      ENSURE(!omkey2mbidFile.empty());
      omkey2mbidFile.append("/phys-services/resources/doms.txt");
      omkey2mbid = I3OMKey2MBIDPtr(new I3FileOMKey2MBID(omkey2mbidFile));
      ENSURE(omkey2mbid);
      
      ENSURE(getenv("I3_TESTDATA")); 
      eventFile = getenv("I3_TESTDATA");
      ENSURE(!eventFile.empty());
      eventFile.append("/testdaq-monolith/");
      eventFile.append("ic_SPS-DAQ-STRINGPROC01_run0000999_LocalCoincidence-IniceIcetop-ATWD0.events");
    }
  
  double
    checkMemUsage(const StatMInfo& statMInfo,
                  double memUsage, double percentage)
    {
      double actualMemUsage = statMInfo.GetMemoryUsage();
      ENSURE_DISTANCE(actualMemUsage, memUsage,  percentage * memUsage,
                      "Error memory usage has changed.  Memory leak?" );
      
      return actualMemUsage;
    }
  
  
  unsigned int
    EventDecodingStressTest(I3DAQEventDecoder& decoder,
                            const string& eventFile,
                            unsigned int nEvent,
                            unsigned int nMemoryCheck,
                            unsigned int nProcessingPerCheck)
    {
      StatMInfo statMInfo;
      
      ifstream ifs(eventFile.c_str(), ios::binary);
      ENSURE(ifs.is_open() && ifs.good());
      
      uint32_t eventSize;
      vector<char> eventData;
      double initialMemUsage;
      unsigned int i = 0;
      do
        {
          ifs.read((char *) &eventSize, 4);
          if(!ifs.eof())
            {
              ENSURE(ifs.good());
              
              eventSize = ntohl(eventSize);
              ENSURE(eventSize);
              
              eventData.resize(eventSize);
              ifs.seekg(-4, ios::cur);
              ifs.read(&(eventData[0]), eventData.size());
              ENSURE(!ifs.eof() && ifs.good());
              
              for(unsigned int j = 0; j < nMemoryCheck; ++j)
                {
                  for(unsigned int k = 0; k < nProcessingPerCheck; ++k)
                    {
                      I3Frame frame(I3Frame::DAQ);
                      decoder.FillEvent(frame, eventData);
                    }
                  if(j) checkMemUsage(statMInfo, initialMemUsage, PERCENTAGE);
                  else initialMemUsage = statMInfo.GetMemoryUsage();
                }
            }
        } while(!ifs.eof() && (++i < nEvent));
      
      ifs.close();
      
      return i;
    }
  
};

#endif
