/** $Id$
 * @file
 * @author Jakob van Santen <vansanten@wisc.edu>
 *
 * $Revision$
 * $Date$
 */

#include <MuonGun/CompactTrack.h>
#include <icetray/python/dataclass_suite.hpp>

void
register_CompactTrack()
{
	using namespace I3MuonGun;
	namespace bp = boost::python;
	
	bp::class_<CompactTrack>("CompactTrack")
	    #define PROPS (Energy)(Radius)(Time)(Type)
	    BOOST_PP_SEQ_FOR_EACH(WRAP_PROP, CompactTrack, PROPS)
	    #undef PROPS
	    .def(bp::dataclass_suite<CompactTrack>())
	;
	
	bp::class_<std::vector<CompactTrack> >("CompactTrackSeries")
	    .def(bp::dataclass_suite<std::vector<CompactTrack> >())
	;
	
	bp::class_<TrackBundle, boost::shared_ptr<TrackBundle>, bp::bases<I3FrameObject> >("CompactTrackSeriesMap")
	    .def(bp::dataclass_suite<TrackBundle>())
	;
	
	register_pointer_conversions<TrackBundle>();
}
