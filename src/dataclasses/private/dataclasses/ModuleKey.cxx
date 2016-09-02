/**
 *  $Id: ModuleKey.cxx 94948 2012-11-04 16:21:52Z nwhitehorn $
 *  
 *  Copyright (C) 2012
 *  The IceCube Collaboration <http://www.icecube.wisc.edu>
 *  
 *  This file is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>
 *  
 */
#include <icetray/serialization.h>
#include <dataclasses/ModuleKey.h>
#include <sstream>

ModuleKey::~ModuleKey() { }

template <typename Archive>
void 
ModuleKey::serialize (Archive & ar, unsigned version)
{
  if (version>modulekey_version_)
    log_fatal("Attempting to read version %u from file but running version %u of ModuleKey class.",version,modulekey_version_);

  ar & make_nvp("I3FrameObject", base_object< I3FrameObject >(*this));
  ar & make_nvp("StringNumber",  stringNumber_);
  ar & make_nvp("OMNumber",  omNumber_);
}

I3_SERIALIZABLE(ModuleKey);
I3_SERIALIZABLE(I3MapModuleKeyString);

#include <sstream>
#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>

using namespace std;

ostream& operator<<(ostream& os, const ModuleKey& key)
{
  os << "ModuleKey(" << key.GetString() << "," << key.GetOM() << ")";
  return os;
}

istream& operator>>(istream& is, ModuleKey& key)
{
  std::string s;
  is >> s;

  boost::regex reg("ModuleKey\\(([\\-\\d]+),(\\d+)\\)");
  boost::smatch matches;
  if (!boost::regex_search(s, matches, reg))
    log_fatal("Error parsing ModuleKey value from string \"%s\"", s.c_str());

  ModuleKey newkey;
  log_trace("matches: %s %s", matches.str(1).c_str(), matches.str(2).c_str());

  newkey.SetString(boost::lexical_cast<int>(matches.str(1)));
  newkey.SetOM(boost::lexical_cast<unsigned>(matches.str(2)));
  key = newkey;
  return is;
}

string ModuleKey::str() const
{
  stringstream s;
  s<<*this;
  return s.str();
}



