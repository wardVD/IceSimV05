/**
 * $Id$
 *
 * Copyright (C) 2012
 * Martin Wolf <martin.wolf@icecube.wisc.edu>
 * and the IceCube Collaboration <http://www.icecube.wisc.edu>
 *
 * @file track_characteristics/calculator.cxx
 * @version $Revision$
 * @date $Date$
 * @author Martin Wolf <martin.wolf@icecube.wisc.edu>
 * @brief This file contains the implementation of the utility functions defined
 *        in the common_variables::track_characteristics namespace.
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
#include <cmath>
#include <vector>

#include "phys-services/I3Calculator.h"

#include "CommonVariables/WeightedMeanIterativeCalculator.h"
#include "CommonVariables/track_characteristics/TrackHitsSeparationLengthIterativeCalculator.h"

#include "CommonVariables/track_characteristics/calculator.h"

//##############################################################################
namespace common_variables { namespace track_characteristics {

namespace details {

//______________________________________________________________________________
double
CalculateEmptyHitsTrackLength(
    const std::vector<double>& sortedHitDistancesAlongTrack
)
{
    // The EmptyHitsTrackLength variable is only defined for at least 2 hits.
    if(sortedHitDistancesAlongTrack.size() < 2)
        return NAN;

    uint32_t n = sortedHitDistancesAlongTrack.size() - 1;

    double emptyHitsTrackLength = NAN;
    for(uint32_t i=0; i<n; ++i)
    {
        double current_emptyHitsTrackLength =
            sortedHitDistancesAlongTrack[i+1] -
            sortedHitDistancesAlongTrack[i];
        if(std::isnan(emptyHitsTrackLength) ||
           emptyHitsTrackLength < current_emptyHitsTrackLength
          )
            emptyHitsTrackLength = current_emptyHitsTrackLength;
    }

    return emptyHitsTrackLength;
}

//______________________________________________________________________________
double
CalculateTrackHitsDistributionSmoothness(
    const std::vector<double>& sortedHitDistancesAlongTrack
)
{
    // The smoothness is only defined for at least 3 hits.
    if(sortedHitDistancesAlongTrack.size() < 3)
        return NAN;

    uint32_t n = sortedHitDistancesAlongTrack.size() - 1;

    double trackHitsDistributionSmoothness = 0.;
    for(uint32_t i=1; i<n; ++i)
    {
        double li = sortedHitDistancesAlongTrack[i] - sortedHitDistancesAlongTrack[0];
        double ln = sortedHitDistancesAlongTrack[n] - sortedHitDistancesAlongTrack[0];
        double s = (double)i/(double)n - li/ln;
        log_trace("common_variables::track_characteristics::details::CalculateTrackHitsDistributionSmoothness: "
                  "hit index: %u, n: %u, s: %f", i, n, s
        );
        if(fabs(s) > fabs(trackHitsDistributionSmoothness))
            trackHitsDistributionSmoothness = s;
    }

    return trackHitsDistributionSmoothness;
}

}/*details*/
//##############################################################################

