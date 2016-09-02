
#ifndef MUONGUN_FLOODLIGHT_H_INCLUDED
#define MUONGUN_FLOODLIGHT_H_INCLUDED

#include <MuonGun/Generator.h>
#include <MuonGun/SamplingSurface.h>
#include <MuonGun/EnergyDistribution.h>

namespace I3MuonGun {

class Floodlight : public Generator {
public:
	Floodlight(SamplingSurfacePtr, boost::shared_ptr<OffsetPowerLaw>, double cosMin=-1, double cosMax=1);
	
	// Generator Interface
	virtual void Generate(I3RandomService &rng, I3MCTree &tree, BundleConfiguration &bundle) const;
	virtual GenerationProbabilityPtr Clone() const;
	virtual bool IsCompatible(GenerationProbabilityConstPtr) const;
	virtual double GetLogGenerationProbability(const I3Particle &axis, const BundleConfiguration &bundle) const;
	virtual SamplingSurfaceConstPtr GetInjectionSurface() const { return surface_; }
	
private:
	Floodlight() {};
	friend class boost::serialization::access;
	template <typename Archive>
	void serialize(Archive &, unsigned);
	

	SamplingSurfacePtr surface_;
	boost::shared_ptr<OffsetPowerLaw> energyGenerator_;
	std::pair<double, double> zenith_range_;
	double log_acceptance_;
};

}

BOOST_CLASS_VERSION(I3MuonGun::Floodlight, 1);

#endif // MUONGUN_FLOODLIGHT_H_INCLUDED
