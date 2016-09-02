/**
 * @author Anna Pollmann <anna.pollmann@uni-wuppertal.de>
 */


#ifndef COMMONVARIABLES_TIMECHARACTERISTICS_CALCULATOR_H_INCLUDED
#define COMMONVARIABLES_TIMECHARACTERISTICS_CALCULATOR_H_INCLUDED

#include <vector>

#include "dataclasses/geometry/I3Geometry.h"
#include "dataclasses/physics/I3Particle.h"
#include "dataclasses/physics/I3RecoPulse.h"

#include "recclasses/I3TimeCharacteristicsValues.h"

//##############################################################################
namespace common_variables { namespace time_characteristics {

namespace details {

//______________________________________________________________________________
/** This is the core implementation of the calculation of the
 *  EmptyHitsTrackLength variable, done by the CalculateTiming
 *  function.
 *
 *  NOTE: The vector of hit distances along the track must be sorted by its
 *        values from lower to higher values!
 *
 *  Returns NAN if there are less then 2 hits.
 */

 bool sortTimes(double i, double j);

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
I3TimeCharacteristicsValuesPtr
CalculateTimeCharacteristics(
    const I3Geometry&           geometry,
    const I3RecoPulseSeriesMap& pulsesMap
);

}/*time_characteristics*/}/*common_variables*/
//##############################################################################

#endif // COMMONVARIABLES_TIMECHARACTERISTICS_CALCULATOR_H_INCLUDED