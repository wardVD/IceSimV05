/**
 * $Id$
 *
 * Copyright (C) 2012
 * Martin Wolf <martin.wolf@icecube.wisc.edu>
 * and the IceCube Collaboration <http://www.icecube.wisc.edu>
 *
 * @file hit_statistics/calculator.h
 * @version $Revision$
 * @date $Date$
 * @author Martin Wolf <martin.wolf@icecube.wisc.edu>
 * @brief This file contains the definition of the calculator functions that are
 *        defined inside the common_variables::hit_statistics namespace.
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
#ifndef COMMONVARIABLES_HITSTATISTICS_CALCULATOR_H_INCLUDED
#define COMMONVARIABLES_HITSTATISTICS_CALCULATOR_H_INCLUDED

#include "dataclasses/I3Double.h"
#include "dataclasses/I3Position.h"
#include "dataclasses/geometry/I3Geometry.h"
#include "dataclasses/physics/I3RecoPulse.h"

#include "recclasses/I3HitStatisticsValues.h"

//##############################################################################
namespace common_variables { namespace hit_statistics {

//______________________________________________________________________________
/** Calculates the hit statistics, e.g. COG, PulseTimespan, QMax, QTot, ZMean,
 *  ZSigma, and ZTravel for the given I3Geometry, and the given
 *  I3RecoPulseSeriesMap.
 */
I3HitStatisticsValuesPtr
CalculateHitStatistics(
    const I3Geometry&           geometry,
    const I3RecoPulseSeriesMap& pulsesMap
);

//______________________________________________________________________________
/** Calculates the COG variable for the given I3Geometry, and the given
 *  I3RecoPulseSeriesMap.
 */
I3PositionPtr
CalculateCOG(
    const I3Geometry&           geometry,
    const I3RecoPulseSeriesMap& pulsesMap
);

//______________________________________________________________________________
/** Calculates the sigma value of the z-component of the COG position for the
 *  given I3Geometry, and the given I3RecoPulseSeriesMap.
 */
I3DoublePtr
CalculateCOGZSigma(
    const I3Geometry&           geometry,
    const I3RecoPulseSeriesMap& pulsesMap
);

//______________________________________________________________________________
/** Calculates the minimal time of all pulses, e.g. the time of the first pulse,
 *  for the given I3Geometry, and the given I3RecoPulseSeriesMap.
 */
I3DoublePtr
CalculateMinPulseTime(
    const I3Geometry&           geometry,
    const I3RecoPulseSeriesMap& pulsesMap
);

//______________________________________________________________________________
/** Calculates the maximal time of all pulses, e.g. the time of the last pulse,
 *  for the given I3Geometry, and the given I3RecoPulseSeriesMap.
 */
I3DoublePtr
CalculateMaxPulseTime(
    const I3Geometry&           geometry,
    const I3RecoPulseSeriesMap& pulsesMap
);

//______________________________________________________________________________
/** Calculates the QMaxDoms variable for the given I3Geometry, and the given
 *  I3RecoPulseSeriesMap.
 */
I3DoublePtr
CalculateQMaxDoms(
    const I3Geometry&           geometry,
    const I3RecoPulseSeriesMap& pulsesMap
);

//______________________________________________________________________________
/** Calculates the QTot variable for the given I3Geometry, and the given
 *  I3RecoPulseSeriesMap.
 */
I3DoublePtr
CalculateQTotPulses(
    const I3Geometry&           geometry,
    const I3RecoPulseSeriesMap& pulsesMap
);

//______________________________________________________________________________
/** Calculates the minimal OM_Z value of all hits.
 */
I3DoublePtr
CalculateZMin(
    const I3Geometry&           geometry,
    const I3RecoPulseSeriesMap& pulsesMap
);

//______________________________________________________________________________
/** Calculates the maximal OM_Z value of all hits.
 */
I3DoublePtr
CalculateZMax(
    const I3Geometry&           geometry,
    const I3RecoPulseSeriesMap& pulsesMap
);

//______________________________________________________________________________
/** Calculates the mean value of OM_Z.
 */
I3DoublePtr
CalculateZMean(
    const I3Geometry&           geometry,
    const I3RecoPulseSeriesMap& pulsesMap
);

//______________________________________________________________________________
/** Calculates the sigma (RMSD) value of the OM_Z values for the given
 *  I3Geometry, and the given I3RecoPulseSeriesMap.
 *
 *  Note: The mean of the OM_Z values will be calculated using the
 *        CalculateZMean utility function.
 */
I3DoublePtr
CalculateZSigma(
    const I3Geometry&           geometry,
    const I3RecoPulseSeriesMap& pulsesMap
);

//______________________________________________________________________________
/** Calculates the ZTravel variable for the given I3Geometry, and the given
 *  I3RecoPulseSeriesMap.
 */
I3DoublePtr
CalculateZTravel(
    const I3Geometry&           geometry,
    const I3RecoPulseSeriesMap& pulsesMap
);

}/*hit_statistics*/}/*common_variables*/
//##############################################################################

#endif // COMMONVARIABLES_HITSTATISTICS_CALCULATOR_H_INCLUDED
