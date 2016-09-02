/**
 * @brief This file contains the implementation of the OMKeySet class.
 * @version $Id$
 * @file OMKeySet.cxx
 * @date $Date$
 *
 * Copyright (C) 2014
 * Martin Wolf <martin.wolf@icecube.wisc.edu>
 * and the IceCube Collaboration <http://www.icecube.wisc.edu>
 *
 *        ----------------------------------------------------------------------
 *        This file is free software; you can redistribute it and/or modify
 *        it under the terms of the GNU General Public License as published by
 *        the Free Software Foundation; either version 3 of the License, or
 *        (at your option) any later version.
 *
 *        This program is distributed in the hope that it will be useful,
 *        but WITHOUT ANY WARRANTY; without even the implied warranty of
 *        MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *        GNU General Public License for more details.
 *
 *        You should have received a copy of the GNU General Public License
 *        along with this program.  If not, see <http://www.gnu.org/licenses/>
 */
#include <ostream>
#include <sstream>
#include <vector>

#include <boost/lexical_cast.hpp>

#include "recclasses/OMKeySet.h"

namespace sttools {

//______________________________________________________________________________
template<class T>
std::string
vector_to_string(std::vector<T> const &v)
{
    std::stringstream ss;
    ss << "[";
    typename T::const_iterator v_begin = v.begin();
    typename T::const_iterator v_end   = v.end();
    for(typename T::const_iterator i = v_begin; i != v_end; ++i)
    {
        if(i != v_begin)
            ss << ", ";
        ss << *i;
    }
    ss << "]";
    return ss.str();
}

//______________________________________________________________________________
std::string
omkey_range_vector_to_strings_string(std::vector<OMKeySet::omkey_range_t> const &v)
{
    std::stringstream ss;
    const std::vector<OMKeySet::omkey_range_t>::const_iterator v_begin = v.begin();
    const std::vector<OMKeySet::omkey_range_t>::const_iterator v_end   = v.end();
    std::vector<OMKeySet::omkey_range_t>::const_iterator it = v_begin;
    for(; it != v_end; ++it)
    {
        if(it != v_begin) {
            ss << ",";
        }

        if(it->string_end == it->string_begin+1)
        {
            ss << it->string_begin;
        }
        else
        {
            ss << it->string_begin << "-" << it->string_end-1;
        }
    }
    return ss.str();
}

//______________________________________________________________________________
std::string
omkey_range_vector_to_oms_string(std::vector<OMKeySet::omkey_range_t> const &v)
{
    std::stringstream ss;
    const std::vector<OMKeySet::omkey_range_t>::const_iterator v_begin = v.begin();
    const std::vector<OMKeySet::omkey_range_t>::const_iterator v_end   = v.end();
    std::vector<OMKeySet::omkey_range_t>::const_iterator it = v_begin;
    for(; it != v_end; ++it)
    {
        if(it != v_begin) {
            ss << ",";
        }

        if(it->om_end == it->om_begin+1)
        {
            ss << it->om_begin;
        }
        else
        {
            ss << it->om_begin << "-" << it->om_end-1;
        }
    }
    return ss.str();
}


//______________________________________________________________________________
std::vector<OMKeySet::omkey_range_t>
strings_oms_strings_to_omkey_range_vector(const std::vector<std::string>& strings, const std::vector<std::string>& oms)
{
    if(strings.size() != oms.size())
    {
        log_fatal("The number of elements for the string and OM range "
                    "lists must be equal.");
    }
    std::vector<OMKeySet::omkey_range_t> omkey_range_vec;
    std::vector<std::string>::const_iterator strings_it = strings.begin();
    std::vector<std::string>::const_iterator oms_it = oms.begin();
    for(; strings_it != strings.end(); ++strings_it, ++oms_it)
    {
        omkey_range_vec.push_back(OMKeySet::omkey_range_t());
        OMKeySet::ExtractOMKeyRange(*strings_it, *oms_it, omkey_range_vec.back());
    }
    return omkey_range_vec;
}


template<class T>
std::vector<T>
string_to_vector(const std::string &s, char delim);

template<>
std::vector<std::string>
string_to_vector(const std::string &s, char delim)
{
    std::vector<std::string> v;
    std::stringstream ss(s);
    std::string stritem;
    while(std::getline(ss, stritem, delim))
    {
        v.push_back(stritem);
    }
    return v;
}

}// namespace sttools

//______________________________________________________________________________
OMKeySet::
OMKeySet(const std::vector<std::string>& strings, const std::vector<std::string>& oms)
{
    omkey_range_vec_ = sttools::strings_oms_strings_to_omkey_range_vector(strings, oms);
}

//______________________________________________________________________________
OMKeySet::
~OMKeySet()
{}

//______________________________________________________________________________
bool
operator==(const OMKeySet& lhs, const OMKeySet& rhs)
{
    return (lhs.GetOMKeyRangeVector() == rhs.GetOMKeyRangeVector());
}

