/**
 * $Id$
 *
 * Copyright (C) 2012
 * Martin Wolf <martin.wolf@icecube.wisc.edu>
 * and the IceCube Collaboration <http://www.icecube.wisc.edu>
 *
 * @file hit_statistics/SigmaIterativeCalculator.h
 * @version $Revision$
 * @date $Date$
 * @author Martin Wolf <martin.wolf@icecube.wisc.edu>
 * @brief This file contains the definition and implementation of the iterative
 *        calculator for a sigma value (for a given base value). The
 *        SigmaIterativeCalculator has been introduced in order to have only one
 *        place for the implementation of a sigma value, and to minimize the
 *        number of needed loops over an I3RecoPulseSeriesMap.
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
#ifndef COMMONVARIABLES_HITSTATISTICS_SIGMAITERATIVECALCULATOR_H_INCLUDED
#define COMMONVARIABLES_HITSTATISTICS_SIGMAITERATIVECALCULATOR_H_INCLUDED

#include <cmath>

//##############################################################################
namespace common_variables { namespace hit_statistics { namespace details {

//==============================================================================
struct SigmaIterativeCalculator
{
    uint64_t n_;
    double   weightSum_;
    double   weightSquaredSum_;
    double   weightTimesValueSum_;
    double   weightTimesValueSquaredSum_;
    double   sigma_;

    //__________________________________________________________________________
    SigmaIterativeCalculator()
    {
        Reset();
    }

    //__________________________________________________________________________
    /** Returns the calculated sigma value as a double value. The result of this
     *  method is only valid after the call of the FinalizeCalculation method,
     *  but this condition is not enforced.
     */
    inline
    double
    AsDouble()
    {
        return sigma_;
    }

    //__________________________________________________________________________
    /** Does the next iteration step for the sigma calculation.
     */
    inline
    void
    DoNextIteration(double value, double weight)
    {
        ++n_;
        weightSum_                  += weight;
        weightSquaredSum_           += weight*weight;
        weightTimesValueSum_        += weight*value;
        weightTimesValueSquaredSum_ += weight*value*value;
    }

    //__________________________________________________________________________
    /** Finalizes the sigma value calculation:
     *
     *    \f$ sigma = sqrt(variance) = sqrt( n_eq/(n_eq - 1) * (E[x^2] - E[x]^2)) \f$
     *
     *  where \f$ n_eq = (\sum w_i)^2 / (\sum w_i^2) \f$ to account for the
     *  unbiased mean.
     */
    inline
    void
    FinalizeCalculation()
    {
        if(n_ > 1) // The variance is only defined for more than one value!
        {
            sigma_ = std::sqrt(
                weightSum_*weightSum_/weightSquaredSum_/(weightSum_*weightSum_/weightSquaredSum_ - 1.)*(
                weightTimesValueSquaredSum_/weightSum_ -
                weightTimesValueSum_/weightSum_*weightTimesValueSum_/weightSum_
                )
            );
        }
    }

    //__________________________________________________________________________
    /** Resets the SigmaIterativeCalculator, in order to be able to use the same
     *  SigmaIterativeCalculator object more than once.
     */
    inline
    void
    Reset()
    {
        n_                          = 0;
        weightSum_                  = 0.;
        weightSquaredSum_           = 0.;
        weightTimesValueSum_        = 0.;
        weightTimesValueSquaredSum_ = 0.;
        sigma_                      = NAN;
    }
};
//==============================================================================

}/*details*/}/*hit_statistics*/}/*common_variables*/
//##############################################################################

#endif // COMMONVARIABLES_HITSTATISTICS_SIGMAITERATIVECALCULATOR_H_INCLUDED
