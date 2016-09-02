/**
 * Basic functionality tests for I3PhotoSplineService.
 *
 * copyright  (C) 2011
 * The Icecube Collaboration
 *
 * $Id$
 *
 * @version $Revision$
 * @date $LastChangedDate$
 * @author Jakob van Santen <vansanten@wisc.edu> Last changed by: $LastChangedBy$
 */

#ifdef USE_PHOTOSPLINE

#include "phys-services/I3GSLRandomService.h"
#include "photonics-service/I3PhotoSplineService.h"
#include "photonics-service/I3PhotonicsServiceCommons.h"
#include "dataclasses/I3Position.h"
#include <boost/filesystem.hpp>
#include <boost/make_shared.hpp>
#include <boost/foreach.hpp>
#include <cassert>

#include <I3Test.h>

extern "C" {
    #include "photospline/splinetable.h"
}

namespace {

struct CoordBundle {
	CoordBundle(const PhotonicsSource &ps, double x, double y, double z);
	
	static PhotonicsSource randomSource(I3RandomService &rng);
	static CoordBundle random(I3RandomService &rng, const PhotonicsSource &ps);
	
	geo_type geometry_;
	parity_type parity_;
	PhotonicsSource source_;
	double x_, y_, z_;
	
	double r_, rho_, cosAzi_, l_;
};

CoordBundle::CoordBundle(const PhotonicsSource &ps, double x, double y, double z)
    : geometry_(SPHERICAL), parity_(ODD), source_(ps), x_(x), y_(y), z_(z)
{
	double rhox, rhoy, rhoz;
	l_ = (x_ - source_.x)*source_.dirx + (y_ - source_.y)*source_.diry + 
	    (z_ - source_.z)*source_.dirz;
	rhox = x_ - (source_.x + l_*source_.dirx);
	rhoy = y_ - (source_.y + l_*source_.diry);
	rhoz = z_ - (source_.z + l_*source_.dirz);
	rho_ = sqrt(rhox*rhox + rhoy*rhoy + rhoz*rhoz);
	if( rho_ != 0 )
		cosAzi_ = (rhox*source_.perpx + rhoy*source_.perpy +
		    rhoz*source_.perpz)/rho_;
	else
		cosAzi_ = 0.0;
	if (cosAzi_ > 1.0)
		cosAzi_=1;
	else if (cosAzi_ < -1.0)
		cosAzi_=-1;
	
	if (parity_ == ODD) cosAzi_ *= -1;
	
	r_ = hypot(rho_, l_);
}

PhotonicsSource CoordBundle::randomSource(I3RandomService &rng)
{
	I3Particle p;
	p.SetPos(I3Position(sqrt(rng.Uniform(0, 500*500)), rng.Uniform(0, 2*M_PI), rng.Uniform(-500, 500), I3Position::cyl));
	p.SetDir(I3Direction(acos(rng.Uniform(-1, 1)), rng.Uniform(0, 2*M_PI)));
	p.SetShape(I3Particle::Cascade);
	p.SetTime(0);
	p.SetLength(0);
	p.SetEnergy(1);
	
	return PhotonicsSource(p);
}

CoordBundle CoordBundle::random(I3RandomService &rng, const PhotonicsSource &ps)
{
	
	I3Direction dir(acos(rng.Uniform(-1, 1)), rng.Uniform(0, 2*M_PI));
	double dist = cbrt(rng.Uniform(0, 400*400*400));
	I3Position pos(I3Position(ps.x, ps.y, ps.z) + dist*dir);
	
	return CoordBundle(ps, pos.GetX(), pos.GetY(), pos.GetZ());
}

} // namespace

TEST_GROUP(I3PhotoSplineService);

namespace fs = boost::filesystem;

struct TableSet {
	fs::path abs, prob;
};

static TableSet
get_splinetables()
{
	ENSURE(getenv("I3_TESTDATA") != NULL,
	    "I3_TESTDATA must be defined in the parent shell.");

	const std::string I3_TESTDATA(getenv("I3_TESTDATA"));
	
	fs::path abs_table(I3_TESTDATA + "/photospline/ems_z0_a0.pt.abs.fits");
	fs::path prob_table(I3_TESTDATA + "/photospline/ems_z0_a0.pt.prob.fits");
		
	ENSURE(fs::exists(abs_table), "Amplitude table exists.");
	ENSURE(fs::exists(prob_table), "Quantile table exists.");
	
	TableSet tabset;
	tabset.abs = abs_table;
	tabset.prob = prob_table;
	
	return tabset;
}

