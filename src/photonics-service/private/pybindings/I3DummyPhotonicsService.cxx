#include <photonics-service/I3DummyPhotonicsService.h>

#include "wrappers.h"

using namespace boost::python;

void register_I3DummyPhotonicsService() {
	class_<I3DummyPhotonicsService,
	    boost::shared_ptr<I3DummyPhotonicsService> ,
	    bases<I3PhotonicsService > >("I3DummyPhotonicsService")
	    ;
}

