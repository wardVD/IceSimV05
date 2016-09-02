/** $Id$
 * @file
 * @author Jakob van Santen <vansanten@wisc.edu>
 *
 * $Revision$
 * $Date$
 */

#include <MuonGun/Flux.h>
#include <icetray/python/gil_holder.hpp>
#include "utils.h"

namespace I3MuonGun {

using namespace boost::python;

class PyFlux : public Flux, public wrapper<Flux> {
public:
	double GetLog(double h, double ct, unsigned m) const
	{
		detail::gil_holder lock;
		return get_override("GetLog")(h, ct, m);
	}
};

}

void register_Flux()
{
	using namespace I3MuonGun;
	using namespace boost::python;
	
	class_<Flux, boost::noncopyable>("Flux", no_init)
	    DEF("__call__", &Flux::operator(), (args("depth"), "cos_theta", "multiplicity"))
	    #define PROPS (MinMultiplicity)(MaxMultiplicity)
	    BOOST_PP_SEQ_FOR_EACH(WRAP_PROP, Flux, PROPS)
	    #undef PROPS
	;
	
	class_<SplineFlux, bases<Flux> >("SplineFlux", init<const std::string&, const std::string&>())
	;
	
	class_<BMSSFlux, bases<Flux> >("BMSSFlux")
	;
	
	class_<PyFlux, boost::noncopyable>("FluxBase")
    	    DEF("__call__", &Flux::operator(), (args("depth"), "cos_theta", "multiplicity"))
    	    #define PROPS (MinMultiplicity)(MaxMultiplicity)
    	    BOOST_PP_SEQ_FOR_EACH(WRAP_PROP, Flux, PROPS)
    	    #undef PROPS
	;
}
