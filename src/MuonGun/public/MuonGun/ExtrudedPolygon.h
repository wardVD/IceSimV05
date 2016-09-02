/** $Id: ExtrudedPolygon.h 137064 2015-08-31 18:24:47Z jvansanten $
 * @file
 * @author Jakob van Santen <jakob.van.santen@desy.de>
 *
 * $Revision: 137064 $
 * $Date: 2015-08-31 13:24:47 -0500 (Mon, 31 Aug 2015) $
 */

#ifndef I3MUONGUN_EXTRUDEDPOLYGON_H_INCLUDED
#define I3MUONGUN_EXTRUDEDPOLYGON_H_INCLUDED

#include <MuonGun/SamplingSurface.h>
#include <phys-services/surfaces/detail/ExtrudedPolygonBase.h>
#include <MuonGun/UprightSurface.h>

namespace I3MuonGun {

class ExtrudedPolygon : public detail::UprightSurface<I3Surfaces::ExtrudedPolygonBase<SamplingSurface > > {
private:
	typedef I3Surfaces::ExtrudedPolygonBase<SamplingSurface > ExtrudedPolygonBase;
	typedef detail::UprightSurface<ExtrudedPolygonBase > Base;
public:
	virtual ~ExtrudedPolygon();
	ExtrudedPolygon(const std::vector<I3Position> &points, double padding=0.) : Base(points, padding) {};
	
	virtual bool operator==(const SamplingSurface&) const
	{
		return false;
	}

protected:
	// UprightSurface interface
	double GetTopArea() const { return ExtrudedPolygonBase::GetCapArea(); };
	double GetSideArea() const { return ExtrudedPolygonBase::GetAverageSideArea(); };
	double GetLength() const { return ExtrudedPolygonBase::GetLength(); };
	std::pair<double, double> GetZRange() const { return ExtrudedPolygonBase::GetZRange(); };

private:
	ExtrudedPolygon() {}
	friend class boost::serialization::access;
	template <typename Archive>
	void serialize(Archive &, unsigned);
};

I3_POINTER_TYPEDEFS(ExtrudedPolygon);

}

BOOST_CLASS_VERSION(I3MuonGun::ExtrudedPolygon, 0);

#endif // I3MUONGUN_EXTRUDEDPOLYGON_H_INCLUDED
