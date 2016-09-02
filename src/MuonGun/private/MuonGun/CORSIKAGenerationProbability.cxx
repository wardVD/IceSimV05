/** $Id: CORSIKAGenerationProbability.cxx 136127 2015-08-12 13:54:32Z jvansanten $
 * @file
 * @author Jakob van Santen <vansanten@wisc.edu>
 *
 * $Revision: 136127 $
 * $Date: 2015-08-12 08:54:32 -0500 (Wed, 12 Aug 2015) $
 */

#include <MuonGun/I3MuonGun.h>
#include <MuonGun/CORSIKAGenerationProbability.h>

#include <dataclasses/physics/I3Particle.h>
#include <MuonGun/Flux.h>
#include <MuonGun/RadialDistribution.h>
#include <MuonGun/EnergyDistribution.h>

#include <boost/foreach.hpp>

namespace I3MuonGun {

CORSIKAGenerationProbability::CORSIKAGenerationProbability(SamplingSurfacePtr s, FluxPtr f, RadialDistributionPtr r, EnergyDistributionPtr e)
    : surface_(s), flux_(f), radialDistribution_(r), energyDistribution_(e)
{
	if (!surface_)
		log_fatal("No sampling surface defined!");
	if (!flux_)
		log_fatal("No flux defined!");
	if (!radialDistribution_)
		log_fatal("No radial distribution defined!");
	if (!energyDistribution_)
		log_fatal("No energy distribution defined!");
}

GenerationProbabilityPtr
CORSIKAGenerationProbability::Clone() const
{
	return boost::make_shared<CORSIKAGenerationProbability>(*this);
}

bool
CORSIKAGenerationProbability::IsCompatible(GenerationProbabilityConstPtr o) const
{
	boost::shared_ptr<const CORSIKAGenerationProbability> other
	    = boost::dynamic_pointer_cast<const CORSIKAGenerationProbability>(o);
	if (!other)
		return false;
	return (*surface_ == *(other->surface_) && *flux_ == *(other->flux_)
	    && *radialDistribution_ == *(other->radialDistribution_)
	    && *energyDistribution_ == *(other->energyDistribution_));
}

double
CORSIKAGenerationProbability::GetLogGenerationProbability(const I3Particle &axis, const BundleConfiguration &bundlespec) const
{	
	std::pair<double, double> steps = surface_->GetIntersection(axis.GetPos(), axis.GetDir());
	// This shower axis doesn't intersect the sampling surface. Bail.
	if (!std::isfinite(steps.first))
		return 0.;
	
	double h = GetDepth(axis.GetPos().GetZ() + steps.first*axis.GetDir().GetZ());
	double coszen = cos(axis.GetDir().GetZenith());
	unsigned m = static_cast<unsigned>(bundlespec.size());
	double logprob = flux_->GetLog(h, coszen, m);
	BOOST_FOREACH(const BundleConfiguration::value_type &track, bundlespec)
		logprob += energyDistribution_->GetLog(h, coszen, m, track.radius, track.energy);
	
	return logprob;
}

}
