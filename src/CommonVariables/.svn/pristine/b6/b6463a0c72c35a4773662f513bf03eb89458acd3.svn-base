/**
 * $Id$
 *
 * Copyright (C) 2012
 * Martin Wolf <martin.wolf@icecube.wisc.edu>
 * and the IceCube Collaboration <http://www.icecube.wisc.edu>
 *
 * @file hit_multiplicity/NHitStringsIterativeCalculator.h
 * @version $Revision$
 * @date $Date$
 * @author Martin Wolf <martin.wolf@icecube.wisc.edu>
 * @brief This file contains the definition and implementation of the iterative
 *        calculator for the number of hit strings. The
 *        NHitStringsIterativeCalculator has been introduced in order to have
 *        only one place for the implementation of the counting of hit strings.
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
#ifndef COMMONVARIABLES_HITMULTIPLICITY_NHITSTRINGSITERATIVECALCULATOR_H_INCLUDED
#define COMMONVARIABLES_HITMULTIPLICITY_NHITSTRINGSITERATIVECALCULATOR_H_INCLUDED

#include <set>

//##############################################################################
namespace common_variables { namespace hit_multiplicity { namespace details {

//==============================================================================
struct NHitStringsIterativeCalculator
{
    std::set<int> hitStrings_;
    uint32_t      nHitStrings_;

    //__________________________________________________________________________
    NHitStringsIterativeCalculator()
    {
        Reset();
    }

    //__________________________________________________________________________
    /** Returns the calculated number of hit strings as an uint32_t value. The
     *  result of this method is only valid after the call of the
     *  FinalizeCalculation method.
     */
    inline
    uint32_t
    AsUInt32()
    {
        return nHitStrings_;
    }

    //__________________________________________________________________________
    /** Does the next iteration step for the number of hit strings calculation.
     */
    inline
    void
    DoNextIteration(int string)
    {
        if(hitStrings_.empty())
            hitStrings_.insert(string);
        else
            // In case the string numbers are somehow sorted from lower to higer
            // values, this will be a more efficient insert operation.
            hitStrings_.insert(--(hitStrings_.end()), string);
    }

    //__________________________________________________________________________
    /** Finalizes the number of hit strings calculation.
     */
    inline
    void
    FinalizeCalculation()
    {
        nHitStrings_ = hitStrings_.size();
    }

    //__________________________________________________________________________
    /** Resets the NHitStringsIterativeCalculator, in order to be able to use
     *  the same NHitStringsIterativeCalculator object more than once.
     */
    inline
    void
    Reset()
    {
        hitStrings_.clear();
        nHitStrings_ = 0;
    }
};
//==============================================================================

}/*details*/}/*hit_multiplicity*/}/*common_variables*/
//##############################################################################

#endif // COMMONVARIABLES_HITMULTIPLICITY_NHITSTRINGSITERATIVECALCULATOR_H_INCLUDED
