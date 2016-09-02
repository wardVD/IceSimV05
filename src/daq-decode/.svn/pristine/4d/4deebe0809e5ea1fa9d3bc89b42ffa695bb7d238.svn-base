/**
 * @brief Testing the I3DAQFile class.
 * 
 * (c) 2005
 * the IceCube Collaboration
 * $Id$
 *
 * @file I3DAQFileTest.cxx
 * @version $Revision$
 * @date $Date$
 * @author tschmidt
 */

// class header files

#include <daq-decode/I3DAQFile.h>

#include <string>
#include <vector>

#include <stdint.h>
#ifndef __APPLE_CC__
#include <netinet/in.h>
#else
#include <sys/types.h>
#include <machine/endian.h>
#endif

#include <stdlib.h>
#include <time.h>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/iostreams/device/file.hpp>
#include <boost/iostreams/filter/gzip.hpp>

#include <I3Test.h>

// definitions


// namespace declarations

using namespace std;

// implementation

TEST_GROUP(I3DAQFileTest);

TEST(GetEvent)
{
  const char* i3_build = getenv("I3_BUILD");
  ENSURE(i3_build != NULL);
  ENSURE(boost::filesystem::exists(i3_build));
  ENSURE(boost::filesystem::is_directory(i3_build));

  string name = i3_build + string("/I3DAQFileTest_temporary_test_file.ddd.gz");
  boost::iostreams::filtering_ostream ofs;
  ofs.push(boost::iostreams::gzip_compressor());
  ofs.push(boost::iostreams::file_sink(name));
  try
  {
    ENSURE(ofs.good());

    srand((unsigned int) time(NULL));
    uint32_t eventSize = 100;
    vector<char> event1(eventSize);
    for(unsigned int i = 0; i < event1.size(); ++i)
      event1[i] = (rand() % 255) - 127;
    eventSize = htonl(eventSize);
    memcpy((void *)(&(event1[0])), (void *)(&(eventSize)), sizeof(uint32_t));
    ofs.write(&(event1[0]), event1.size());
    ENSURE(ofs.good());
    boost::iostreams::close(ofs);

    I3DAQFile myfile(name);
    ENSURE_EQUAL(name, myfile.GetName());
    ENSURE(myfile.GetHeader().empty());
    vector<char> buffer;
    ENSURE(myfile.GetEvent(buffer));
    ENSURE_EQUAL(event1.size(), buffer.size());
    ENSURE(event1 == buffer);

    ENSURE(!myfile.GetEvent(buffer));

    boost::filesystem::remove(name);
  }
  catch(...)
  {
    boost::filesystem::remove(name);
    throw;
  }
}

