
#include "TestUtils.h"
#include "neutrino-generator/Particle.h"
#include "earthmodel-service/EarthModelService.h"
#include "phys-services/surfaces/Cylinder.h"
#include <boost/make_shared.hpp>
#include <vector>

namespace nugen { namespace test {

//______________________________________________________________________
boost::shared_ptr<I3RandomService> make_random(int seed)
{
        boost::shared_ptr<I3RandomService> rng(new I3GSLRandomService(seed));
        return rng;
}

//______________________________________________________________________
boost::shared_ptr<Steering> make_steering(std::string captype,
                                            SimMode simmode,
                                            VTXGenMode vtxgen,
                                            InjectionMode itype)
{
	using namespace nugen;

        // make earthmodel with icecap type option
        std::vector<std::string> dummy;
        boost::shared_ptr<earthmodel::EarthModelService> earth(
                        new earthmodel::EarthModelService("EarthModel", "",
                                                           dummy, dummy, captype));
	boost::shared_ptr<Steering> steering(new nugen::Steering(earth,
	    simmode, vtxgen, itype));

	return steering;
}
	
//______________________________________________________________________
boost::shared_ptr<I3NuGInjector> make_injector(nugen::InjectionMode itype)
{
	using namespace nugen;

	boost::shared_ptr<I3RandomService> rng = make_random(1337);
        // make earthmodel with IceSheet option
	boost::shared_ptr<Steering> steering = make_steering("IceSheet",
	    FULL /* sim mode */, NUGEN /* vertex mode */, itype /* injection type */
	);

        if (itype == CIRCLE) {
           std::vector<double> cyl_param(3, 0); // value 0 is always ignored
           cyl_param[0] = 1e3;
	   steering->SetCylinderParams(cyl_param);
        } else {
	   steering->SetDetectionSurface(boost::make_shared<I3Surfaces::Cylinder>(1e3, 500));
        }

	return boost::make_shared<I3NuGInjector>(rng, steering, "harold");
}

//______________________________________________________________________
boost::shared_ptr<I3NuGInteractionInfo> make_interaction(boost::shared_ptr<I3RandomService> rng, 
                                                         boost::shared_ptr<Steering> steering)
{
	using namespace nugen;
	boost::shared_ptr<I3NuGInteractionInfo> intinfo(new I3NuGInteractionInfo(rng, steering, "csms"));
        // initialize it before return
	intinfo->Initialize();
        return intinfo;
}
	
}}
