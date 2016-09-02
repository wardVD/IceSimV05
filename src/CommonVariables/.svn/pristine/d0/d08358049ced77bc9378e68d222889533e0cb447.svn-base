/**
 * $Id$
 *
 * Copyright (C) 2012
 * Martin Wolf <martin.wolf@icecube.wisc.edu>
 * and the IceCube Collaboration <http://www.icecube.wisc.edu>
 *
 * @file hit_multiplicity/calculator.h
 * @version $Revision$
 * @date $Date$
 * @author Martin Wolf <martin.wolf@icecube.wisc.edu>
 * @brief This file contains the definition of the calculator functions that are
 *        defined inside the common_variables::hit_multiplicity namespace.
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
#ifndef COMMONVARIABLES_HITMULTIPLICITY_CALCULATOR_H_INCLUDED
#define COMMONVARIABLES_HITMULTIPLICITY_CALCULATOR_H_INCLUDED

#include "dataclasses/geometry/I3Geometry.h"
#include "dataclasses/physics/I3RecoPulse.h"

#include "recclasses/I3HitMultiplicityValues.h"

//##############################################################################
namespace common_variables { namespace hit_multiplicity {

//______________________________________________________________________________
/** Calculates the hit multiplicity, e.g. NString, NChannel, NHit, and NOneHit
 *  for the given I3Geometry, and the given I3RecoPulseSeriesMap.
 *
 *  If the "considerOnlyFirstPulse" argument is set to "true"
 *  (the default value), only the first pulse in each of the
 *  I3RecoPulseSeries will be used for the calculation.
 */
I3HitMultiplicityValuesPtr
CalculateHitMultiplicity(
    const I3Geometry&           geometry,
    const I3RecoPulseSeriesMap& pulsesMap
);

}/*hit_multiplicity*/}/*common_variables*/
//##############################################################################

#endif // COMMONVARIABLES_HITMULTIPLICITY_CALCULATOR_H_INCLUDED
