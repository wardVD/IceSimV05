/**
 * copyright  (C) 2005
 * the IceCube collaboration
 * Version $Id: I3TimeWindowCleaning.cxx 41353 2008-01-24 14:59:35Z schlenst $
 */

#include <dataclasses/physics/I3RecoHit.h>
#include <dataclasses/physics/I3MCHit.h>
#include <dataclasses/physics/I3RecoPulse.h>
#include <dataclasses/physics/I3DOMLaunch.h>

#include "DomTools/I3TimeWindowCleaning.h"


I3_MODULE(I3TimeWindowCleaning<I3RecoHit>);

I3_MODULE(I3TimeWindowCleaning<I3MCHit>);

I3_MODULE(I3TimeWindowCleaning<I3RecoPulse>);

I3_MODULE(I3TimeWindowCleaning<I3DOMLaunch>);

