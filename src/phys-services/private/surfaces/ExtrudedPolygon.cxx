/** $Id: ExtrudedPolygon.cxx 137064 2015-08-31 18:24:47Z jvansanten $
 * @file
 * @author Jakob van Santen <jakob.van.santen@desy.de>
 *
 * $Revision: 137064 $
 * $Date: 2015-08-31 13:24:47 -0500 (Mon, 31 Aug 2015) $
 */

#include <phys-services/surfaces/ExtrudedPolygon.h>

namespace I3Surfaces {

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

I3_SERIALIZABLE(I3Surfaces::ExtrudedPolygon);
