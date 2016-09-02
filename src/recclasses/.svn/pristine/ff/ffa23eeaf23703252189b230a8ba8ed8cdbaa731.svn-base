/**
 * Copyright (C) 2011 - 2013
 * Martin Wolf <martin.wolf@fysik.su.se>, Robert Franke
 * and the IceCube Collaboration <http://www.icecube.wisc.edu>
 *
 * @version $Id$
 * @file OMKeyPair.h
 * @date $Date$
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

#ifndef RECCLASSES_OMKEYPAIR_H_INCLUDED
#define RECCLASSES_OMKEYPAIR_H_INCLUDED

#include <iostream>
#include <sstream>

#include "icetray/OMKey.h"
#include "icetray/I3FrameObject.h"
#include <boost/serialization/access.hpp>
#include <boost/serialization/version.hpp>

#include "dataclasses/I3Vector.h"

static const unsigned omkeypair_version_ = 0;

/**
 * @brief Stores a pair of OMKeys, where the order matters.
 *
 * OMKeyPair(a, b) is not equal OMKeyPair(b, a), if a and b are OMKeys.
 */
class OMKeyPair : public I3FrameObject
{
public:
    OMKeyPair(
        const OMKey &key1,
        const OMKey &key2
    )
      : key1_(key1),
        key2_(key2)
    {}

    virtual
    ~OMKeyPair();

    inline OMKey GetKey1() const { return key1_; }
    inline OMKey GetKey2() const { return key2_; }

    inline void SetKey1(const OMKey &k) { key1_ = k; }
    inline void SetKey2(const OMKey &k) { key2_ = k; }

    //////
    // Public class methods.
    std::string str() const;

    /**
     * Returns an OMKeyPair with reversed OMKeys.
     */
    OMKeyPair
    reversed() const;

protected:
    OMKey key1_;
    OMKey key2_;

    /** The default constructor, needed by boost::serialization.
     */
    OMKeyPair()
    {}

private:
    friend class boost::serialization::access;

    template <class Archive>
    void serialize(Archive& ar, unsigned version);

    SET_LOGGER("OMKeyPair");
};

BOOST_CLASS_VERSION(OMKeyPair, omkeypair_version_);

bool
operator==(const OMKeyPair &pair1, const OMKeyPair &pair2);

inline
bool
operator<(const OMKeyPair& lhs, const OMKeyPair& rhs)
{
    if(lhs.GetKey1() < rhs.GetKey1() ||
       lhs.GetKey2() < rhs.GetKey2())
        return true;

    return false;
}

std::ostream&
operator<<(std::ostream& os, const OMKeyPair &pair);

I3_POINTER_TYPEDEFS(OMKeyPair);

typedef I3Vector<OMKeyPair> I3VectorOMKeyPair;
I3_POINTER_TYPEDEFS(I3VectorOMKeyPair);

#endif// ! STTOOLS_OMKEYPAIR_H_INCLUDED
