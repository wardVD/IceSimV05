/**
 * @brief This file contains the implementation of the OMKeyLink class.
 * @version $Id$
 * @file OMKeyLink.cxx
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
#include "recclasses/OMKeyLink.h"

//______________________________________________________________________________
OMKeyLink::
~OMKeyLink()
{}

//______________________________________________________________________________
template <typename Archive>
void
OMKeyLink::
serialize(Archive & ar, unsigned version)
{
    if(version > omkeylink_version_)
        log_fatal("Attempting to read version %u from file but running "
                  "version %u of OMKeyLink class.",
                  version, omkeylink_version_);

    ar & make_nvp("I3FrameObject", base_object< I3FrameObject >(*this));
    ar & make_nvp("OMKey1", omKey1_);
    ar & make_nvp("OMKey2", omKey2_);
}

I3_SERIALIZABLE(OMKeyLink);
I3_SERIALIZABLE(I3VectorOMKeyLink);

//______________________________________________________________________________
bool
operator==(const OMKeyLink& lhs, const OMKeyLink& rhs)
{
    if( ( lhs.GetOMKey1() == rhs.GetOMKey1() &&
          lhs.GetOMKey2() == rhs.GetOMKey2()
        ) ||
        ( lhs.GetOMKey1() == rhs.GetOMKey2() &&
          lhs.GetOMKey2() == rhs.GetOMKey1()
        )
      )
        return true;
    return false;
}

//______________________________________________________________________________
bool
operator<(const OMKeyLink& lhs, const OMKeyLink& rhs)
{
    if(lhs == rhs)
        return false;

    if((lhs.GetOMKey1() < rhs.GetOMKey1()) ||
       (lhs.GetOMKey2() < rhs.GetOMKey2())
      )
        return true;

    return false;
}

//______________________________________________________________________________
std::string
OMKeyLink::
str() const
{
    std::stringstream ss;
    ss << (*this);
    return ss.str();
}

//______________________________________________________________________________
std::ostream& operator<<(std::ostream& os, const OMKeyLink &link)
{
    os << "OMKeyLink(" << link.GetOMKey1() << ", "<< link.GetOMKey2() << ")";
    return os;
}
