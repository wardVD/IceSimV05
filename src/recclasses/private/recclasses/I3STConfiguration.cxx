/**
 * @brief This file contains the implementation of the I2STConfiguration
 *        class.
 * @version $Id$
 * @file I3STConfiguration.cxx
 * @date $Date$
 *
 * Copyright (C) 2013
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
#include <ostream>

#include "icetray/serialization.h"
#include "dataclasses/ostream_overloads.hpp"
#include "recclasses/I3STConfiguration.h"

//______________________________________________________________________________
I3STConfiguration::
~I3STConfiguration()
{}

//______________________________________________________________________________
template <class Archive>
void
I3STConfiguration::
serialize(Archive &ar, unsigned version)
{
    if(version > i3stconfiguration_version_) {
        log_fatal("Attempting to read version %u from file but running version "
                  "%u of I3STConfiguration class.",
                  version, i3stconfiguration_version_
        );
    }

    ar & make_nvp("I3FrameObject", base_object<I3FrameObject>(*this));
    ar & make_nvp("Name",          this->name_);
    ar & make_nvp("OMKeyLinkSets", this->omKeyLinkSets_);
}

I3_SERIALIZABLE(I3STConfiguration);
I3_SERIALIZABLE(I3VectorSTConfiguration);

//______________________________________________________________________________
std::string
I3STConfiguration::
GetPrettySettingsStr(unsigned int nLeadingWS) const
{
    std::string ws(nLeadingWS, ' ');

    std::stringstream ss;
    ss << ws << "Name:          " << GetName() << std::endl <<
          ws << "OMKeyLinkSets: " << GetOMKeyLinkSets()
          ;
    return ss.str();
}

//______________________________________________________________________________
std::ostream&
operator<<(std::ostream& os, const I3STConfiguration &rhs)
{
    os << "I3STConfiguration:" << std::endl
       << rhs.GetPrettySettingsStr(4);
    return os;
}
