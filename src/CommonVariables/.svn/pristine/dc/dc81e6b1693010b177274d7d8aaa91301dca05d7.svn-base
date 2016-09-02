/**
 * $Id$
 *
 * Copyright (C) 2012
 * Martin Wolf <martin.wolf@icecube.wisc.edu>
 * and the IceCube Collaboration <http://www.icecube.wisc.edu>
 *
 * @file hit_statistics/COGIterativeCalculator.h
 * @version $Revision$
 * @date $Date$
 * @author Martin Wolf <martin.wolf@icecube.wisc.edu>
 * @brief This file contains the definition and implementation of the iterative
 *        calculator for the COG position. The COGIterativeCalculator has been
 *        introduced in order to have only one place for the implementation of
 *        the COG, and to minimize the number of needed loops over an
 *        I3RecoPulseSeriesMap.
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
#ifndef COMMONVARIABLES_HITSTATISTICS_COGITERATIVECALCULATOR_H_INCLUDED
#define COMMONVARIABLES_HITSTATISTICS_COGITERATIVECALCULATOR_H_INCLUDED

#include <cmath>

#include "dataclasses/I3Position.h"

//##############################################################################
namespace common_variables { namespace hit_statistics { namespace details {

//==============================================================================
struct COGIterativeCalculator
{
    double x_;
    double y_;
    double z_;
    double pulseChargeSum_;

    //__________________________________________________________________________
    COGIterativeCalculator()
    {
        Reset();
    }

    //__________________________________________________________________________
    /** Returns the calculated COG as an I3Position object. The result of this
     *  method is only valid after the call of the FinalizeCalculation method,
     *  but this condition is not enforced.
     */
    inline
    I3Position
    AsI3Position()
    {
        return I3Position(x_, y_, z_, I3Position::car);
    }

    //__________________________________________________________________________
    /** Does the next iteration step. This method is supposed to get called
     *  within the I3RecoPulseSeries loop for a particular pulse.
     */
    inline
    void
    DoNextIteration(double omX, double omY, double omZ, double pulseCharge)
    {
        x_ += pulseCharge*omX;
        y_ += pulseCharge*omY;
        z_ += pulseCharge*omZ;

        if(std::isnan(pulseChargeSum_))
            pulseChargeSum_ = pulseCharge;
        else
            pulseChargeSum_ += pulseCharge;
    }

    //__________________________________________________________________________
    /** Finalizes the COG calculation. This method is supposed to get called
     *  after the I3RecoPulseSeriesMap loop.
     */
    inline
    void
    FinalizeCalculation()
    {
        if(std::isnan(pulseChargeSum_))
        {
            x_ = NAN;
            y_ = NAN;
            z_ = NAN;
        }
        else
        {
            if(pulseChargeSum_ == 0.)
                pulseChargeSum_ = 1.;
            x_ /= pulseChargeSum_;
            y_ /= pulseChargeSum_;
            z_ /= pulseChargeSum_;
        }
    }

    //__________________________________________________________________________
    /** Resets the COGIterativeCalculator, in order to be able to use the same
     *  COGIterativeCalculator object more than once.
     */
    inline
    void
    Reset()
    {
        x_ = 0.;
        y_ = 0.;
        z_ = 0.;
        pulseChargeSum_ = NAN;
    }
};
//==============================================================================

}/*details*/}/*hit_statistics*/}/*common_variables*/
//##############################################################################

#endif // COMMONVARIABLES_HITSTATISTICS_COGITERATIVECALCULATOR_H_INCLUDED
