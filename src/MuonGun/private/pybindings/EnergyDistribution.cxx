/** $Id: EnergyDistribution.cxx 123615 2014-09-19 13:31:22Z jvansanten $
 * @file
 * @author Jakob van Santen <vansanten@wisc.edu>
 *
 * $Revision: 123615 $
 * $Date: 2014-09-19 08:31:22 -0500 (Fri, 19 Sep 2014) $
 */

#include <MuonGun/EnergyDistribution.h>
#include <phys-services/I3RandomService.h>
#include "utils.h"

#include <icetray/python/gil_holder.hpp>

namespace I3MuonGun {

using namespace boost::python;

class PyEnergyDistribution : public EnergyDistribution, public wrapper<EnergyDistribution> {
public:
	virtual double GetLog(double depth, double cos_theta,
	    unsigned multiplicity, double radius, double energy) const
	{
		detail::gil_holder lock;
		return get_override("GetLog")(depth, cos_theta, multiplicity, radius, energy);
	}
	virtual double Generate(I3RandomService &rng, double depth, double cos_theta,
	    unsigned multiplicity, double radius) const
	{
		return 0;
	}
	virtual bool operator==(const EnergyDistribution&) const
	{
		return false;
	}
	

};

}

void register_EnergyDistribution()
{
	using namespace boost::python;
	using namespace I3MuonGun;
	
	class_<EnergyDistribution, EnergyDistributionPtr, boost::noncopyable>("EnergyDistribution", no_init)
	    DEF("__call__", &EnergyDistribution::operator(), (arg("depth"), "cos_theta", "multiplicity", "radius", "energy"))
	    .def("generate", &EnergyDistribution::Generate, (arg("rng"), arg("depth"), "cos_theta", "multiplicity", "radius"))
	;
	
	class_<SplineEnergyDistribution, boost::shared_ptr<SplineEnergyDistribution>,
	    bases<EnergyDistribution> >("SplineEnergyDistribution",
	    init<const std::string&, const std::string&>((arg("singles"), "bundles")))
	;
	
	class_<BMSSEnergyDistribution, boost::shared_ptr<BMSSEnergyDistribution>,
	    bases<EnergyDistribution> >("BMSSEnergyDistribution")
	;
	
	class_<PyEnergyDistribution, boost::noncopyable>("EnergyDistributionBase")
    	    DEF("__call__", &EnergyDistribution::operator(), (arg("depth"), "cos_theta", "multiplicity", "radius", "energy"))
    	    .def("generate", &EnergyDistribution::Generate, (arg("rng"), arg("depth"), "cos_theta", "multiplicity", "radius"))
		    ;
	
	class_<OffsetPowerLaw, boost::shared_ptr<OffsetPowerLaw> >("OffsetPowerLaw",
	    init<double,double,double,double>((arg("gamma"), "offset", "min", "max")))
	    DEF("__call__", &OffsetPowerLaw::operator(), (arg("energy")))
	    .def("generate", &OffsetPowerLaw::Generate)
	;
}
