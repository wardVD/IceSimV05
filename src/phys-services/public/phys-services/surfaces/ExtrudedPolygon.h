/** $Id: ExtrudedPolygon.h 137064 2015-08-31 18:24:47Z jvansanten $
 * @file
 * @author Jakob van Santen <jakob.van.santen@desy.de>
 *
 * $Revision: 137064 $
 * $Date: 2015-08-31 13:24:47 -0500 (Mon, 31 Aug 2015) $
 */

#ifndef I3SURFACES_EXTRUDEDPOLYGON_H_INCLUDED
#define I3SURFACES_EXTRUDEDPOLYGON_H_INCLUDED

#include <phys-services/surfaces/SamplingSurface.h>
#include <phys-services/surfaces/detail/ExtrudedPolygonBase.h>

namespace I3Surfaces {

class ExtrudedPolygon : public ExtrudedPolygonBase<SamplingSurface> {
private:
	typedef ExtrudedPolygonBase<SamplingSurface> Base;
public:
	ExtrudedPolygon(const std::vector<I3Position> &points, double padding=0.) : Base(points, padding) {};
	~ExtrudedPolygon();
private:
	ExtrudedPolygon() {}
	friend class boost::serialization::access;
	template <typename Archive>
	void serialize(Archive &, unsigned);
};

I3_POINTER_TYPEDEFS(ExtrudedPolygon);

}

BOOST_CLASS_VERSION(I3Surfaces::ExtrudedPolygon, 0);

#endif // I3SURFACES_EXTRUDEDPOLYGON_H_INCLUDED
