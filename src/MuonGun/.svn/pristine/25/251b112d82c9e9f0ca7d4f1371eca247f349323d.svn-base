/** $Id$
 * @file
 * @author Jakob van Santen <vansanten@wisc.edu>
 *
 * $Revision$
 * $Date$
 */

#ifndef I3MUONGUN_CYLINDER_H_INCLUDED
#define I3MUONGUN_CYLINDER_H_INCLUDED

#include <MuonGun/SamplingSurface.h>
#include <phys-services/surfaces/detail/CylinderBase.h>
#include <MuonGun/UprightSurface.h>

class I3Direction;
class I3RandomService;

namespace I3MuonGun {

/**
 * @brief A cylinder aligned with the z axis
 */
class Cylinder : public detail::UprightSurface<I3Surfaces::detail::CylinderBase<SamplingSurface > > {
private:
	typedef I3Surfaces::detail::CylinderBase<SamplingSurface > CylinderBase;
	typedef detail::UprightSurface<CylinderBase > Base;
public:
	Cylinder(double length, double radius, I3Position center=I3Position(0,0,0)) : Base(length, radius, center)
	{}

	// SamplingSurface interface
	bool operator==(const SamplingSurface&) const;

	double GetLength() const { return CylinderBase::GetLength(); };

protected:
	// UprightSurface interface
	double GetTopArea() const;
	double GetSideArea() const;
	std::pair<double, double> GetZRange() const;
	
private:
	Cylinder() {}
	
	friend class boost::serialization::access;
	template <typename Archive>
	void serialize(Archive &, unsigned);
};

I3_POINTER_TYPEDEFS(Cylinder);

}

BOOST_CLASS_VERSION(I3MuonGun::Cylinder, 1);

#endif // I3MUONGUN_CYLINDER_H_INCLUDED
