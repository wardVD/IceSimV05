/** $Id$
 * @file
 * @author Jakob van Santen <jakob.van.santen@desy.de>
 *
 * $Revision$
 * $Date$
 */

#include <MuonGun/ExtrudedPolygon.h>

namespace I3MuonGun {

ExtrudedPolygon::~ExtrudedPolygon() {}

template <typename Archive>
void
ExtrudedPolygon::serialize(Archive &ar, unsigned version)
{
	if (version > 0)
		log_fatal_stream("Version "<<version<<" is from the future");

	ar & make_nvp("Base", base_object<Base>(*this));
}

}

template class I3Surfaces::ExtrudedPolygonBase<I3MuonGun::SamplingSurface>;
template class I3MuonGun::detail::UprightSurface<I3Surfaces::ExtrudedPolygonBase<I3MuonGun::SamplingSurface> >;

I3_SERIALIZABLE(I3MuonGun::ExtrudedPolygon);
