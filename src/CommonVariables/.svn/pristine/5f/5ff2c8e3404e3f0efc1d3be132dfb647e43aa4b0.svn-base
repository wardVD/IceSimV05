/**
 * $Id$
 *
 * Copyright (C) 2012
 * Martin Wolf <martin.wolf@icecube.wisc.edu>
 * and the IceCube Collaboration <http://www.icecube.wisc.edu>
 *
 * @file I3DirectHitsDefinition.h
 * @version $Revision$
 * @date $Date$
 * @author Martin Wolf <martin.wolf@icecube.wisc.edu>
 * @brief This file contains the definition of the I3DirectHitsDefinition
 *        class, describing a particular class of direct hits.
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
#ifndef I3DIRECTHITSDEFINITION_H_INCLUDED
#define I3DIRECTHITSDEFINITION_H_INCLUDED

#include <string>
#include <vector>

#include "icetray/I3Logging.h"
#include "icetray/I3PointerTypedefs.h"

#include "CommonVariables/direct_hits/I3DirectHitsTimeWindow.h"

//==============================================================================
/**
 * @class I3DirectHitsDefinition
 * @brief This class describes a definition of a class of direct hits.
 */
class I3DirectHitsDefinition
{
  public:
    //__________________________________________________________________________
    I3DirectHitsDefinition();

    //__________________________________________________________________________
    I3DirectHitsDefinition(std::string name, double time0, double time1);

    //__________________________________________________________________________
    /// copy constructor
    I3DirectHitsDefinition(const I3DirectHitsDefinition& dhd);

    //__________________________________________________________________________
    virtual ~I3DirectHitsDefinition() {}

    //__________________________________________________________________________
    inline
    std::string
    GetName() const
    { return this->name_; }

    //__________________________________________________________________________
    inline
    const I3DirectHitsTimeWindow&
    GetTimeWindow() const
    { return this->timeWindow_; }

    //__________________________________________________________________________
    inline
    I3DirectHitsTimeWindow&
    GetTimeWindow()
    { return this->timeWindow_; }

    //__________________________________________________________________________
    inline
    void
    SetName(std::string name)
    { this->name_ = name; }

    //__________________________________________________________________________
    inline
    void
    SetTimeWindow(const I3DirectHitsTimeWindow& timeWindow)
    { this->timeWindow_ = timeWindow; }

    //__________________________________________________________________________
    bool operator==(const I3DirectHitsDefinition& rhs) const;

  protected:
    /// the name of the direct hits class
    std::string name_;

    /// the time window definition of this direct hits class
    I3DirectHitsTimeWindow timeWindow_;

  private:
    SET_LOGGER("I3DirectHitsDefinition")
};

//______________________________________________________________________________
std::ostream&
operator<<(std::ostream& oss, const I3DirectHitsDefinition& d);

//______________________________________________________________________________
I3_POINTER_TYPEDEFS(I3DirectHitsDefinition);

//==============================================================================
// define series types
typedef std::vector<I3DirectHitsDefinition> I3DirectHitsDefinitionSeries;
I3_POINTER_TYPEDEFS(I3DirectHitsDefinitionSeries);

#endif //I3DIRECTHITSDEFINITION_H_INCLUDED
