/**
 * $Id$
 *
 * Copyright (C) 2012
 * Martin Wolf <martin.wolf@icecube.wisc.edu>
 * and the IceCube Collaboration <http://www.icecube.wisc.edu>
 *
 * @file hit_statistics/ZTravelIterativeCalculator.h
 * @version $Revision$
 * @date $Date$
 * @author Martin Wolf <martin.wolf@icecube.wisc.edu>
 * @brief This file contains the definition and implementation of the iterative
 *        calculator for the ZTravel cut variable. The
 *        ZTravelIterativeCalculator has been introduced in order to have only
 *        one compact place for the implementation of the ZTravel variable.
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
#ifndef COMMONVARIABLES_HITSTATISTICS_ZTRAVELITERATIVECALCULATOR_H_INCLUDED
#define COMMONVARIABLES_HITSTATISTICS_ZTRAVELITERATIVECALCULATOR_H_INCLUDED

#include <algorithm>
#include <cmath>

//##############################################################################
namespace common_variables { namespace hit_statistics { namespace details {

//==============================================================================
struct ZTravelIterativeCalculator
{
    //__________________________________________________________________________
    /** A structure for sorting the pulses after time.
     */
    struct ZTravelPulse_t {
        double pulseTime;
        double omZ;

        ZTravelPulse_t(double pulseTime_, double omZ_)
          : pulseTime(pulseTime_),
            omZ(omZ_)
        {}
    };

    //__________________________________________________________________________
    /** Compares two ZTravelPulse_t objects after the time of the pulse.
     */
    static
    bool
    compare_ZTravelPulse_pulseTime(ZTravelPulse_t lhs, ZTravelPulse_t rhs)
    {
        return (lhs.pulseTime < rhs.pulseTime);
    }

    //__________________________________________________________________________
    std::vector<ZTravelPulse_t> zTravelPulseVec_;
    double zTravel_;

    //__________________________________________________________________________
    ZTravelIterativeCalculator()
    {
        Reset();
    }

    //__________________________________________________________________________
    /** Returns the calculated ZTravel variable as a double value. The result of
     *  this method is only valid after the call of the FinalizeCalculation
     *  method, but this condition is not enforced.
     */
    inline
    double
    AsDouble()
    {
        return zTravel_;
    }

    //__________________________________________________________________________
    /** Does the next iteration step. This method is supposed to get called
     *  within the I3RecoPulseSeriesMap loop for a particular DOM.
     */
    inline
    void
    DoNextIteration(double pulseTime, double omZ)
    {
        ZTravelPulse_t zTravelPulse(pulseTime, omZ);
        zTravelPulseVec_.push_back(zTravelPulse);
    }

    //__________________________________________________________________________
    /** Finalizes the ZTravel calculation. This method is supposed to get called
     *  after the I3RecoPulseSeriesMap loop.
     */
    inline
    void
    FinalizeCalculation()
    {
        // zTravel is not defined for less then 4 pulses (e.g. hit DOMs).
        size_t zTravelPulseVecSize = zTravelPulseVec_.size();
        if(zTravelPulseVecSize >= 4)
        {
            std::sort(zTravelPulseVec_.begin(), zTravelPulseVec_.end(), compare_ZTravelPulse_pulseTime);
            double zMeanFirstQuartile = 0.;
            size_t zTravelPulseVecFirstQuartileSize = zTravelPulseVecSize/4;
            for(size_t i=0; i<zTravelPulseVecFirstQuartileSize; ++i)
                zMeanFirstQuartile += zTravelPulseVec_[i].omZ;
            zMeanFirstQuartile /= zTravelPulseVecFirstQuartileSize;

            zTravel_ = 0.;
            for(size_t i=0; i<zTravelPulseVecSize; ++i)
                zTravel_ += (zTravelPulseVec_[i].omZ - zMeanFirstQuartile);
            zTravel_ /= zTravelPulseVecSize;
        }
    }

    //__________________________________________________________________________
    /** Resets the ZTravelIterativeCalculator, in order to be able to use the
     *  same ZTravelIterativeCalculator object more than once.
     */
    inline
    void
    Reset()
    {
        zTravelPulseVec_.clear();
        zTravel_ = NAN;
    }
};
//==============================================================================

}/*details*/}/*hit_statistics*/}/*common_variables*/
//##############################################################################

#endif // COMMONVARIABLES_HITSTATISTICS_ZTRAVELITERATIVECALCULATOR_H_INCLUDED
