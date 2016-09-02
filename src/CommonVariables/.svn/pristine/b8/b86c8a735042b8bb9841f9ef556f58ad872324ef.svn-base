/**
 * $Id$
 *
 * Copyright (C) 2012
 * Martin Wolf <martin.wolf@icecube.wisc.edu>
 * and the IceCube Collaboration <http://www.icecube.wisc.edu>
 *
 * @file I3DirectHitsTimeWindow.cxx
 * @version $Revision$
 * @date $Date$
 * @author Martin Wolf <martin.wolf@icecube.wisc.edu>
 * @brief This file contains the implementation of the I3DirectHitsTimeWindow
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

#include "CommonVariables/direct_hits/I3DirectHitsTimeWindow.h"

//______________________________________________________________________________
I3DirectHitsTimeWindow::
I3DirectHitsTimeWindow()
  : time0_(0.),
    time1_(0.)
{}

//______________________________________________________________________________
I3DirectHitsTimeWindow::
I3DirectHitsTimeWindow(double time0, double time1)
  : time0_(time0),
    time1_(time1)
{}

//______________________________________________________________________________
I3DirectHitsTimeWindow::
I3DirectHitsTimeWindow(const I3DirectHitsTimeWindow& tw)
  : time0_(tw.GetTime0()),
    time1_(tw.GetTime1())
{}

//______________________________________________________________________________
bool
I3DirectHitsTimeWindow::
operator==(const I3DirectHitsTimeWindow& rhs) const
{
    return (this->time0_ == rhs.time0_ || (std::isnan(this->time0_) && std::isnan(rhs.time0_))) &&
           (this->time1_ == rhs.time1_ || (std::isnan(this->time1_) && std::isnan(rhs.time1_)));
}

//______________________________________________________________________________
std::ostream&
operator<<(std::ostream& oss, const I3DirectHitsTimeWindow& rhs)
{
    oss << "I3DirectHitsTimeWindow("                                <<
               "time0 [ns]: " << rhs.GetTime0()/I3Units::ns << ", " <<
               "time1 [ns]: " << rhs.GetTime1()/I3Units::ns <<
           ")";
    return oss;
}
