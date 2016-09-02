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

#include "genie-icetray/TRandomWrapperForI3RandomService.h"


TRandomWrapperForI3RandomService::TRandomWrapperForI3RandomService(I3RandomServicePtr randomService)
:
randomService_(randomService)
{
    if (!randomService_)
        log_fatal("I3RandomServicePtr is (null)!");
    
    SetName("TRandomWrapperForI3RandomService");
    SetTitle("Random number generator: wrapper for I3RandomService");
}

//______________________________________________________________________________
TRandomWrapperForI3RandomService::~TRandomWrapperForI3RandomService()
{
}

//______________________________________________________________________________
Double_t TRandomWrapperForI3RandomService::Rndm(Int_t)
{
    //  Produces uniformly-distributed floating points in ]0,1]
    return 1.-randomService_->Uniform(1.); // Uniform(a) returns [0,a[
}

//______________________________________________________________________________
void TRandomWrapperForI3RandomService::RndmArray(Int_t n, Float_t *array)
{
    // Return an array of n random numbers uniformly distributed in ]0,1]
    
    for(Int_t i=0; i<n; i++) array[i]=(Float_t)Rndm();
}

//______________________________________________________________________________
void TRandomWrapperForI3RandomService::RndmArray(Int_t n, Double_t *array)
{
    // Return an array of n random numbers uniformly distributed in ]0,1]
    
    for(Int_t i=0; i<n; i++) array[i]=(Double_t)Rndm();
}
//______________________________________________________________________________
void TRandomWrapperForI3RandomService::SetSeed(UInt_t seed)
{
    log_error("TRandomWrapperForI3RandomService::SetSeed() is invalid! Ignoring.");
}
