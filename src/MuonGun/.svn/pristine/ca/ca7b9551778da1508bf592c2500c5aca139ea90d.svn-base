/** $Id$
 * @file
 * @author Jakob van Santen <vansanten@wisc.edu>
 *
 * $Revision$
 * $Date$
 */

#include <MuonGun/RadialDistribution.h>
#include <phys-services/I3RandomService.h>
#include "utils.h"

void
register_RadialDistribution()
{
	namespace bp = boost::python;
	using namespace I3MuonGun;
	
	bp::class_<RadialDistribution, RadialDistributionPtr,
	    boost::noncopyable>("RadialDistribution", bp::no_init)
	    DEF("__call__", &RadialDistribution::operator(),
	        (bp::arg("depth"), "cos_theta", "multiplicity", "radius"))
	    .def("generate", &RadialDistribution::Generate)
	;
	
	bp::class_<BMSSRadialDistribution, BMSSRadialDistributionPtr,
	    bp::bases<RadialDistribution> >("BMSSRadialDistribution")
	;
	
	bp::class_<SplineRadialDistribution,
	    bp::bases<RadialDistribution> >("SplineRadialDistribution", bp::init<const std::string &>())
	;
}
