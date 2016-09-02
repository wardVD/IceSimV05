/** $Id: Generator.cxx 136127 2015-08-12 13:54:32Z jvansanten $
 * @file
 * @author Jakob van Santen <vansanten@wisc.edu>
 *
 * $Revision: 136127 $
 * $Date: 2015-08-12 08:54:32 -0500 (Wed, 12 Aug 2015) $
 */

#include <MuonGun/Generator.h>
#include <MuonGun/SamplingSurface.h>
#include <dataclasses/physics/I3Particle.h>
#include <icetray/python/dataclass_suite.hpp>
#include <boost/serialization/list.hpp>

static I3MuonGun::SamplingSurfacePtr
GetInjectionSurface(const I3MuonGun::GenerationProbability &self)
{
	return boost::const_pointer_cast<I3MuonGun::SamplingSurface>(self.GetInjectionSurface());
}

void register_Generator()
{
	using namespace I3MuonGun;
	using namespace boost::python;
	
	class_<GenerationProbability, bases<I3FrameObject>, GenerationProbabilityPtr, boost::noncopyable>("GenerationProbability", no_init)
	    .def("generated_events", &GenerationProbability::GetGeneratedEvents)
	    .add_property("total_events", &GenerationProbability::GetTotalEvents, &GenerationProbability::SetTotalEvents)
	    .add_property("surface", &GetInjectionSurface)
	    .def("__add__",  (GenerationProbabilityPtr (*)(GenerationProbabilityPtr, GenerationProbabilityPtr))(&operator+))
	    .def("__mul__",  (GenerationProbabilityPtr (*)(GenerationProbabilityPtr, double))(&operator*))
	    .def("__rmul__", (GenerationProbabilityPtr (*)(GenerationProbabilityPtr, double))(&operator*))
	    .def("__imul__", (GenerationProbabilityPtr (*)(GenerationProbabilityPtr, double))(&operator*=))
	;
	implicitly_convertible<GenerationProbabilityPtr, GenerationProbabilityConstPtr>();
	register_pointer_conversions<GenerationProbability>();
	
	class_<Generator, bases<GenerationProbability, I3FrameObject>, boost::noncopyable>("Generator", no_init)
	;
	register_pointer_conversions<Generator>();
	
	class_<BundleEntry>("BundleEntry", init<double, double>())
	    .def_readwrite("radius", &BundleEntry::radius)
	    .def_readwrite("energy", &BundleEntry::energy)
	;
	
	class_<BundleConfiguration, boost::shared_ptr<BundleConfiguration> >("BundleConfiguration")
	    .def(list_indexing_suite<BundleConfiguration>())
	;
}
