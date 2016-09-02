/**
 *  $Id: I3PhotonicsHitMaker.cxx 129433 2015-02-23 21:43:55Z nega $
 *
 *  Copyright (C) 2011
 *  Nathan Whitehorn <nwhitehorn@icecube.wisc.edu>
 *  and the IceCube Collaboration <http://www.icecube.wisc.edu>
 *
 */

#include <photonics-service/I3PhotonicsService.h>
#include <phys-services/I3RandomService.h>
#include <dataclasses/physics/I3Particle.h>
#include <simclasses/I3MCPE.h>
#include <dataclasses/physics/I3MCTree.h>
#include <dataclasses/geometry/I3Geometry.h>
#include <dataclasses/calibration/I3Calibration.h>
#include <dataclasses/status/I3DetectorStatus.h>

#include <icetray/I3Module.h>
#include <boost/foreach.hpp>

using namespace std; // Get C99 math functions back

class I3PhotonicsHitMaker : public I3Module
{
public:
	I3PhotonicsHitMaker(const I3Context& context);
	void Configure();
	void DAQ(I3FramePtr frame);
private:
	SET_LOGGER("I3PhotonicsHitMaker");

	I3PhotonicsServicePtr trackp_, cascadep_;
	I3RandomServicePtr rng_;
	std::string hcList_;
	std::string input_, output_;
	double unshadowed_;
	double binwidth_;

	std::vector<I3Particle::ParticleType> light_producing_tracks;
	std::vector<I3Particle::ParticleType> hadronic_types;
};

I3_MODULE(I3PhotonicsHitMaker);

I3PhotonicsHitMaker::I3PhotonicsHitMaker(const I3Context &context) :
    I3Module(context)
{
	AddOutBox("OutBox");

	AddParameter("CascadeService", "Photonics service to use for cascades",
	    I3DefaultName<I3PhotonicsService>::value());
	AddParameter("TrackService", "Photonics service to use for tracks",
	    I3DefaultName<I3PhotonicsService>::value());
	AddParameter("UnshadowedFraction", "Fraction of photocathode "
	    "available to receive light (e.g. unshadowed by the cable)",
	    0.99);
	AddParameter("BinWidth", "Join photons from a single particle within "
	    "BinWidth ns into a single MC hit PE", 0.2);
	AddParameter("HCList", "Name for list of light-producing particles. "
	    "If set to the empty string (default), no list will be written.",
	    "");
	AddParameter("Input", "Name of input particles in frame",
	    "I3MCTree");
	AddParameter("Output", "Name of output in frame",
	    "I3MCPESeriesMap");
	AddParameter("RandomService",
	    "A random number generating service (derived from I3RandomService)."
	    " The default is to get it from the context.",
	    rng_);

	// Hard coded list of track-type particles that produce light
	//
	// If std::vectors were less painful to initialize statically, this
	// would be a static global

	light_producing_tracks.push_back(I3Particle::MuMinus);
	light_producing_tracks.push_back(I3Particle::MuPlus);
	light_producing_tracks.push_back(I3Particle::TauMinus);
	light_producing_tracks.push_back(I3Particle::TauPlus);
	light_producing_tracks.push_back(I3Particle::STauMinus);
	light_producing_tracks.push_back(I3Particle::STauPlus);
	light_producing_tracks.push_back(I3Particle::Monopole);

	// List of particle types that need the hadronic correction factor
	hadronic_types.push_back(I3Particle::Hadrons);
	hadronic_types.push_back(I3Particle::PiPlus);
	hadronic_types.push_back(I3Particle::PiMinus);
	hadronic_types.push_back(I3Particle::NuclInt);
}

void
I3PhotonicsHitMaker::Configure()
{
	GetParameter("CascadeService", cascadep_);
	GetParameter("TrackService", trackp_);
	GetParameter("BinWidth", binwidth_);
	GetParameter("UnshadowedFraction", unshadowed_);
	GetParameter("HCList", hcList_);
	GetParameter("Input", input_);
	GetParameter("Output", output_);

	if ((!cascadep_) && (!trackp_))
		log_fatal("You need to set at least one of the "
		    "CascadeService/TrackService parameters.");

	GetParameter("RandomService", rng_);
	if (!rng_) rng_ = context_.Get<I3RandomServicePtr>();
	if (!rng_)
		log_fatal("No random service available. Please add one.");
}

struct photo_source {
	PhotonicsSource source;
	I3PhotonicsServicePtr service;
	I3Particle *particle;
};

static inline bool
operator <(const I3MCPE &a, const I3MCPE &b)
{
	return a.time < b.time;
}

