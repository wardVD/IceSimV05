/**
 * $Id$
 *
 * Copyright (C) 2012
 * Martin Wolf <martin.wolf@icecube.wisc.edu>
 * and the IceCube Collaboration <http://www.icecube.wisc.edu>
 *
 * @file track_characteristics/calculator.h
 * @version $Revision$
 * @date $Date$
 * @author Martin Wolf <martin.wolf@icecube.wisc.edu>
 * @brief This file contains the definition of the utility functions defined in
 *        the common_variables::track_characteristics namespace.
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
#ifndef COMMONVARIABLES_TRACKCHARACTERISTICS_CALCULATOR_H_INCLUDED
#define COMMONVARIABLES_TRACKCHARACTERISTICS_CALCULATOR_H_INCLUDED

#include <vector>

#include "dataclasses/geometry/I3Geometry.h"
#include "dataclasses/physics/I3Particle.h"
#include "dataclasses/physics/I3RecoPulse.h"

#include "recclasses/I3TrackCharacteristicsValues.h"

//##############################################################################
namespace common_variables { namespace track_characteristics {

namespace details {

//______________________________________________________________________________
/** This is the core implementation of the calculation of the
 *  EmptyHitsTrackLength variable, done by the CalculateTrackCharacteristics
 *  function.
 *
 *  NOTE: The vector of hit distances along the track must be sorted by its
 *        values from lower to higher values!
 *
 *  Returns NAN if there are less then 2 hits.
 */
double
CalculateEmptyHitsTrackLength(
    const std::vector<double>& sortedHitDistancesAlongTrack
);

//______________________________________________________________________________
/** This is the core implementation of the calculation of the
 *  TrackHitsDistributionSmoothness variable, done by the
 *  CalculateTrackCharacteristics function.
 *
 *  NOTE: The vector of hit distances along the track must be sorted by its
 *        values from lower to higher values!
 *
 *  Returns NAN if there are less then 3 hits.
 */
double
CalculateTrackHitsDistributionSmoothness(
    const std::vector<double>& sortedHitDistancesAlongTrack
);

}/*details*/
//##############################################################################

//______________________________________________________________________________
/** Calculates the track characteristics values, e.g. TrackPulseTimespan,
 *  HitsTrackLength, EmptyHitsTrackLength, TrackHitsSeperationLength,
 *  and TrackHitsDistributionSmoothness, for the given I3Geometry, the
 *  given I3RecoPulseSeriesMap, and the given track, given through the
 *  I3Particle object, for hits within the given cylinder radius around the
 *  track.
 */
I3TrackCharacteristicsValuesPtr
CalculateTrackCharacteristics(
    const I3Geometry&           geometry,
    const I3RecoPulseSeriesMap& pulsesMap,
    const I3Particle&           particle,
    double                      trackCylinderRadius
);

}/*track_characteristics*/}/*common_variables*/
//##############################################################################

#endif // COMMONVARIABLES_TRACKCHARACTERISTICS_CALCULATOR_H_INCLUDED
