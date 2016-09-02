/**
 * copyright (C) 2011
 * Claudio Kopper <claudio.kopper@nikhef.nl>
 * $Id$
 *
 * @file ConvertToGST.h
 * @version $Revision$
 * @date $Date$
 * @author Claudio Kopper
 *
 *
 *  This file is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>
 *  
 */

#ifndef TRANDOMWRAPPERFORI3RANDOMSERVICE_H_INCLUDED
#define TRANDOMWRAPPERFORI3RANDOMSERVICE_H_INCLUDED


//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TRandomWrapperForI3RandomService                                     //
//                                                                      //
// looks like a ROOT TRandom to the outside, but actually uses an       //
// I3RandomService to generate random numbers.                          //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#ifndef ROOT_TRandom
#include "TRandom.h"
#endif

#include "phys-services/I3RandomService.h"

class TRandomWrapperForI3RandomService : public TRandom
{
private:
    I3RandomServicePtr randomService_;
    
public:
    TRandomWrapperForI3RandomService(I3RandomServicePtr randomService);
    virtual ~TRandomWrapperForI3RandomService();
    virtual Double_t  Rndm(Int_t i=0);
    virtual void      RndmArray(Int_t n, Float_t *array);
    virtual void      RndmArray(Int_t n, Double_t *array);
    virtual void      SetSeed(UInt_t seed=0);
};

#endif //TRANDOMWRAPPERFORI3RANDOMSERVICE_H_INCLUDED
