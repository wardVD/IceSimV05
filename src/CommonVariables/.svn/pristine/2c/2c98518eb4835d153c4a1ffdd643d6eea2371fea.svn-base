/**
 * $Id$
 *
 * Copyright (C) 2012
 * Martin Wolf <martin.wolf@icecube.wisc.edu>
 * and the IceCube Collaboration <http://www.icecube.wisc.edu>
 *
 * @file I3DirectHitsTimeWindow.h
 * @version $Revision$
 * @date $Date$
 * @author Martin Wolf <martin.wolf@icecube.wisc.edu>
 * @brief This file contains the definition of the I3DirectHitsTimeWindow
 *        class, describing a time window of a particular class of direct hits.
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
 *
 */
#ifndef I3DIRECTHITSTIMEWINDOW_H_INCLUDED
#define I3DIRECTHITSTIMEWINDOW_H_INCLUDED

#include <iostream>
#include <string>
#include <vector>

#include "icetray/I3Logging.h"
#include "icetray/I3PointerTypedefs.h"

//==============================================================================
/**
 * @class I3DirectHitsTimeWindow
 * @brief This class describes a time window to calculate the direct hits which
 *        fall inside this time window.
 *        NOTE: This class should be in principle a general I3TimeWindow class,
 *              but at the time this project has been written no such class did
 *              exist.
 */
class I3DirectHitsTimeWindow
{
  public:
    //__________________________________________________________________________
    I3DirectHitsTimeWindow();

    //__________________________________________________________________________
    I3DirectHitsTimeWindow(double time0, double time1);

    //__________________________________________________________________________
    I3DirectHitsTimeWindow(const I3DirectHitsTimeWindow& tw);

    //__________________________________________________________________________
    inline
    double
    GetTime0() const
    { return this->time0_; }

    //__________________________________________________________________________
    inline
    double
    GetTime1() const
    { return this->time1_; }

    //__________________________________________________________________________
    inline
    void
    SetTime0(double time)
    { this->time0_ = time; }

    //__________________________________________________________________________
    inline
    void
    SetTime1(double time)
    { this->time1_ = time; }

    //__________________________________________________________________________
    inline
    bool
    IsTimeAfterTimeWindow(double time) const
    {
        return (time > this->time1_);
    }

    //__________________________________________________________________________
    inline
    bool
    IsTimeBeforeTimeWindow(double time) const
    {
        return (time < this->time0_);
    }

    //__________________________________________________________________________
    inline
    bool
    IsTimeInsideTimeWindow(double time) const
    {
        return ((time >= this->time0_) && (time <= this->time1_));
    }

    //__________________________________________________________________________
    bool
    operator==(const I3DirectHitsTimeWindow& rhs) const;

  protected:
    double time0_;
    double time1_;

  private:
    SET_LOGGER("I3DirectHitsTimeWindow")
};

//______________________________________________________________________________
std::ostream&
operator<<(std::ostream& oss, const I3DirectHitsTimeWindow& rhs);

//______________________________________________________________________________
I3_POINTER_TYPEDEFS(I3DirectHitsTimeWindow);

//==============================================================================
#endif // I3DIRECTHITSTIMEWINDOW_H_INCLUDED
