#include "icepick/I3AndServices.h"
#include "interfaces/I3IcePickModule.h"
#include "interfaces/I3IceForkModule.h"
#include "icetray/I3IcePickInstaller.h"

I3_MODULE(I3IcePickModule<I3AndServices>);
I3_MODULE(I3IceForkModule<I3AndServices>);
I3_SERVICE_FACTORY(I3IcePickInstaller<I3AndServices>);
