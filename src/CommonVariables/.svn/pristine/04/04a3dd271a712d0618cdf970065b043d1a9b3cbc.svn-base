/**
 * $Id$
 *
 * Copyright (C) 2012
 * Martin Wolf <martin.wolf@icecube.wisc.edu>
 * and the IceCube Collaboration <http://www.icecube.wisc.edu>
 *
 * @file track_characteristics/TrackHitsSeparationLengthIterativeCalculator.h
 * @version $Revision$
 * @date $Date$
 * @author Martin Wolf <martin.wolf@icecube.wisc.edu>
 * @brief This file contains the definition and implementation of the iterative
 *        calculator for the TrackHitsSeparationLength variable. The
 *        TrackHitsSeparationLengthIterativeCalculator has been introduced in
 *        order to have only one place for the implementation of
 *        the TrackHitsSeparationLength variable.
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
#ifndef COMMONVARIABLES_TRACKCHARACTERISTICS_TRACKHITSSEPARATIONLENGTHITERATIVECALCULATOR_H_INCLUDED
#define COMMONVARIABLES_TRACKCHARACTERISTICS_TRACKHITSSEPARATIONLENGTHITERATIVECALCULATOR_H_INCLUDED

#include <algorithm>
#include <cmath>
#include <vector>

#include "dataclasses/physics/I3Particle.h"

#include "phys-services/I3Calculator.h"

#include "CommonVariables/hit_statistics/COGIterativeCalculator.h"

//##############################################################################
namespace common_variables { namespace track_characteristics { namespace details {

//==============================================================================
struct TrackHitsSeparationLengthIterativeCalculator
{
    I3Particle particle_;

    std::vector<double> omXs_;
    std::vector<double> omYs_;
    std::vector<double> omZs_;
    std::vector<double> hitTimes_;
    std::vector<double> hitCharges_;

    double trackHitsSeparationLength_;

    //__________________________________________________________________________
    TrackHitsSeparationLengthIterativeCalculator(const I3Particle& particle)
    {
        Reset(particle);
    }

    //__________________________________________________________________________
    /** Returns the calculated TrackHitsSeparationLength variable as a double
     *  value. The result of this method is only valid after the call of the
     *  FinalizeCalculation method, but this condition is not enforced.
     */
    inline
    double
    AsDouble()
    {
        return trackHitsSeparationLength_;
    }

    //__________________________________________________________________________
    /** Does the next iteration step. This method is supposed to get called
     *  within the I3RecoPulseSeriesMap loop for a particular DOM. Usually the
     *  hitTime is the time of the first pulse, and hitCharge is the integrated
     *  charge of the OM.
     */
    inline
    void
    DoNextIteration(
        double omX,
        double omY,
        double omZ,
        double hitTime,
        double hitCharge
    )
    {
        omXs_.push_back(omX);
        omYs_.push_back(omY);
        omZs_.push_back(omZ);
        hitTimes_.push_back(hitTime);
        hitCharges_.push_back(hitCharge);
    }

    //__________________________________________________________________________
    /** Finalizes the TrackHitsSeparationLength calculation. This method is
     *  supposed to get called after the I3RecoPulseSeriesMap loop.
     */
    inline
    void
    FinalizeCalculation()
    {
        uint64_t nHits = hitTimes_.size();

        // The TrackHitsSeparationLength is only defined for at least 4 hits.
        if(nHits < 4)
            return;

        // Calculate maxHitTimeOfFirstQuartile and minHitTimeOfLastQuartile,
        // where maxHitTimeOfFirstQuartile is the time of the first hit that
        // does NOT belong to the first quartile of the hits, and where
        // minHitTimeOfLastQuartile is the time of the first hit that DOES
        // belong to the last quartile of the hits.
        uint64_t nHitsInQuartile = nHits/4;
        std::vector<double> sortedHitTimes(hitTimes_);
        std::sort(sortedHitTimes.begin(), sortedHitTimes.end());
        double maxHitTimeOfFirstQuartile = sortedHitTimes[nHitsInQuartile];
        double minHitTimeOfLastQuartile  = sortedHitTimes[nHits - nHitsInQuartile];

        common_variables::hit_statistics::details::COGIterativeCalculator cogFirstQuartileIterCalc;
        common_variables::hit_statistics::details::COGIterativeCalculator cogLastQuartileIterCalc;

        // Iterate over the stored vectors of omXs_, omYs_, omZs_, hitTimes_,
        // and hitCharges_ to calculate the COG of the first and the last
        // quartile of the hits in time.
        for (uint64_t i=0; i<nHits; ++i)
        {
            if(hitTimes_[i] < maxHitTimeOfFirstQuartile)
                // The hit is one of the first quartile.
                cogFirstQuartileIterCalc.DoNextIteration(omXs_[i], omYs_[i], omZs_[i], hitCharges_[i]);
            else if(hitTimes_[i] >= minHitTimeOfLastQuartile)
                // The hit is one of the last quartile.
                cogLastQuartileIterCalc.DoNextIteration(omXs_[i], omYs_[i], omZs_[i], hitCharges_[i]);
        }
        cogFirstQuartileIterCalc.FinalizeCalculation();
        cogLastQuartileIterCalc.FinalizeCalculation();

        double firstQuartileDistAlongTrack = I3Calculator::DistanceAlongTrack(particle_, cogFirstQuartileIterCalc.AsI3Position());
        double lastQuartileDistAlongTrack  = I3Calculator::DistanceAlongTrack(particle_, cogLastQuartileIterCalc.AsI3Position());

        trackHitsSeparationLength_ = lastQuartileDistAlongTrack -
                                     firstQuartileDistAlongTrack;
    }

    //__________________________________________________________________________
    /** Resets the TrackHitsSeparationLengthIterativeCalculator, in order to be
     *  able to use the same TrackHitsSeparationLengthIterativeCalculator object
     *  more than once.
     */
    inline
    void
    Reset(const I3Particle& particle)
    {
        particle_ = particle;

        omXs_.clear();
        omYs_.clear();
        omZs_.clear();
        hitTimes_.clear();
        hitCharges_.clear();

        trackHitsSeparationLength_ = NAN;
    }
};
//==============================================================================

}/*details*/}/*track_characteristics*/}/*common_variables*/
//##############################################################################

#endif // COMMONVARIABLES_TRACKCHARACTERISTICS_TRACKHITSSEPARATIONLENGTHITERATIVECALCULATOR_H_INCLUDED