//______________________________________________________________________________
bool
operator==(const OMKeySet::omkey_range_t& lhs, const OMKeySet::omkey_range_t& rhs)
{
    return (lhs.string_begin == rhs.string_begin &&
            lhs.string_end   == rhs.string_end   &&
            lhs.om_begin     == rhs.om_begin     &&
            lhs.om_end       == rhs.om_end
           );
}

//______________________________________________________________________________
template <class Archive>
void
OMKeySet::
save(Archive & ar, unsigned version) const
{
    std::string strings = sttools::omkey_range_vector_to_strings_string(omkey_range_vec_);
    std::string oms     = sttools::omkey_range_vector_to_oms_string(omkey_range_vec_);
    ar & make_nvp("I3FrameObject", base_object< I3FrameObject >(*this));
    ar & make_nvp("Strings", strings);
    ar & make_nvp("OMs",     oms);
}

//______________________________________________________________________________
template <class Archive>
void
OMKeySet::
load(Archive & ar, unsigned version)
{
    if(version > omkeyset_version_)
        log_fatal("Attempting to read version %u from file but running "
                  "version %u of OMKeySet class.",
                  version, omkeyset_version_);

    std::string strings;
    std::string oms;
    ar & make_nvp("I3FrameObject", base_object< I3FrameObject >(*this));
    ar & make_nvp("Strings", strings);
    ar & make_nvp("OMs",     oms);
    std::vector<std::string> strings_vec = sttools::string_to_vector<std::string>(strings, ',');
    std::vector<std::string> oms_vec     = sttools::string_to_vector<std::string>(oms, ',');
    omkey_range_vec_ = sttools::strings_oms_strings_to_omkey_range_vector(strings_vec, oms_vec);
}

I3_SPLIT_SERIALIZABLE(OMKeySet);
I3_SERIALIZABLE(I3VectorOMKeySet);

//______________________________________________________________________________
void
OMKeySet::
ExtractOMKeyRange(std::string const & s, std::string const & o, omkey_range_t &range)
{
    const size_t sp = s.find('-');
    if(sp == std::string::npos)
    {
        // No hyphen found. Element should be a single integer expression.
        const int32_t number = boost::lexical_cast<int32_t>(s);
        range.string_begin = number;
        range.string_end   = number + 1;
    }
    else
    {
        // Hyphen found at position p. Element should be of the form
        // "START-END".
        range.string_begin = boost::lexical_cast<int32_t>(s.substr(0, sp));
        range.string_end   = boost::lexical_cast<int32_t>(s.substr(sp+1, std::string::npos)) + 1;
    }

    const size_t op = o.find('-');
    if(op == std::string::npos)
    {
        // No hyphen found. Element should be a single integer expression.
        const uint32_t number = boost::lexical_cast<uint32_t>(o);
        range.om_begin = number;
        range.om_end   = number + 1;
    }
    else
    {
        // Hyphen found at position p. Element should be of the form
        // "START-END".
        range.om_begin = boost::lexical_cast<uint32_t>(o.substr(0, op));
        range.om_end   = boost::lexical_cast<uint32_t>(o.substr(op+1, std::string::npos)) + 1;
    }
}

//______________________________________________________________________________
bool
OMKeySet::
Contains(OMKey const &omkey) const
{
    const int string = omkey.GetString();
    const unsigned int om = omkey.GetOM();

    std::vector<omkey_range_t>::const_iterator it = omkey_range_vec_.begin();
    const std::vector<omkey_range_t>::const_iterator it_end = omkey_range_vec_.end();
    for(; it != it_end; ++it)
    {
        if(string >= it->string_begin && string < it->string_end &&
           om     >= it->om_begin     && om     < it->om_end
          )
        {
            return true;
        }
    }

    return false;
}

//______________________________________________________________________________
bool
OMKeySet::
IsEmpty() const
{
    return (omkey_range_vec_.size() == 0);
}

//______________________________________________________________________________
I3VectorOMKey
OMKeySet::
GetOMKeys() const
{
    std::set<OMKey> omKeySet;

    std::vector<omkey_range_t>::const_iterator it = omkey_range_vec_.begin();
    const std::vector<omkey_range_t>::const_iterator it_end = omkey_range_vec_.end();

    for(; it != it_end; ++it)
    {
        for(int32_t string = it->string_begin; string < it->string_end; ++string)
        for(uint32_t om = it->om_begin; om < it->om_end; ++om)
        {
            omKeySet.insert(OMKey(string, om));
        }
    }

    I3VectorOMKey omKeys(omKeySet.begin(), omKeySet.end());
    return omKeys;
}

//______________________________________________________________________________
std::string
OMKeySet::
str() const
{
    std::stringstream ss;
    ss << (*this);
    return ss.str();
}

//______________________________________________________________________________
std::ostream& operator<<(std::ostream& os, const OMKeySet &rhs)
{
    os << "OMKeySet(["
       << sttools::omkey_range_vector_to_strings_string(rhs.GetOMKeyRangeVector())
       << "], ["
       << sttools::omkey_range_vector_to_oms_string(rhs.GetOMKeyRangeVector())
       << "])";
    return os;
}