void
I3PhotonicsHitMaker::DAQ(I3FramePtr frame)
{
	I3MCTreeConstPtr mctree = frame->Get<I3MCTreeConstPtr>(input_);
	const I3Geometry& geometry = frame->Get<I3Geometry>();
	const I3Calibration& calib = frame->Get<I3Calibration>();
	// const I3DetectorStatus& status = frame->Get<I3DetectorStatus>();

	boost::shared_ptr<I3Vector<I3Particle> > hclist =
	    boost::shared_ptr<I3Vector<I3Particle> >(new I3Vector<I3Particle>);

	I3MCPESeriesMapPtr mchits;
        if(frame->Has(output_)){
          I3MCPESeriesMapConstPtr old_map = frame->Get<I3MCPESeriesMapConstPtr>(output_);
          mchits = I3MCPESeriesMapPtr(new I3MCPESeriesMap(*old_map));
        }else{
          mchits = I3MCPESeriesMapPtr(new I3MCPESeriesMap);
        }
        if(!mchits) log_fatal("Could not create an I3MCPeSeriesMap");

	// Assumes cascade and track services have same range. Assert?
	double lowZ, highZ, lowAng, highAng;
	if (cascadep_) {
		lowZ = cascadep_->GetDepthSelectionLow();
		highZ = cascadep_->GetDepthSelectionHigh();
		lowAng = cascadep_->GetAngularSelectionLow();
		highAng = cascadep_->GetAngularSelectionHigh();
	} else if (trackp_) {
		lowZ = trackp_->GetDepthSelectionLow();
		highZ = trackp_->GetDepthSelectionHigh();
		lowAng = trackp_->GetAngularSelectionLow();
		highAng = trackp_->GetAngularSelectionHigh();
	} else {
		log_fatal("TrackService and CascadeService are NULL. "
		    "Cannot continue.");
	}

	// Step one: collect all light-producing particles and visible energies
	for (I3MCTree::const_iterator i = mctree->begin(); i != mctree->end(); i++) {
		const I3Particle &particle = *i;

		// Do not generate photons for dark particles and particles the
		// are not in ice
		if (particle.GetShape() == I3Particle::Dark) continue;
		if (particle.GetLocationType() != I3Particle::InIce) continue;

		// Check if particle is a cascade or a light-producting track
		if (!(particle.IsCascade() ||
		    (std::find(light_producing_tracks.begin(),
		     light_producing_tracks.end(), particle.GetType())
		     != light_producing_tracks.end() && particle.GetLength() > 0
		    ))) continue;

		// Make sure the particle is in the range of the table
		double pzenith = particle.GetDir().GetZenith()/I3Units::deg;
		if (pzenith > 180) pzenith = 180; // Fix rounding errors
		if (pzenith < lowAng || pzenith > highAng)
			continue;
		if (particle.GetPos().GetZ()/I3Units::m < lowZ ||
		    particle.GetPos().GetZ()/I3Units::m > highZ)
			continue;

		hclist->push_back(particle);
	}

	// Step two: create photonics sources for all light-producing particles
	std::vector<struct photo_source> photo_sources;
	BOOST_FOREACH(I3Particle &src, *hclist) {
		I3PhotonicsServicePtr service;
		if (src.IsCascade())
			service = cascadep_;
		else
			service = trackp_;

		if (!service) {
			log_fatal("There is no I3PhotonicsService "
			    "for particle type %s.",
			    src.GetTypeString().c_str());
		}

		struct photo_source source;
		source.source = PhotonicsSource(src);
		source.service = service;
		source.particle = &src;

		photo_sources.push_back(source);
	}

	// Step 3: Sample photons for sources
	for (I3OMGeoMap::const_iterator i = geometry.omgeo.begin();
	    i != geometry.omgeo.end(); i++) {
		// In-ice only
		if (i->second.omtype == I3OMGeo::IceTop)
			continue;

		I3MCPESeries omhits;
		const I3Position &pos = i->second.position;

		// Relative DOM efficiency
		double light_scale = unshadowed_;
		std::map<OMKey, I3DOMCalibration>::const_iterator domCalib =
		    calib.domCal.find(i->first);
		if (domCalib != calib.domCal.end()) {
			double rde = domCalib->second.GetRelativeDomEff();
			if (isfinite(rde))
				light_scale *= rde;
		}

		if (trackp_)
			trackp_->SelectModuleCoordinates(pos.GetX()/I3Units::m,
			    pos.GetY()/I3Units::m, pos.GetZ()/I3Units::m);
		if (cascadep_)
			cascadep_->SelectModuleCoordinates(
			    pos.GetX()/I3Units::m, pos.GetY()/I3Units::m,
			    pos.GetZ()/I3Units::m);

		BOOST_FOREACH(struct photo_source src, photo_sources) {
			double nphotons, emissiondist, geotime;
			src.service->SelectSource(nphotons, emissiondist,
			    geotime, src.source);
			nphotons *= light_scale;

			nphotons = rng_->Poisson(nphotons);
			if (nphotons == 0)
				continue;

			double *time_delays = new double[int(nphotons)];
			src.service->GetTimeDelays(rng_, time_delays,
			    int(nphotons));
			std::sort(&time_delays[0], &time_delays[int(nphotons)]);

			for (int i = 0, q = 1; i < int(nphotons); i += q) {
				// Get the number of photons in this bin
				for (q = 1; i+q < int(nphotons) &&
				    time_delays[i+q] - time_delays[i] <
				    binwidth_; q++) {}

				// Note: the time delay is the time of the
				// first hit in a bit, if we're using bins,
				// not the mean, to make sure direct photons
				// are maximally direct.
				I3MCPE hit(*src.particle);
				hit.time = src.particle->GetTime() + geotime 
                                  + time_delays[i];				    
				hit.npe = q;
				omhits.push_back(hit);
			}
			delete [] time_delays;
		}

		std::sort(omhits.begin(), omhits.end());

		if (omhits.size() > 0)
			(*mchits)[i->first] = omhits;
	}

	// Write output to frame
	if (hcList_ != "")
		frame->Put(hcList_, hclist);

        if(frame->Has(output_)) frame->Delete(output_);
	frame->Put(output_, mchits);
	PushFrame(frame);
}
