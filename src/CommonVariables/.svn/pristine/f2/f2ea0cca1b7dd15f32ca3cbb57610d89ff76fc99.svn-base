/**
 * $Id$
 *
 * Copyright (C) 2012
 * Martin Wolf <martin.wolf@icecube.wisc.edu>
 * and the IceCube Collaboration <http://www.icecube.wisc.edu>
 *
 * @file hit_multiplicity/calculator.cxx
 * @version $Revision$
 * @date $Date$
 * @author Martin Wolf <martin.wolf@icecube.wisc.edu>
 * @brief This file contains the implementation of the calculator functions that
 *        are defined inside the common_variables::hit_multiplicity
 *        namespace.
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
#include "CommonVariables/hit_multiplicity/calculator.h"

#include "CommonVariables/hit_multiplicity/NHitStringsIterativeCalculator.h"

//##############################################################################
namespace common_variables { namespace hit_multiplicity {

//______________________________________________________________________________
I3HitMultiplicityValuesPtr
CalculateHitMultiplicity(
    const I3Geometry&           geometry,
    const I3RecoPulseSeriesMap& pulsesMap
)
{
    details::NHitStringsIterativeCalculator nHitStringsIterCalc;

    uint32_t nHitDoms         = 0;
    uint32_t nHitDomsOnePulse = 0;
    uint64_t nPulses          = 0;

    // Iterate over every hit DOM.
    for(I3RecoPulseSeriesMap::const_iterator pulsesMapCIter = pulsesMap.begin();
        pulsesMapCIter != pulsesMap.end();
        ++pulsesMapCIter
       )
    {
        const OMKey&             omKey  = pulsesMapCIter->first;
        const I3RecoPulseSeries& pulses = pulsesMapCIter->second;

        // Check if the hit DOM matches with the current geometry.
        I3OMGeoMap::const_iterator geoMapCIter = geometry.omgeo.find(omKey);
        if(geoMapCIter == geometry.omgeo.end())
        {
            log_fatal(
                "common_variables::hit_multiplicity::CalculateHitMultiplicity: "
                "Didn't find the current OMKey %s in the Geometry! "
                "Did you use the wrong Geometry?",
                omKey.str().c_str()
            );
        }

        // Sanity check: Check if there are pulses at all.
        if(pulses.size() == 0)
        {
            log_error(
                "common_variables::hit_multiplicity::CalculateHitMultiplicity: "
                "The OMKey %s does not have pulses at all! "
                "Skipping this DOM!",
                omKey.str().c_str()
            );
            continue;
        }

        // Count NHitDoms (formerly NChannel).
        ++nHitDoms;

        nHitStringsIterCalc.DoNextIteration(omKey.GetString());

        size_t nPulses_ThisDom(pulses.size());

        // Count the number of DOMs with exactly one pulse.
        if(nPulses_ThisDom == 1)
            ++nHitDomsOnePulse;

        nPulses += nPulses_ThisDom;

    }//END FOR I3RecoPulseSeriesMap

    //--------------------------------------------------------------------------
    // Finalize the calculation of NStrings.
    nHitStringsIterCalc.FinalizeCalculation();

    //--------------------------------------------------------------------------
    // Construct the I3HitMultiplicityValues output I3FrameObject.
    I3HitMultiplicityValuesPtr hmValuesPtr(new I3HitMultiplicityValues(
        nHitStringsIterCalc.AsUInt32(),
        nHitDoms,
        nHitDomsOnePulse,
        nPulses
    ));

    return hmValuesPtr;
}

}/*hit_multiplicity*/}/*common_variables*/
//##############################################################################
