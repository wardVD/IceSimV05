/**
 * copyright  (C) 2004
 * the icecube collaboration
 * $Id$
 *
 * @file I3DAQFile.cxx
 * @brief implementation of the I3DAQFile class
 * @version $Revision$
 * @date $Date$
 * @author tschmidt
 */

// class header file

#include <daq-decode/I3DAQFile.h>

#include <stdint.h>
#ifndef __APPLE_CC__
#include <netinet/in.h>
#else
#include <sys/types.h>
#include <machine/endian.h>
#endif

#include <time.h>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/iostreams/device/file.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include "boost/algorithm/string.hpp"

// namespace declarations

using namespace std;
namespace ba = boost::algorithm;

// implementation

I3DAQFile::I3DAQFile()
  : valid_(false)
{
}


I3DAQFile::I3DAQFile(const string& name)
{
  Open(name);
}


I3DAQFile::~I3DAQFile()
{
}
  

void I3DAQFile::Open(const string& name)
{
  name_ = name;
  Open();
}


void I3DAQFile::Open()
{
  valid_ = false;

  if(name_.empty()) log_fatal("undefined file name");
  if(!boost::filesystem::exists(name_) || boost::filesystem::is_directory(name_))
    log_fatal("cannot find file \"%s\"", name_.c_str());
  
  ifs_ =
    boost::shared_ptr<boost::iostreams::filtering_istream>(new boost::iostreams::filtering_istream());
  if (ba::ends_with(name_, ".gz"))
  {
    ifs_->push(boost::iostreams::gzip_decompressor());
    log_info("file \"%s\" ends in .gz - using gzip decompressor", name_.c_str());
  }
  log_info("opening file \"%s\"", name_.c_str());
  ifs_->push(boost::iostreams::file_source(name_));
  if(!ifs_->good()) log_fatal("cannot open file \"%s\"", name_.c_str());
  
  valid_ = true;
}

  
const string& I3DAQFile::GetName() const
{
  return(name_);
}


const vector<char>& I3DAQFile::GetHeader() const
{
  if(!valid_) log_fatal("invalid header");
  
  return(dummy_);
}


const vector<char>& I3DAQFile::GetHeader()
{
  if(!valid_) Open();
  
  return(dummy_);
}


bool I3DAQFile::GetEvent(vector<char>& buffer)
{
  bool retVal;
  uint32_t eventSize;
  
  if(!valid_) Open();
  
  ifs_->read((char *)(&eventSize), sizeof(uint32_t));
  if((retVal = !ifs_->eof()))
  {
    if(!ifs_->good())
      log_fatal("cannot read block length from file \"%s\"", name_.c_str());
    eventSize = ntohl(eventSize);
    if(eventSize <= sizeof(uint32_t))
      log_fatal("invalid event size");

    vector<char> tmp(eventSize);
    eventSize = htonl(eventSize);
    memcpy((void *)(&(tmp[0])), (void *)(&(eventSize)), sizeof(uint32_t));    
    ifs_->read(&(tmp[sizeof(uint32_t)]), tmp.size() - sizeof(uint32_t));
    if(ifs_->eof() || !ifs_->good())
      log_fatal("cannot read event data from file \"%s\"", name_.c_str());
    buffer.swap(tmp);
  }
  
  return(retVal);
}
