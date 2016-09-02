/** $Id$
 * @file
 * @author Jakob van Santen <vansanten@wisc.edu>
 *
 * $Revision$
 * $Date$
 */

#include "MuonGun/MuonPropagator.h"

void
register_MuonPropagator()
{
	using namespace I3MuonGun;
	using namespace boost::python;
	
	class_<MuonPropagator, boost::shared_ptr<MuonPropagator> >("MuonPropagator",
	    init<const std::string&, double, double, double>((
	    arg("medium")="ice", arg("ecut")=-1, arg("vcut")=-1, arg("rho")=1.0)))
	    .def("propagate", &MuonPropagator::propagate, (arg("particle"), arg("distance"),
	        arg("losses")=boost::shared_ptr<std::vector<I3Particle> >()))
	    .def("stochastic_rate", &MuonPropagator::GetStochasticRate, (arg("energy"), arg("fraction"), arg("type")=I3Particle::MuMinus))
	    .def("total_stochastic_rate", &MuonPropagator::GetTotalStochasticRate, (arg("energy"), arg("type")=I3Particle::MuMinus))
	    .def("set_seed", &MuonPropagator::SetSeed)
	    .staticmethod("set_seed")
	;
	
	class_<Crust, boost::shared_ptr<Crust> >("Crust",
	    init<boost::shared_ptr<MuonPropagator> >())
	    .def("add_layer", &Crust::AddLayer)
	    .def("ingest", &Crust::Ingest)
	;
}
