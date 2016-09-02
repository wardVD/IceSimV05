#include <fstream>
#include <iostream>
#include <string>
#ifndef __APPLE_CC__
#include <netinet/in.h>
#else
#include <sys/types.h>
#include <machine/endian.h>
#endif

#include "boost/algorithm/string.hpp"

#include "payload-parsing/decode.h"
#include "payload-parsing/utility.h"

#include "phys-services/I3XMLOMKey2MBID.h"
#include "phys-services/I3FileOMKey2MBID.h"

using namespace boost::algorithm;

int main(int argc,char** argv)
{
  if(argc < 2)
    {
      std::cerr << "supply infile" << std::endl;
      return(1);
    }
  if(argc < 3)
  {
    std::cerr << "supply mainboard ID to OMKey mapping" << std::endl;
    return(1);
  }
  if(argc < 4)
  {
    std::cerr << "supply outfile" << std::endl;
    return(1);
  }

  std::string infile = argv[1];
  std::string idfile = argv[2];
  std::string outfile = argv[3];
  
  I3OMKey2MBID* idMap = NULL;
  if (ends_with(idfile, ".txt"))
    idMap = new I3FileOMKey2MBID(idfile);
  else if (ends_with(idfile, ".xml") || ends_with(idfile, ".xml.gz"))
    idMap = new I3XMLOMKey2MBID(idfile);
  else {
    std::cerr << "invalid mainboard ID to OMKey mapping" << std::endl;
    return(1);
  }
    
  std::cout << "Reading: " << infile << std::endl;

  std::ifstream ifs(infile.c_str());

  if (ifs.eof())
    {
      std::cerr << "Empty file" << std::endl;
      return(1);
    }

  uint32_t eventSize, eventType=0;
  std::vector<char> eventData;

  std::ofstream fout(outfile.c_str());

  boost::shared_ptr<I3Map<OMKey,std::vector<I3DOMLaunch> > > launches(new I3Map<OMKey,std::vector<I3DOMLaunch> >());
  unsigned int nLaunches=0;
  while(ifs.peek() != EOF)
    {
      ifs.read((char *) &eventSize, 4);
      ifs.read((char *) &eventType, 4);
      eventSize = ntohl(eventSize);
      eventType = ntohl(eventType);
      if(!ifs.eof() && eventType==3)
        {
          if(!eventSize)
            std::cout << "event size is 0!?!" << std::endl;
          
          eventData.resize(eventSize);
          ifs.seekg(-8, std::ios::cur);
          ifs.read(&(eventData[0]), eventData.size());

          payload_parsing::DecodeTarget<3>::type target;
          payload_parsing::DecodeConfiguration config(0,0,true,true);
          payload_parsing::decode_payload<3>(target,config,eventData,0);
          
          (*launches)[idMap->GetOMKey(target.mbid)].push_back(target.launch);
          nLaunches++;
        }
      else{
        if(eventType!=3)
          std::cout << "Stopping at payload of unexpected type (" << eventType << ')' << std::endl;
        break;
      }
    }
  std::cout << "Decoded " << nLaunches << " launches" << std::endl;
  I3Frame frame(I3Frame::DAQ);
  frame.Put("RawLaunches",launches);
  std::cout << "Writing " << outfile << std::endl;
  frame.save(fout);
  delete idMap;
  return(0);
}