TEST(SamplingAtProblematicCoordinates)
{	
	TableSet tables = get_splinetables();
	
	I3RandomServicePtr rng(new I3GSLRandomService(1337));
	I3PhotoSplineServicePtr pxs = boost::make_shared<I3PhotoSplineService>(
	    tables.abs.string(), tables.prob.string(), 0.0);
	
	double mean_amp, distance, geo_time;
	unsigned npe = 10;
	I3Position ompos(-368.96798706054688, -210.44700622558594, -10);
	
	PhotonicsSource source(-319.30874149281027 /* x */,
	    -196.30586324235492 /* y */, -80.019894444077849 /* z */,
	    2.2372775142830483 /* zenith */, 6.0654194407395012 /* azimuth */,
	    1 /* speed */, 0 /* length */, 4579.0288237634586 /* energy */,
	    1 /* type */);
	
	pxs->SelectModuleCoordinates(ompos.GetX(), ompos.GetY(), ompos.GetZ());
	pxs->SelectSource(mean_amp, distance, geo_time, source);
	
	std::vector<double> delays(npe, -1.0);
	/* The following doesn't segfault (anymore). */
	pxs->GetTimeDelays(rng, &delays.front(), npe);
	
	std::vector<double>::const_iterator it = delays.begin();
	for ( ; it != delays.end(); it++)
		ENSURE(*it >= 0, "GetTimeDelays() returned a physical delay.");
}

namespace {
struct SelectSourceValues {
	double meanAmp, geoTime, emissionDistance;
	std::vector<double> quantiles;
	int success;
};
}

void verifyEvaluate(const std::vector<CoordBundle> &coords, I3RandomService &rng, I3PhotoSplineService &pxs)
{
	BOOST_FOREACH(const CoordBundle &coord, coords) {
		pxs.SelectModuleCoordinates(coord.x_, coord.y_, coord.z_);
		SelectSourceValues noGradient, withGradient;
		double gradients[6];
		pxs.SelectSource(noGradient.meanAmp, noGradient.emissionDistance, noGradient.geoTime, coord.source_);
		
		// Form random time bins
		std::vector<double> time_edges;
		for (int i=0; i < 100; i++)
			time_edges.push_back(noGradient.geoTime + rng.Uniform(1e4, 1e4));
		std::sort(time_edges.begin(), time_edges.end());
		noGradient.quantiles.resize(time_edges.size()-1);
		withGradient.quantiles.resize(time_edges.size()-1);
		
		pxs.GetProbabilityQuantiles(&time_edges[0], 0,
		    &noGradient.quantiles[0], time_edges.size()-1);
		
		// Prevent any helpful caching behind the scenes
		pxs.SelectModuleCoordinates(coord.x_+1, coord.y_, coord.z_);
		pxs.SelectModuleCoordinates(coord.x_, coord.y_, coord.z_);
		pxs.SelectSource(withGradient.meanAmp, gradients, withGradient.emissionDistance, withGradient.geoTime, coord.source_);
		double qgradients[time_edges.size()-1][6];
		pxs.GetProbabilityQuantiles(&time_edges[0], 0,
		    &withGradient.quantiles[0], qgradients, time_edges.size()-1);
		
		ENSURE_EQUAL(noGradient.meanAmp, withGradient.meanAmp,
		    "Amplitude gradient does not affect the primary value");
		ENSURE_EQUAL(noGradient.emissionDistance, withGradient.emissionDistance);
		ENSURE_EQUAL(noGradient.geoTime, withGradient.geoTime);
		
		for (unsigned i=0; i < time_edges.size()-1; i++)
			ENSURE_EQUAL(noGradient.quantiles[i], withGradient.quantiles[i],
			    "Quantile gradient does not affect the primary value");
		
	}
}

TEST(GradientEvaluation)
{
	TableSet tables = get_splinetables();
	
	I3RandomServicePtr rng(new I3GSLRandomService(42));
	I3PhotoSplineService pxs(tables.abs.string(), tables.prob.string(), 0.0);
	
	for (int i=0; i < 10; i++) {
		PhotonicsSource source = CoordBundle::randomSource(*rng);
		std::vector<CoordBundle> coords;
		for (int j=0; j < 100; j++)
			coords.push_back(CoordBundle::random(*rng, source));
		verifyEvaluate(coords, *rng, pxs);
	}
}

#endif /* USE_PHOTOSPLINE */

