/** $Id$
 * @file
 * @author Jakob van Santen <vansanten@wisc.edu>
 *
 * $Revision$
 * $Date$
 */

#include <MuonGun/CORSIKAGenerationProbability.h>
#include <MuonGun/Flux.h>
#include <MuonGun/RadialDistribution.h>
#include <MuonGun/EnergyDistribution.h>

void register_CORSIKAGenerationProbability()
{
	using namespace I3MuonGun;
	using namespace boost::python;
	
	class_<CORSIKAGenerationProbability, boost::shared_ptr<CORSIKAGenerationProbability>, bases<GenerationProbability> >
	    ("CORSIKAGenerationProbability", init<SamplingSurfacePtr, FluxPtr, RadialDistributionPtr, EnergyDistributionPtr>())
	    #define RO_PROPS (Flux)(RadialDistribution)(EnergyDistribution)
	    BOOST_PP_SEQ_FOR_EACH(WRAP_PROP_RO, CORSIKAGenerationProbability, RO_PROPS)
	    #undef RO_PROPS
	;
}
