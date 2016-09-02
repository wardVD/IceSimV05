/**
 * $Id$
 *
 * Copyright (C) 2012
 * Martin Wolf <martin.wolf@icecube.wisc.edu>
 * and the IceCube Collaboration <http://www.icecube.wisc.edu>
 *
 * @file I3DirectHitsDefinition.cxx
 * @version $Revision$
 * @date $Date$
 * @author Martin Wolf <martin.wolf@icecube.wisc.edu>
 * @brief This file contains the implementation of the I3DirectHitsDefinition
 *        class, which is an I3FrameObject holding the values for a particular
 *        class of direct hits.
 *
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
#include <iostream>

#include "icetray/I3Units.h"

#include "CommonVariables/direct_hits/I3DirectHitsDefinition.h"

//______________________________________________________________________________
I3DirectHitsDefinition::
I3DirectHitsDefinition()
  : name_(""),
    timeWindow_(I3DirectHitsTimeWindow())
{
}

//______________________________________________________________________________
I3DirectHitsDefinition::
I3DirectHitsDefinition(std::string name, double time0, double time1)
  : name_(name),
    timeWindow_(I3DirectHitsTimeWindow(time0, time1))
{
}

//______________________________________________________________________________
I3DirectHitsDefinition::
I3DirectHitsDefinition(const I3DirectHitsDefinition& dhd)
  : name_(dhd.GetName()),
    timeWindow_(I3DirectHitsTimeWindow(dhd.GetTimeWindow()))
{
}

//______________________________________________________________________________
bool
I3DirectHitsDefinition::
operator==(const I3DirectHitsDefinition& rhs) const
{
    return (this->name_ == rhs.name_) &&
           (this->timeWindow_ == rhs.timeWindow_);
}

//______________________________________________________________________________
std::ostream&
operator<<(std::ostream& oss, const I3DirectHitsDefinition& rhs)
{
    oss << "I3DirectHitsDefinition("                 <<
               "Name: \"" << rhs.GetName() << "\", " <<
               rhs.GetTimeWindow()                   <<
           ")";
    return oss;
}
