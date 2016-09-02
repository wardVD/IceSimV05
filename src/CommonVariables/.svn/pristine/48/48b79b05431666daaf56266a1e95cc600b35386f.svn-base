/**
 * $Id$
 *
 * Copyright (C) 2012
 * Martin Wolf <martin.wolf@icecube.wisc.edu>
 * and the IceCube Collaboration <http://www.icecube.wisc.edu>
 *
 * @file WeightedMeanIterativeCalculator.h
 * @version $Revision$
 * @date $Date$
 * @author Martin Wolf <martin.wolf@icecube.wisc.edu>
 * @brief This file contains the definition and implementation of the iterative
 *        calculator for a weighted mean value. The
 *        WeightedMeanIterativeCalculator has been introduced in order to have
 *        only one place for the implementation of a weighted mean value, and to
 *        minimize the number of needed loops over an I3RecoPulseSeriesMap.
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
#ifndef COMMONVARIABLES_WEIGHTEDMEANITERATIVECALCULATOR_H_INCLUDED
#define COMMONVARIABLES_WEIGHTEDMEANITERATIVECALCULATOR_H_INCLUDED

#include <cmath>

//##############################################################################
namespace common_variables { namespace details {

//==============================================================================
struct WeightedMeanIterativeCalculator
{
    double weightTimesValueSum_;
    double weightSum_;
    double weightedMean_;

    //__________________________________________________________________________
    WeightedMeanIterativeCalculator()
    {
        Reset();
    }

    //__________________________________________________________________________
    /** Returns the calculated weighted mean value as a double value. The
     *  result of this method is only valid after the call of the
     *  FinalizeCalculation method.
     */
    inline
    double
    AsDouble()
    {
        return weightedMean_;
    }

    //__________________________________________________________________________
    /** Does the next iteration step for the weighted mean value calculation.
     */
    inline
    void
    DoNextIteration(double value, double weight)
    {
        weightTimesValueSum_ += weight*value;
        weightSum_           += weight;
    }

    //__________________________________________________________________________
    /** Finalizes the weighted mean value calculation.
     */
    inline
    void
    FinalizeCalculation()
    {
        if(weightSum_ > 0.)
        {
            weightedMean_ = weightTimesValueSum_ / weightSum_;
        }
    }

    //__________________________________________________________________________
    /** Resets the WeightedMeanIterativeCalculator, in order to be able to use
     *  the same WeightedMeanIterativeCalculator object more than once.
     */
    inline
    void
    Reset()
    {
        weightTimesValueSum_ = 0.;
        weightSum_           = 0.;
        weightedMean_        = NAN;
    }
};
//==============================================================================

}/*details*/}/*common_variables*/
//##############################################################################

#endif // COMMONVARIABLES_WEIGHTEDMEANITERATIVECALCULATOR_H_INCLUDED
