/**
 * @brief This file contains the implementation of the OMKeyLinkSet class.
 * @version $Id$
 * @file OMKeyLinkSet.cxx
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
#include <icetray/python/stream_to_string.hpp>
#include "recclasses/OMKeyLinkSet.h"
#include <boost/lexical_cast.hpp>

//______________________________________________________________________________
OMKeyLinkSet::
~OMKeyLinkSet()
{}

//______________________________________________________________________________
template <typename Archive>
void
OMKeyLinkSet::
serialize(Archive & ar, unsigned version)
{
    if(version > omkeylinkset_version_)
        log_fatal("Attempting to read version %u from file but running "
                  "version %u of OMKeyLinkSet class.",
                  version, omkeylinkset_version_);

    ar & make_nvp("I3FrameObject", base_object< I3FrameObject >(*this));
    ar & make_nvp("OMKeySet1", omKeySet1_);
    ar & make_nvp("OMKeySet2", omKeySet2_);
}

I3_SERIALIZABLE(OMKeyLinkSet);
I3_SERIALIZABLE(I3VectorOMKeyLinkSet);

//______________________________________________________________________________
bool
operator==(const OMKeyLinkSet& lhs, const OMKeyLinkSet& rhs)
{
    if( ( lhs.GetOMKeySet1() == rhs.GetOMKeySet1() &&
          lhs.GetOMKeySet2() == rhs.GetOMKeySet2()
        ) ||
        ( lhs.GetOMKeySet1() == rhs.GetOMKeySet2() &&
          lhs.GetOMKeySet2() == rhs.GetOMKeySet1()
        )
      )
        return true;
    return false;
}

//______________________________________________________________________________
bool
OMKeyLinkSet::
Contains(OMKeyLink const &omkeylink) const
{
    if( ( omKeySet1_.Contains(omkeylink.GetOMKey1()) &&
          omKeySet2_.Contains(omkeylink.GetOMKey2())
        ) ||
        ( omKeySet1_.Contains(omkeylink.GetOMKey2()) &&
          omKeySet2_.Contains(omkeylink.GetOMKey1())
        )
      )
        return true;
    return false;
}

//______________________________________________________________________________
std::ostream& operator<<(std::ostream& os, const OMKeyLinkSet &rhs)
{
    os << "OMKeyLinkSet(" << rhs.GetOMKeySet1() << ", " << rhs.GetOMKeySet2() << ")";
    return os;
}
