/**
 * @version $Id$
 * @file OMKeyLink.h
 * @date $Date$
 *
 * Copyright (C) 2014
 * Martin Wolf <martin.wolf@fysik.su.se>
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
#ifndef RECCLASSES_OMKEYLINK_H_INCLUDED
#define RECCLASSES_OMKEYLINK_H_INCLUDED

#include <iostream>

#include <icetray/I3Logging.h>
#include "icetray/OMKey.h"
#include "icetray/I3FrameObject.h"
#include <boost/serialization/access.hpp>
#include <boost/serialization/version.hpp>

#include "dataclasses/I3Vector.h"

static const unsigned omkeylink_version_ = 0;

/**
 * @brief Stores two OMKeys where the order does not matter.
 *
 * OMKeyLink(a, b) is equal to OMKeyLink(b, a), if a and b are OMKeys.
 */
class OMKeyLink : public I3FrameObject
{
  public:
    /** The default constructor, needed for boost::serialization.
     */
    OMKeyLink()
    {}

    /** The normal constructor.
     */
    OMKeyLink(
        const OMKey &omKey1,
        const OMKey &omKey2
    )
      : omKey1_(omKey1),
        omKey2_(omKey2)
    {}

    virtual
    ~OMKeyLink();

    const OMKey & GetOMKey1() const { return omKey1_; }
    OMKey & GetOMKey1() { return omKey1_; }
    void SetOMKey1(const OMKey& omKey) { omKey1_ = omKey; }

    const OMKey & GetOMKey2() const { return omKey2_; }
    OMKey & GetOMKey2() { return omKey2_; }
    void SetOMKey2(const OMKey& omKey) { omKey2_ = omKey; }

    /**
     * Returns the string representation of the OMKeyLink object as a
     * std::string object.
     */
    std::string
    str() const;

  protected:
    OMKey omKey1_;
    OMKey omKey2_;

  private:
    friend class boost::serialization::access;

    template <class Archive>
    void serialize(Archive& ar, unsigned version);

    SET_LOGGER("OMKeyLink");
};

BOOST_CLASS_VERSION(OMKeyLink, omkeylink_version_);

/**
 * Equality comparison operator. Returns true, if the two OM links represent the
 * same OM link.
 */
bool
operator==(const OMKeyLink& lhs, const OMKeyLink& rhs);

/**
 * Less comparison operator.
 * Required for putting OMKeyLink into a std::map or std::set.
 */
bool
operator<(const OMKeyLink& lhs, const OMKeyLink& rhs);

std::ostream&
operator<<(std::ostream& os, const OMKeyLink &link);

I3_POINTER_TYPEDEFS(OMKeyLink);

typedef I3Vector<OMKeyLink> I3VectorOMKeyLink;
I3_POINTER_TYPEDEFS(I3VectorOMKeyLink);

#endif// ! STTOOLS_OMKEYLINK_H_INCLUDED
