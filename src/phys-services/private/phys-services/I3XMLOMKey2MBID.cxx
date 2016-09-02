/**
 * copyright  (C) 2004
 * the icecube collaboration
 * $Id: I3XMLOMKey2MBID.cxx 142697 2016-03-01 19:25:51Z jvansanten $
 *
 * @file phys-services/I3XMLOMKey2MBID.cxx
 * @brief implementation of the I3XMLOMKey2MBID class
 * @version $Revision: 142697 $
 * @date $Date: 2016-03-01 13:25:51 -0600 (Tue, 01 Mar 2016) $
 * @author tschmidt
 */

// class header files

#include <phys-services/I3XMLOMKey2MBID.h>
#include <icetray/open.h>

#include <utility>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/device/file.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/utility.hpp>
#include "boost/algorithm/string.hpp"

// definitions


// namespace declarations

using namespace std;
namespace ba = boost::algorithm;

// implementation

void I3XMLOMKey2MBID::Dump(const string& outfile,
                           const map<long long int, OMKey>& conversionTable)
{
  if(outfile.empty()) log_fatal("undefined file name");
  
  boost::iostreams::filtering_ostream ofs;
  I3::dataio::open(ofs, outfile);
  boost::archive::xml_oarchive oa(ofs);
  
  oa << boost::serialization::make_nvp("MBID_OMKey", conversionTable);
}


I3XMLOMKey2MBID::I3XMLOMKey2MBID(const string& infile)
  : I3OMKey2MBID()
{
  if(infile.empty()) log_fatal("undefined file name");
  if(!boost::filesystem::exists(infile)
     || boost::filesystem::is_directory(infile))
    log_fatal("cannot find file \"%s\"", infile.c_str());
  
  boost::iostreams::filtering_istream ifs;
  I3::dataio::open(ifs, infile);

  if(!ifs.good()) log_fatal("cannot open file \"%s\"", infile.c_str());
  boost::archive::xml_iarchive ia(ifs);
  
  ia >> boost::serialization::make_nvp("MBID_OMKey", mbid2omkey_);

  map<long long int, OMKey>::const_iterator iter;
  for(iter = mbid2omkey_.begin(); iter != mbid2omkey_.end(); ++iter)
    if(!(omkey2mbid_.insert(make_pair(iter->second, iter->first))).second)
      log_fatal("duplicated OM key - (%d, %u)",
                iter->second.GetString(), iter->second.GetOM());
}


I3XMLOMKey2MBID::~I3XMLOMKey2MBID()
{
}
