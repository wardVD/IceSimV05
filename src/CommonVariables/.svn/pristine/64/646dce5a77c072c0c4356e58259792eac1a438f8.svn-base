/**
 * $Id$
 *
 * Copyright (C) 2012
 * Martin Wolf <martin.wolf@icecube.wisc.edu>
 * and the IceCube Collaboration <http://www.icecube.wisc.edu>
 *
 * @file hit_statistics/ArithMeanIterativeCalculator.h
 * @version $Revision$
 * @date $Date$
 * @author Martin Wolf <martin.wolf@icecube.wisc.edu>
 * @brief This file contains the definition and implementation of the iterative
 *        calculator for an arithmetic mean value. The
 *        ArithMeanIterativeCalculator has been introduced in order to have only
 *        one place for the implementation of an arithmetic mean value, and to
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
#ifndef COMMONVARIABLES_HITSTATISTICS_ARITHMEANITERATIVECALCULATOR_H_INCLUDED
#define COMMONVARIABLES_HITSTATISTICS_ARITHMEANITERATIVECALCULATOR_H_INCLUDED

#include <cmath>
#include <stdint.h>

//##############################################################################
namespace common_variables { namespace hit_statistics { namespace details {

//==============================================================================
struct ArithMeanIterativeCalculator
{
    uint64_t n_;
    double   sum_;
    double   arithMean_;

    //__________________________________________________________________________
    ArithMeanIterativeCalculator()
    {
        Reset();
    }

    //__________________________________________________________________________
    /** Returns the calculated arithmetic mean value as a double value. The
     *  result of this method is only valid after the call of the
     *  FinalizeCalculation method.
     */
    inline
    double
    AsDouble()
    {
        return arithMean_;
    }

    //__________________________________________________________________________
    /** Does the next iteration step for the arithmetic mean value calculation.
     */
    inline
    void
    DoNextIteration(double value)
    {
        ++n_;
        sum_ += value;
    }

    //__________________________________________________________________________
    /** Finalizes the arithmetic mean value calculation.
     */
    inline
    void
    FinalizeCalculation()
    {
        if(n_ > 0)
        {
            arithMean_ = sum_ / n_;
        }
    }

    //__________________________________________________________________________
    /** Resets the ArithMeanIterativeCalculator, in order to be able to use the
     *  same ArithMeanIterativeCalculator object more than once.
     */
    inline
    void
    Reset()
    {
        n_         = 0;
        sum_       = 0.;
        arithMean_ = NAN;
    }
};
//==============================================================================

}/*details*/}/*hit_statistics*/}/*common_variables*/
//##############################################################################

#endif // COMMONVARIABLES_HITSTATISTICS_ARITHMEANITERATIVECALCULATOR_H_INCLUDED
