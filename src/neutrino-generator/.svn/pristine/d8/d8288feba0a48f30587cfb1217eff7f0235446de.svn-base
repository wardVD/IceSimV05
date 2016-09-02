#ifndef TESTUTILS_H
#define TESTUTILS_H


#include "neutrino-generator/legacy/I3NuGInjector.h"
#include "neutrino-generator/legacy/I3NuGInteractionInfo.h"
#include "neutrino-generator/utils/EnumTypeDefs.h"
#include "neutrino-generator/Steering.h"
#include "phys-services/I3GSLRandomService.h"
#include <string>

namespace nugen { namespace test {

boost::shared_ptr<I3RandomService> make_random(int seed=1234);

boost::shared_ptr<Steering> make_steering(std::string captype = "IceSheet",
                                       SimMode simmode = FULL,
                                       VTXGenMode vtxgen = NUGEN,
                                       InjectionMode itype = CIRCLE);

boost::shared_ptr<I3NuGInjector> make_injector(nugen::InjectionMode itype);
boost::shared_ptr<I3NuGInteractionInfo> make_interaction(boost::shared_ptr<I3RandomService> rng,boost::shared_ptr<nugen::Steering> steering);

}}

#endif // TESTUTILS_H
