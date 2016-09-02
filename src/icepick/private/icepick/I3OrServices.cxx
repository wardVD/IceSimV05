#include "icepick/I3OrServices.h"
#include "interfaces/I3IcePickModule.h"
#include "interfaces/I3IceForkModule.h"
#include "icetray/I3IcePickInstaller.h"

I3_MODULE(I3IcePickModule<I3OrServices>);
I3_MODULE(I3IceForkModule<I3OrServices>);
I3_SERVICE_FACTORY(I3IcePickInstaller<I3OrServices>);
