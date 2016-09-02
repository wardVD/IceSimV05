/**
 * @version $Id$
 * @file OMKeySet.h
 * @date $Date$
 *
 * Copyright (C) 2014
 * Martin Wolf <martin.wolf@fysik.su.se>
 * and the IceCube Collaboration <http://www.icecube.wisc.edu>
 *
 * ----------------------------------------------------------------------
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 */

#ifndef RECCLASSES_OMKEYSET_H_INCLUDED
#define RECCLASSES_OMKEYSET_H_INCLUDED

#include <string>
#include <vector>

#include "icetray/OMKey.h"
#include "icetray/I3FrameObject.h"
#include <boost/serialization/access.hpp>
#include <boost/serialization/version.hpp>

#include "dataclasses/I3Vector.h"

static const unsigned omkeyset_version_ = 0;

/**
 * @brief Stores a selection of OMKeys as a collection of ranges.
 *
 * The class stores a collection of ranges, specifying string and OM
 * numbers and at the beginning and end of each range. It offers a
 * compact way to describe large parts of the detector, for example,
 * a veto region, or a sub-detector, like DeepCore or IceTop.
 */
class OMKeySet : public I3FrameObject
{
  public:
    /**
     * @brief Holds a range of strings and OM numbers.
     *
     * Ranges for strings [string_start, string_end) and OM numbers
     * [om_start, om_end) follow the usual semi-open convention in C++,
     * i.e. the value of start is included and the value of end is
     * excluded from the interval.
     */
    struct omkey_range_t
    {
        // We need to take signed integers for strings, because AMANDA strings
        // have negative numbers. Just for the unlikely case ... ;o)
        int32_t string_begin;
        int32_t string_end;
        uint32_t om_begin;
        uint32_t om_end;
    };

    /** Extracts a range from the given string expression. Range string
     *  expression could be: "START-END" or "NUMBER", where START, END, and
     *  NUMBER are integer expressions.
     */
    static
    void
    ExtractOMKeyRange(std::string const & s, std::string const & o, omkey_range_t &range);

    /** The default constructor, needed by boost::serialization.
     */
    OMKeySet()
    {}

    /** The normal constructor. Constructs an OMKey set based on a list of
     *  string-om ranges. The ranges are specified via strings. Each string can
     *  be a single integer number, e.g. "36", or a range of integer numbers,
     *  e.g. "1-86".
     */
    OMKeySet(const std::vector<std::string>& strings, const std::vector<std::string>& oms);

    virtual
    ~OMKeySet();

    const std::vector<omkey_range_t>& GetOMKeyRangeVector() const {
        return omkey_range_vec_;
    }

    /** Checks if the given OMKey is part of this OMKey set definition.
     */
    bool
    Contains(OMKey const &omkey) const;

    /** Checks if this OMKey set defines any OMKeys at all.
     */
    bool
    IsEmpty() const;

    /** Generates an I3VectorOMKey with all OMs which are defined by this OMKey
     *  set.
     */
    I3VectorOMKey
    GetOMKeys() const;

    std::string str() const;

  protected:
    std::vector<omkey_range_t> omkey_range_vec_;

  private:
    friend class boost::serialization::access;

    template<class Archive> void save(Archive & ar, unsigned version) const;
    template<class Archive> void load(Archive & ar, unsigned version);
    BOOST_SERIALIZATION_SPLIT_MEMBER();

    SET_LOGGER("OMKeySet");
};

BOOST_CLASS_VERSION(OMKeySet, omkeyset_version_);

/**
 * Equality comparison operator. Returns true, if the two OMKeySet objects
 * represent the same OMKey set.
 * This operator is needed to put an OMKeySet object into a std::vector.
 */
bool operator==(const OMKeySet& lhs, const OMKeySet& rhs);

bool operator==(const OMKeySet::omkey_range_t& lhs, const OMKeySet::omkey_range_t& rhs);

std::ostream&
operator<<(std::ostream& os, const OMKeySet &rhs);

I3_POINTER_TYPEDEFS(OMKeySet);

typedef I3Vector<OMKeySet> I3VectorOMKeySet;
I3_POINTER_TYPEDEFS(I3VectorOMKeySet);

#endif// ! STTOOLS_OMKEYSET_H_INCLUDED
