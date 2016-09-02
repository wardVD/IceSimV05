
#include "icetray/load_project.h"

#define REGISTER_THESE_THINGS                                           \
  (TypeDefs)(Steering)(I3NuGInteractionInfo)(I3NeutrinoGenerator)(InteractionBase)(InteractionCC)(InteractionNC)(InteractionGR)

#define I3_REGISTRATION_FN_DECL(r, data, t) void BOOST_PP_CAT(register_,t)();
#define I3_REGISTER(r, data, t) BOOST_PP_CAT(register_,t)();
BOOST_PP_SEQ_FOR_EACH(I3_REGISTRATION_FN_DECL, ~, REGISTER_THESE_THINGS)

I3_PYTHON_MODULE(neutrino_generator)
{
	boost::python::import("icecube.sim_services");
	boost::python::import("icecube.earthmodel_service");
	load_project("libneutrino-generator", false);
	
	BOOST_PP_SEQ_FOR_EACH(I3_REGISTER, ~, REGISTER_THESE_THINGS);
}
