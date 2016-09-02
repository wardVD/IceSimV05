/**
 * $Id$
 *
 * Copyright (C) 2012
 * Martin Wolf <martin.wolf@icecube.wisc.edu>
 * and the IceCube Collaboration <http://www.icecube.wisc.edu>
 *
 * @file direct_hits/calculator.h
 * @version $Revision$
 * @date $Date$
 * @author Martin Wolf <martin.wolf@icecube.wisc.edu>
 * @brief This file contains the definition of the calculator functions that are
 *        defined inside the common_variables::direct_hits namespace.
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
#ifndef COMMONVARIABLES_DIRECTHITS_CALCULATOR_H_INCLUDED
#define COMMONVARIABLES_DIRECTHITS_CALCULATOR_H_INCLUDED

#include "dataclasses/I3Vector.h"
#include "dataclasses/geometry/I3Geometry.h"
#include "dataclasses/physics/I3Particle.h"
#include "dataclasses/physics/I3RecoPulse.h"

#include "CommonVariables/direct_hits/I3DirectHitsDefinition.h"
#include "recclasses/I3DirectHitsValues.h"

//##############################################################################
namespace common_variables { namespace direct_hits {

//______________________________________________________________________________
/** Gets an I3DirectHitsDefinitionSeries object with the default direct hits
 *  definitions (e.g. time windows) A, B, C, D:
 *
 *  - time window "A": [-15ns; + 15ns]
 *  - time window "B": [-15ns; + 25ns]
 *  - time window "C": [-15ns; + 75ns]
 *  - time window "D": [-15ns; +125ns]
 */
I3DirectHitsDefinitionSeries
GetDefaultDefinitions();

//______________________________________________________________________________
/** Calculates the direct hits for the given time windows (given
 *  through the I3DirectHitsDefinitionSeries object, the given
 *  I3Geometry, the given I3RecoPulseSeriesMap and the given
 *  I3Particle.
 */
I3DirectHitsValuesMapPtr
CalculateDirectHits(
    const I3DirectHitsDefinitionSeries& dhDefinitions,
    const I3Geometry&                   geometry,
    const I3RecoPulseSeriesMap&         pulsesMap,
    const I3Particle&                   particle
);

//______________________________________________________________________________
/** Calculates the direct hits for the default direct hits time windows
 *  (defined through the GetDefaultDefinitions() function), the given
 *  I3Geometry, the given I3RecoPulseSeriesMap and the given
 *  I3Particle.
 */
I3DirectHitsValuesMapPtr
CalculateDirectHits(
    const I3Geometry&           geometry,
    const I3RecoPulseSeriesMap& pulsesMap,
    const I3Particle&           particle
)
{
    return CalculateDirectHits(
        GetDefaultDefinitions(),
        geometry,
        pulsesMap,
        particle
    );
}

//______________________________________________________________________________
/** Returns an I3RecoPulseSeriesMapPtr with the direct hit DOMs for a given
 *  direct hits definition, e.g. direct hits time window, for the given
 *  I3Geometry, the given I3RecoPulseSeriesMap, and the given I3Particle.
 *
 *  If the *allPulsesOfDirectDoms* option (default is ``false``) is set to
 *  ``true``, the I3RecoPulseSeries object of each direct hit DOM will contain
 *  all pulses of the original I3RecoPulseSeries of that DOM. Otherwise only the
 *  direct pulses will be included.
 */
I3RecoPulseSeriesMapPtr
GetDirectHitsPulseMap(
    const I3DirectHitsDefinition dhDefinition,
    const I3Geometry&            geometry,
    const I3RecoPulseSeriesMap&  pulsesMap,
    const I3Particle&            particle,
    bool                         allPulsesOfDirectDoms = false
);

//______________________________________________________________________________
/** Returns an I3VectorDoublePtr containing the time residuals of all direct
 *  pulses for a given direct hits definition, e.g. direct hits time window, for
 *  the given I3Geometry, the given I3RecoPulseSeriesMap, and the given
 *  I3Particle.
 */
I3VectorDoublePtr
GetDirectPulsesTimeResiduals(
    const I3DirectHitsDefinition dhDefinition,
    const I3Geometry&            geometry,
    const I3RecoPulseSeriesMap&  pulsesMap,
    const I3Particle&            particle
);

}/*direct_hits*/}/*common_variables*/
//##############################################################################

#endif // COMMONVARIABLES_DIRECTHITS_CALCULATOR_H_INCLUDED
