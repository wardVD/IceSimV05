/** $Id$
 * @file
 * @author Jakob van Santen <vansanten@wisc.edu>
 *
 * $Revision$
 * $Date$
 */

#include <icetray/I3FrameObject.h>
#include <icetray/load_project.h>

#ifdef USE_NUMPY
#include <boost/numpy.hpp>
#endif

namespace bp = boost::python;

#define REGISTER_THESE_THINGS                                           \
  (I3MuonGun)(RadialDistribution)(EnergyDistribution)                   \
  (Flux)(Generator)(Surface)(WeightCalculator)(CanCan)                  \
  (CORSIKAGenerationProbability)(Track)

#define REGISTER_EXTRA (histogram)(TrackBinner)(MuonPropagator)(CompactTrack)

#define I3_REGISTRATION_FN_DECL(r, data, t) void BOOST_PP_CAT(register_,t)();
#define I3_REGISTER(r, data, t) BOOST_PP_CAT(register_,t)();
BOOST_PP_SEQ_FOR_EACH(I3_REGISTRATION_FN_DECL, ~, REGISTER_THESE_THINGS)
#ifdef USE_PROPOSAL
BOOST_PP_SEQ_FOR_EACH(I3_REGISTRATION_FN_DECL, ~, REGISTER_EXTRA);
#endif

I3_PYTHON_MODULE(MuonGun)
{
#ifdef USE_NUMPY
	boost::numpy::initialize();
#endif
	load_project("libMuonGun", false);
	BOOST_PP_SEQ_FOR_EACH(I3_REGISTER, ~, REGISTER_THESE_THINGS);
	#ifdef USE_PROPOSAL
	BOOST_PP_SEQ_FOR_EACH(I3_REGISTER, ~, REGISTER_EXTRA);
	#endif
}

