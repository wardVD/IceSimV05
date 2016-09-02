/**
 * @brief This file contains the implementation of the OMKeyPair class.
 * @version $Id$
 * @file OMKeyPair.cxx
 * @date $Date$
 *
 * Copyright (C) 2013
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
#include "recclasses/OMKeyPair.h"

using namespace std;

//______________________________________________________________________________
OMKeyPair::
~OMKeyPair()
{}

//______________________________________________________________________________
std::string
OMKeyPair::
str() const
{
    stringstream ss;
    ss << (*this);
    return ss.str();
}

//______________________________________________________________________________
OMKeyPair
OMKeyPair::
reversed() const
{
    return OMKeyPair(key2_, key1_);
}

//______________________________________________________________________________
template <typename Archive>
void
OMKeyPair::
serialize(Archive & ar, unsigned version)
{
    if(version > omkeypair_version_)
        log_fatal("Attempting to read version %u from file but running "
                  "version %u of OMKeyPair class.",
                  version, omkeypair_version_);

    ar & make_nvp("I3FrameObject", base_object< I3FrameObject >(*this));
    ar & make_nvp("OMKey1", key1_);
    ar & make_nvp("OMKey2", key2_);
}

I3_SERIALIZABLE(OMKeyPair);
I3_SERIALIZABLE(I3VectorOMKeyPair);

//______________________________________________________________________________
bool
operator==(const OMKeyPair &pair1, const OMKeyPair &pair2)
{
    return ( (pair1.GetKey1() == pair2.GetKey1()) &&
             (pair1.GetKey2() == pair2.GetKey2())
           );
}

//______________________________________________________________________________
ostream& operator<<(ostream& os, const OMKeyPair &pair)
{
    os << "OMKeyPair(" << pair.GetKey1() << "," << pair.GetKey2() << ")";
    return os;
}
