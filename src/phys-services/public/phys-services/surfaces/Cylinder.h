/** $Id: Cylinder.h 137064 2015-08-31 18:24:47Z jvansanten $
 * @file
 * @author Jakob van Santen <jakob.van.santen@desy.de>
 *
 * $Revision: 137064 $
 * $Date: 2015-08-31 13:24:47 -0500 (Mon, 31 Aug 2015) $
 */

#ifndef I3SURFACES_CYLINDER_H_INCLUDED
#define I3SURFACES_CYLINDER_H_INCLUDED

#include <phys-services/surfaces/SamplingSurface.h>
#include <phys-services/surfaces/detail/CylinderBase.h>

namespace I3Surfaces {

/**
 * @brief A cylinder aligned with the z axis
 */
class Cylinder : public detail::CylinderBase<SamplingSurface> {
private:
	typedef detail::CylinderBase<SamplingSurface> Base;
public:
	virtual ~Cylinder();
	Cylinder(double length, double radius, I3Position center=I3Position(0,0,0)) : Base(length, radius, center) {};

private:
	Cylinder() {}
	
	friend class boost::serialization::access;
	template <typename Archive>
	void serialize(Archive &, unsigned);
};

I3_POINTER_TYPEDEFS(Cylinder);

}

BOOST_CLASS_VERSION(I3Surfaces::Cylinder, 0);

#endif // I3SURFACES_CYLINDER_H_INCLUDED