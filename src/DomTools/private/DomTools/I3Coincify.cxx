/**
 * copyright  (C) 2005
 * the IceCube collaboration
 * Version $Id: I3Coincify.cxx 41353 2008-01-24 14:59:35Z schlenst $
 */

#include "DomTools/I3Coincify.h"
#include <dataclasses/physics/I3RecoPulse.h>
#include <dataclasses/physics/I3RecoHit.h>
#include <dataclasses/physics/I3MCHit.h>

I3_MODULE(I3Coincify<I3RecoPulse>);

I3_MODULE(I3Coincify<I3RecoHit>);

I3_MODULE(I3Coincify<I3MCHit>);

I3_MODULE(I3Coincify<I3DOMLaunch>);