//______________________________________________________________________________
I3TrackCharacteristicsValuesPtr
CalculateTrackCharacteristics(
    const I3Geometry&           geometry,
    const I3RecoPulseSeriesMap& pulsesMap,
    const I3Particle&           particle,
    double                      trackCylinderRadius
)
{
    std::vector<double> hitDistancesAlongTrack;
    hitDistancesAlongTrack.reserve(pulsesMap.size());

    details::TrackHitsSeparationLengthIterativeCalculator trackHitsSeparationLengthIterCalc(
        particle
    );
    common_variables::details::WeightedMeanIterativeCalculator avgDomDistQTotDom;

    // Iterate over every hit DOM.
    for(I3RecoPulseSeriesMap::const_iterator pulsesMapCIter = pulsesMap.begin();
        pulsesMapCIter != pulsesMap.end();
        ++pulsesMapCIter
       )
    {
        const OMKey&             omKey  = pulsesMapCIter->first;
        const I3RecoPulseSeries& pulses = pulsesMapCIter->second;

        // Check if the hit DOM matches with the current geometry, and get the
        // position of the DOM.
        I3OMGeoMap::const_iterator geoMapCIter = geometry.omgeo.find(omKey);
        if(geoMapCIter == geometry.omgeo.end())
        {
            log_fatal(
                "common_variables::track_characteristics::CalculateTrackCharacteristics: "
                "Didn't find the current OMKey %s in the Geometry! "
                "Did you use the wrong Geometry?",
                omKey.str().c_str()
            );
        }
        const I3Position& omPos = geoMapCIter->second.position;

        // Sanity check: Check if there are pulses at all.
        if(pulses.size() == 0)
        {
            log_error(
                "common_variables::track_characteristics::CalculateTrackCharacteristics: "
                "The OMKey %s does not have pulses at all! "
                "Skipping this DOM!",
                omKey.str().c_str()
            );
            continue;
        }

        // Check if the DOM is inside the track cylinder.
        double domCadToTrack = I3Calculator::ClosestApproachDistance(particle, omPos);
        if(std::isnan(domCadToTrack) || domCadToTrack > trackCylinderRadius)
            continue;

        // Record the distance of this hit along the track.
        double hitDistAlongTrack = I3Calculator::DistanceAlongTrack(particle, omPos);
        hitDistancesAlongTrack.push_back(hitDistAlongTrack);

        double qTotDom = 0.;

        // Iterate over the pulses of the DOM's pulse series.
        for(I3RecoPulseSeries::const_iterator pulsesCIter = pulses.begin();
            pulsesCIter != pulses.end();
            ++pulsesCIter
           )
        {
            const I3RecoPulse& pulse = *pulsesCIter;

            // Get the charge of the pulse and count it for the total charge of
            // the OM, if the charge has a physical value.
            double pulseCharge = pulse.GetCharge();
            if(! (std::isnan(pulseCharge) ||
                  std::isinf(pulseCharge)
                 )
              )
            {
                qTotDom += pulseCharge;
            }
        }//END FOR I3RecoPulseSeries

        trackHitsSeparationLengthIterCalc.DoNextIteration(
            omPos.GetX(),
            omPos.GetY(),
            omPos.GetZ(),
            pulses[0].GetTime(),
            qTotDom);

        avgDomDistQTotDom.DoNextIteration(domCadToTrack, qTotDom);
    }//END FOR I3RecoPulseSeriesMap

    //--------------------------------------------------------------------------
    // Finalize the calculation of EmptyHitsTrackLength and
    // TrackHitsDistributionSmoothness using the collected hit distances along
    // the track.
    sort(hitDistancesAlongTrack.begin(), hitDistancesAlongTrack.end());
    double emptyHitsTrackLength = details::CalculateEmptyHitsTrackLength(hitDistancesAlongTrack);
    double trackHitsDistributionSmoothness = details::CalculateTrackHitsDistributionSmoothness(hitDistancesAlongTrack);

    //--------------------------------------------------------------------------
    // Finalize the calculation of the TrackHitsSeparationLength variable.
    trackHitsSeparationLengthIterCalc.FinalizeCalculation();

    //--------------------------------------------------------------------------
    // Finalize the calculation of the AvgDomDistQ variable.
    avgDomDistQTotDom.FinalizeCalculation();

    //--------------------------------------------------------------------------
    // Construct the I3TrackCharacteristicsValues frame object with all
    // calculated values.
    I3TrackCharacteristicsValuesPtr i3TrackCharacteristicsValuesPtr(new I3TrackCharacteristicsValues(
        avgDomDistQTotDom.AsDouble(),
        emptyHitsTrackLength,
        trackHitsSeparationLengthIterCalc.AsDouble(),
        trackHitsDistributionSmoothness
    ));
    return i3TrackCharacteristicsValuesPtr;
}

//______________________________________________________________________________
}/*track_characteristics*/}/*common_variables*/
//##############################################################################
