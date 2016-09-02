/**
 * @version $Id$
 * @file public/recclasses/OMKeyLinkSet.h
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

#ifndef RECCLASSES_OMKEYLINKSET_H_INCLUDED
#define RECCLASSES_OMKEYLINKSET_H_INCLUDED

#include <string>

#include "icetray/I3FrameObject.h"
#include <boost/serialization/access.hpp>
#include <boost/serialization/version.hpp>
#include <icetray/I3Logging.h>

#include "recclasses/OMKeyLink.h"
#include "recclasses/OMKeySet.h"

static const unsigned omkeylinkset_version_ = 0;

/**
 * @brief Stores a pair of OMKeySet objects, where the order does not matter
 * (see project CommonVariables).
 *
 * OMKeyLinkSet(a, b) is equal to OMKeyLinkSet(b, a), if a and b are
 * OMKeySets. OMKeySet compactly stores ranges of strings and OM numbers.
 */
class OMKeyLinkSet : public I3FrameObject
{
  public:
    /** The default constructor, needed by boost::serialization.
     */
    OMKeyLinkSet()
    {}

    virtual
    ~OMKeyLinkSet();

    /** The normal constructor.
     */
    OMKeyLinkSet(const OMKeySet& omKeySet1, const OMKeySet& omKeySet2)
      : omKeySet1_(omKeySet1),
        omKeySet2_(omKeySet2)
    {}

    const OMKeySet& GetOMKeySet1() const { return omKeySet1_; }
    OMKeySet& GetOMKeySet1() { return omKeySet1_; }
    void SetOMKeySet1(const OMKeySet &omKeySet) { omKeySet1_ = omKeySet; }

    const OMKeySet& GetOMKeySet2() const { return omKeySet2_; }
    OMKeySet& GetOMKeySet2() { return omKeySet2_; }
    void SetOMKeySet2(const OMKeySet &omKeySet) { omKeySet2_ = omKeySet; }

    /** Checks if the given OMKeyLink is part of this OMKey link set definition.
     */
    bool
    Contains(OMKeyLink const &omkeylink) const;

    std::string str() const;

  protected:
    OMKeySet omKeySet1_;
    OMKeySet omKeySet2_;

  private:
    friend class boost::serialization::access;

    template <class Archive>
    void serialize(Archive& ar, unsigned version);

    SET_LOGGER("OMKeyLinkSet");
};

BOOST_CLASS_VERSION(OMKeyLinkSet, omkeylinkset_version_);

/**
 * Equality comparison operator. Returns true, if the two OMKeyLinkSet objects
 * represent the same OMKey link set.
 * This operator is needed to put an OMKeyLinkSet object into a std::vector.
 */
bool
operator==(const OMKeyLinkSet& lhs, const OMKeyLinkSet& rhs);

std::ostream&
operator<<(std::ostream& os, const OMKeyLinkSet &rhs);

I3_POINTER_TYPEDEFS(OMKeyLinkSet);

typedef I3Vector<OMKeyLinkSet> I3VectorOMKeyLinkSet;
I3_POINTER_TYPEDEFS(I3VectorOMKeyLinkSet);

#endif // ! STTOOLS_OMKEYLINKSET_H_INCLUDED
