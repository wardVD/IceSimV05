/** $Id$
 * @file
 * @author Jakob van Santen <vansanten@wisc.edu>
 *
 * $Revision$
 * $Date$
 */

#ifndef I3MUONGUN_CORSIKAGENERATIONPROBABILITY_H_INCLUDED
#define I3MUONGUN_CORSIKAGENERATIONPROBABILITY_H_INCLUDED

#include <MuonGun/Generator.h>
#include <MuonGun/SamplingSurface.h>

namespace I3MuonGun {

I3_FORWARD_DECLARATION(SamplingSurface);
I3_FORWARD_DECLARATION(Flux);
I3_FORWARD_DECLARATION(RadialDistribution);
I3_FORWARD_DECLARATION(EnergyDistribution);

/**
 * @brief A parametrization of the muon yield from direct air-shower simulation
 */
class CORSIKAGenerationProbability : public GenerationProbability {
public:
	CORSIKAGenerationProbability(SamplingSurfacePtr, FluxPtr, RadialDistributionPtr, EnergyDistributionPtr);
	
	SamplingSurfaceConstPtr GetSurface() const { return surface_; }
	FluxConstPtr GetFlux() const { return flux_; }
	RadialDistributionConstPtr GetRadialDistribution() const { return radialDistribution_; }
	EnergyDistributionConstPtr GetEnergyDistribution() const { return energyDistribution_; }
public:
	// GenerationProbability interface
	virtual SamplingSurfaceConstPtr GetInjectionSurface() const { return surface_; }
	GenerationProbabilityPtr Clone() const;
	virtual bool IsCompatible(GenerationProbabilityConstPtr) const;
	
protected:
	double GetLogGenerationProbability(const I3Particle &axis, const BundleConfiguration &bundle) const;
private:
	SamplingSurfacePtr surface_;
	FluxPtr flux_;
	RadialDistributionPtr radialDistribution_;
	EnergyDistributionPtr energyDistribution_;
};

}

#endif // I3MUONGUN_CORSIKAGENERATIONPROBABILITY_H_INCLUDED
