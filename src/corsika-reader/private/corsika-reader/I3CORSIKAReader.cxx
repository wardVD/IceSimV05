/**
 *  $Id: I3CORSIKAReader.cxx 141942 2016-02-16 17:26:43Z hpandya $
 *  
 *  Copyright (C) 2012
 *  The IceCube Collaboration <http://www.icecube.wisc.edu>
 *  
 */

#include <icetray/open.h>
#include <icetray/I3Frame.h>
#include <icetray/I3Module.h>
#include <dataio/I3FileStager.h>

#include <dataclasses/physics/I3MCTreeUtils.h>
#include <dataclasses/physics/I3Particle.h>
#include <dataclasses/I3Double.h>
#include <phys-services/I3RandomService.h>
#include <phys-services/surfaces/Cylinder.h>

#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/device/file.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/assign.hpp>
#include <boost/python.hpp>
#include <boost/scoped_ptr.hpp>

#include <cmath>

const double EarthRadius = 637131500*I3Units::cm; // as in CORSIKA

typedef I3Map<I3ParticleID, double> I3ParticleIDMap;
typedef boost::shared_ptr<I3ParticleIDMap> I3ParticleIDMapPtr;

class I3CORSIKAReader : public I3Module
{
  public:
	I3CORSIKAReader(const I3Context& context);
	void Configure();
	void Process();

  private:
	SET_LOGGER("I3CorsikaReader");

	int32_t CorsikaToPDG(int corsika_id);
	int FillFrameFromCORSIKA(I3FramePtr frame);
	int ReadNextBlock(float corsika_block[274], char type[5]);
	void ProcessEventHeader(float *corsika_block, I3FramePtr frame,
	    I3MCTreePtr mctree, I3Particle &primary, double core_displace[2]);
	void ProcessParticleBlock(float *corsika_block, I3FramePtr frame,
	    std::vector<I3Particle> &daughters,
	    I3MCTreePtr mctree, I3ParticleIDMapPtr weights,
	    const I3Particle &primary, double core_displace[2]);

	I3RandomServicePtr rng_;
	I3FileStagerPtr file_stager_;
	I3::dataio::shared_filehandle current_filename_;
	boost::iostreams::filtering_istream i3_ifs_;
	boost::iostreams::filtering_istream corsika_ifs_;
	std::vector<std::string> filenames_;
	std::vector<std::string> files_;
	std::vector<std::string>::const_iterator filenames_iter_;
	std::vector<I3Particle::ParticleType> particles_to_write_;
	std::string weightdictname_; 
	int nevents_;
	int oversampling_;
	bool runComplete_;
	bool checkOnly_;
	unsigned lastEventID_;
	I3Surfaces::Cylinder surface_;
	I3MapStringDouble weightMap_;
	bool volumeCorr_;

	int run_number_;
	double obs_level_;
	bool padded_;
	bool curved_, curved_obs_;
	int sub_blocks_;
	
	bool thinned_;
     bool ehistory_;
	
	// Book-keeping for extended history
	I3Particle mother_, grandmother_;
	int ancestors_;
	
};

I3_MODULE(I3CORSIKAReader);

I3CORSIKAReader::I3CORSIKAReader(const I3Context& context)
    : I3Module(context), surface_(1600*I3Units::m, 800*I3Units::m)
{

	AddParameter("FilenameList", "Paths to CORSIKA DAT files", filenames_);
	AddParameter("Prefix", "Path to I3 file with frames to prefix to "
	    "the stream (e.g. GCD information)", "");
	AddParameter("NEvents", "Number of CORSIKA showers per input file, "
	    "required for older CORSIKA versions that do not write this into "
	    "the run number. If value is wrong, will fail (with a useful "
	    "message) at run time. Negative values disable weight checking. ",
	    0);
	AddParameter("OverSampling", "number of times to oversample showers ",1);
	AddParameter("CylinderHeight", "Height of detector cylinder",
	    surface_.GetLength());
	AddParameter("CylinderRadius", "Radius of detector cylinder",
	    surface_.GetRadius());
	AddParameter("ZenithBias", "True if CORSIKA was compiled with the "
	    "VOLUMECORR option, False if the VOLUMEDET option was used. The "
	    "default zenith bias (proportional to sin(theta) for surface "
	    "detectors) is not supported.", true);
	particles_to_write_.push_back(I3Particle::MuMinus);
	particles_to_write_.push_back(I3Particle::MuPlus);
	particles_to_write_.push_back(I3Particle::NuE);
	particles_to_write_.push_back(I3Particle::NuEBar);
	particles_to_write_.push_back(I3Particle::NuMu);
	particles_to_write_.push_back(I3Particle::NuMuBar);
	particles_to_write_.push_back(I3Particle::NuTau);
	particles_to_write_.push_back(I3Particle::NuTauBar);
	AddParameter("ParticlesToWrite", "Types of particle to include in the "
	    "output MC tree. If blank, include all particles present in the "
	    "shower at ground level (this can be very large)",
	    particles_to_write_);
	AddParameter("WeightMapName", "Name of weights map to store in frame",
	    "CorsikaWeightMap");
	AddParameter("CheckIntegrity", "Check that each CORSIKA file is "
	    "complete and contains the expected number of showers, but emit no "
	    "frames.", false);
	AddParameter("Thinned", "Read thinned CORSIKA files", false);
	AddParameter("EHistory", "Read EHISTORY CORSIKA files."
             "For proper rotation of Direction/Position"
             "This Parameter must be passed.", false);

	AddOutBox("OutBox");
}

void I3CORSIKAReader::Configure()
{
	std::string filename;
	
	file_stager_ = context_.Get<I3FileStagerPtr>();
	if (!file_stager_)
		file_stager_ = I3TrivialFileStager::create();

	GetParameter("OverSampling", oversampling_);
	GetParameter("FilenameList", filenames_);

	GetParameter("Prefix", filename);
	if (filename.length() > 0)
		file_stager_->WillReadLater(filename);

	// Oversampling by reading input files multiple times
	for ( int f_i = 0; f_i < oversampling_ ; f_i++)
	{
	    for ( filenames_iter_ = filenames_.begin(); filenames_iter_ != filenames_.end(); filenames_iter_++)
		files_.push_back(*filenames_iter_);
	}
	filenames_iter_ = files_.begin();
	for ( ; filenames_iter_ != files_.end(); filenames_iter_++)
		file_stager_->WillReadLater(*filenames_iter_);
	filenames_iter_ = files_.begin();
	GetParameter("NEvents", nevents_);
	if (filename.length() > 0) {
		log_info("Prefixing with filename %s", filename.c_str());
		current_filename_ = file_stager_->GetReadablePath(filename);
		I3::dataio::open(i3_ifs_, *current_filename_);
	}
	GetParameter("ParticlesToWrite", particles_to_write_);
	double length, radius;
	GetParameter("CylinderHeight", length);
	GetParameter("CylinderRadius", radius);
	surface_ = I3Surfaces::Cylinder(length, radius);
	GetParameter("ZenithBias", volumeCorr_);
	GetParameter("WeightMapName", weightdictname_ );

	GetParameter("CheckIntegrity", checkOnly_);
	GetParameter("Thinned", thinned_);
	GetParameter("Ehistory", ehistory_);

	rng_ = context_.Get<I3RandomServicePtr>();
	if (rng_ == NULL)
		log_fatal("No I3RandomService in context!");
	
	ancestors_ = 0;
	runComplete_ = true;
	lastEventID_ = 0;
}

void I3CORSIKAReader::Process()
{
	log_trace("%s: %s", GetName().c_str(), __PRETTY_FUNCTION__);

	I3FramePtr frame(new I3Frame(I3Frame::DAQ));
	if (!i3_ifs_.empty()) {
		// Get frame from prefix file
		frame->load(i3_ifs_);
		if (i3_ifs_.peek() == EOF)
			i3_ifs_.reset();
		PushFrame(frame);
		return;
	}

	if (FillFrameFromCORSIKA(frame) != 0)
		return;

	PushFrame(frame);
}

typedef std::map<int, int> CORSIKAtoPDGMap_t;

static const CORSIKAtoPDGMap_t CORSIKAtoPDGMap = boost::assign::list_of<std::pair<int, int> >
	(1,	22)	// Gamma
	(2,	-11)	// e+
	(3,	11)	// e-
	(5,	-13)	// mu-
	(6,	13)	// mu+
	(7,	111)	// pi_0
	(8,	211)	// pi+
	(9,	-211)	// pi-
	(10,	130)	// K_L
	(11,	321)	// K+
	(12,	-321)	// K-
	(13,	2112)	// neutron
	(14,	2212)	// proton
	(15,	-2212)	// anti-proton
	(16,	310)	// K_S
	(17,	221)	// eta
	(18,	3122)	// lambda
	(19,	3222)	// sigma+
	(20,	3212)	// sigma 0
	(21,	3112)	// sigma-
	(22,	3322)	// xi 0
	(23,	3312)	// xi-
	(24,	3334)	// omega-
	(25,	-2112)	// anti-neutron
	(26,	-3122)	// lambda-bar
	(27,	-3112)	// sigma- bar
	(28,	-3212)	// sigma 0 bar
	(29,	-3222)	// sigma+ bar
	(30,	-3322)	// xi 0 bar
	(31,	-3312)	// xi+
	(32,	-3334)	// omega+
	(50,	223)	// small omega
	(51,	113)	// rho 0
	(52,	213)	// rho-plus
	(53,	-213)	// rho-minus
	(54,	2224)	// delta ++
	(55,	2214)	// delta +
	(56,	2114)	// delta 0
	(57,	1114)	// delta -
	(58,	-2224)	// delta ++ bar
	(59,	-2214)	// delta + bar
	(60,	-2114)	// delta 0 bar
	(61,	-1114)	// delta - bar
	(62,	313)	// k*0 (892)
	(63,	323)	// k*+ (892)
	(64,	-323)	// k*- (892)
	(65,	-313)	// k*0 bar (892)
	(66,	12)	// nu_e
	(67,	-12)	// anti-nu_e
	(68,	14)	// nu_mu
	(69,	-14)	// anti-nu_mu
	// 71-76 are special CORSIKA interaction tracking
     // Had to add these extra entries to map to accommodate EHISTORY showers.
	(71,	221)	// eta
	(72,	221)	// eta
	(73,	221)	// eta
	(74,	221)	// eta
	(116,	421)	// D0
	(117,	411)	// D+
	(118,	-411)	// D-
	(119,	-421)	// D0 bar
	(120,	431)	// D_s +
	(121,	-431)	// D_s -
	(122,	441)	// eta_c
	(123,	423)	// D*0 (2007)
	(124,	413)	// D*+ (2010)
	(125,	-413)	// D*- (2010)
	(126,	-423)	// D*0 bar (2007)
	(127,	433)	// D*s+
	(128,	-433)	// D*s-
	(130,	443)	// J/psi
	(131,	-15)	// tau+
	(132,	15)	// tau-
	(133,	16)	// nu_tau
	(134,	-16)	// anti-nu_tau
	(137,	4122)	// lambda_c+
	(138,	4232)	// xi_c+
	(139,	4132)	// xi_c0
	(140,	4222)	// sigma_c++
	(141,	4212)	// sigma_c+
	(142,	4112)	// sigma_c0
	(143,	4322)	// xi_c prime +
	(144,	4312)	// xi_c prime 0
	(145,	4332)	// omega_c 0
	(149,	-4122)	// lambda_c-
	(150,	-4232)	// xi_c-
	(151,	-4132)	// xi_c0 bar
	(152,	-4222)	// sigma_c--
	(153,	-4212)	// sigma_c-
	(154,	-4112)	// sigma_c0 bar
	(155,	-4322)	// xi_c prime -
	(156,	-4312)	// xi_c prime 0 bar
	(157,	-4332)	// omega_c 0 bar
	(161,	4224)	// sigma_c * ++
	(162,	4214)	// sigma_c * +
	(163,	4114)	// sigma_c * 0
	(171,	-4224)	// sigma_c * --
	(172,	-4214)	// sigma_c * -
	(173,	-4114)	// sigma_c * 0 bar
	(176,	511)	//B 0
	(179,	-511)	//B 0 bar
	(177,	521)	//B +
	(178,	-521)	//B -
	(180,	531)	//B_s 0
	(181,	-531)	//B_s 0 bar
	(182,	541)	//B_c +
	(183,	-541)	//B_c -
	(184,	5122)	//lambda_b 0
	(185,	5112)	//sigma_b -
	(186,	5222)	//sigma_b +
	(187,	5232)	//xi_b 0
	(188,	5132)	//xi_b -
	(189,	5332)	//omega_b -
	(190,	-5122)	//lambda_b 0 bar
	(191,	-5112)	//sigma_b + bar
	(192,	-5222)	//sigma_b - bar
	(193,	-5232)	//xi_b 0 bar
	(194,	-5132)	//xi_b +
	(195,	-5332)	//omega_b +
;

int32_t
I3CORSIKAReader::CorsikaToPDG(int corsika_id)
{
	// Some event generators (e.g. DPMJET) emit history entries
	// with PDG codes that CORSIKA's numbering scheme can't
	// accomodate (e.g. phi mesons).
	if (corsika_id == 9999)
		return I3Particle::unknown;
	if (corsika_id >= 200)
		return 1000000000 + /* PDG Nucleus code */
		    10000 * (corsika_id % 100) + /* A */
		    10 * (corsika_id / 100); /* Z */
	
	CORSIKAtoPDGMap_t::const_iterator i;
	i = CORSIKAtoPDGMap.find(corsika_id);
	if (i == CORSIKAtoPDGMap.end())
		log_fatal("Unknown CORSIKA ID %d", corsika_id);

	return i->second;
}

// Distance to the surface
double
GetSlantDepth(const I3Direction &dir, const I3Position &pos)
{
	double d = I3Constants::SurfaceElev-I3Constants::OriginElev-pos.GetZ();
	double r = EarthRadius;
	double ct = cos(dir.GetZenith());
	return sqrt(2*r*d + ct*ct*(r-d)*(r-d) - d*d) - (r-d)*ct;
}

void
I3CORSIKAReader::ProcessEventHeader(float *corsika_block, I3FramePtr frame,
    I3MCTreePtr mctree, I3Particle &primary, double core_displace[2])
{
	curved_obs_ = (corsika_block[168] != 0);
	curved_ = (corsika_block[79] != 0);

	primary = I3Particle();
	primary.SetShape(I3Particle::Primary);
	primary.SetLocationType(I3Particle::Anywhere);

	primary.SetPdgEncoding(CorsikaToPDG(corsika_block[3]));
	primary.SetShape(I3Particle::Primary);
	I3Direction dir(corsika_block[8], corsika_block[9],
	    -corsika_block[10]);

     if (ehistory_){
         log_info("EHISTORY: Rotating Primary Direction by 119 deg i.e. std ARRANG value.");
         //ARRANG keyword conflicts with EHISTORY option. 
         //Hence rotation of Primary is being done in the corsika-reader.
         //There is no need to apply rotation matrix on Primary position because it is derived using Primary Dir
         dir.RotateZ(-1.0*(2.0*I3Constants::pi - 2.076942)*I3Units::rad);// -1*(2 pi - 119 degrees).
     } else {
         dir.RotateZ(-corsika_block[93]*I3Units::rad);
     }

	primary.SetDir(dir);
	primary.SetEnergy(corsika_block[4]*I3Units::GeV);
	
	log_debug_stream(primary.GetEnergy() << " GeV " << primary.GetTypeString());

	// Randomly shift shower core around
	I3Position shower_intersection =
	    surface_.SampleImpactPosition(primary.GetDir(), *rng_);
	
	double xspeed, yspeed, zspeed;
	xspeed = primary.GetSpeed()*sin(primary.GetZenith() / I3Units::radian)*
	    cos(primary.GetAzimuth() / I3Units::radian);
	yspeed = primary.GetSpeed()*sin(primary.GetZenith() / I3Units::radian)*
	    sin(primary.GetAzimuth() / I3Units::radian);
	zspeed = primary.GetSpeed()*cos(primary.GetZenith() / I3Units::radian);

	// Time 0 is when the particle intersects the observation level
	double time_to_intersection;
	if (curved_) {
		double entry_altitude = EarthRadius +
		    corsika_block[158]*I3Units::cm;
		double detection_altitude = EarthRadius +
		    obs_level_ + I3Constants::OriginElev;
		double detection_angle = primary.GetZenith();

		double flight_distance =
		    -detection_altitude*cos(detection_angle) +
		    sqrt(pow(entry_altitude, 2) -
		    pow(detection_altitude*sin(detection_angle), 2));
		primary.SetTime(-flight_distance/primary.GetSpeed());
		time_to_intersection = primary.GetTime() - GetSlantDepth(
		    primary.GetDir(), shower_intersection)/primary.GetSpeed();
	} else {
		primary.SetTime(-(corsika_block[158]*I3Units::cm -
		    I3Constants::SurfaceElev)/zspeed);
		time_to_intersection = primary.GetTime() -
		    (I3Constants::SurfaceElev - I3Constants::OriginElev
		    - shower_intersection.GetZ())/zspeed;
	}

	primary.SetPos(
             shower_intersection.GetX() - time_to_intersection*xspeed,
             shower_intersection.GetY() - time_to_intersection*yspeed,
             shower_intersection.GetZ() - time_to_intersection*zspeed
             ); 

	I3Position surface_pos = primary.ShiftAlongTrack(
	    -primary.GetTime()*primary.GetSpeed());
	core_displace[0] = surface_pos.GetX();
	core_displace[1] = surface_pos.GetY();

	double interaction_height = corsika_block[7]*I3Units::cm;
	if (interaction_height < 0) {
		// TMARGIN option set, times relative to entrance into the
		// atmosphere
		interaction_height = -interaction_height;
	}
	
	I3DoublePtr height(new I3Double(interaction_height));
	frame->Put("CorsikaInteractionHeight", height);

	double time_to_interaction =
	    (corsika_block[158]*I3Units::cm - interaction_height)/zspeed;
	primary.SetLength(primary.GetSpeed()*time_to_interaction);

	I3MCTreeUtils::AddPrimary(*mctree, primary);

	if (weightMap_.size() == 0) {
		weightMap_["EnergyPrimaryMin"] = corsika_block[59]*I3Units::GeV;
		weightMap_["EnergyPrimaryMax"] = corsika_block[60]*I3Units::GeV;
		weightMap_["PrimarySpectralIndex"] = corsika_block[58];
		weightMap_["NEvents"] = nevents_*oversampling_;
		// In VOLUMEDET mode, the angular density of injected showers is
		// uniform, but the projected area of the target cylinder is not,
		// so diagonal showers have to be spread out over a larger area than
		// vertical or horizontal showers, effectively reducing the simulated
		// livetime at middling zenith angles. In VOLUMECORR mode, however,
		// the angular density of injected showers is already proportional to
		// the target's projected area, so the average shower sees the same
		// projected area regardless of zenith angle.
		double thetaMin = corsika_block[81]*I3Units::deg;
		double thetaMax = corsika_block[82]*I3Units::deg;
		weightMap_["ThetaMin"] = thetaMin;
		weightMap_["ThetaMax"] = thetaMax;
		weightMap_["AreaSum"] =
		    surface_.GetAcceptance(cos(thetaMax), cos(thetaMin));
		weightMap_["CylinderRadius"] = surface_.GetRadius();
		weightMap_["CylinderLength"] = surface_.GetLength();
	}
	I3MapStringDoublePtr weightdict(new I3MapStringDouble(weightMap_));
	if (!volumeCorr_) {
		double thetaMin = corsika_block[81]*I3Units::deg;
		double thetaMax = corsika_block[82]*I3Units::deg;
		(*weightdict)["AreaSum"] = 2*M_PI*
		    surface_.GetArea(primary.GetDir())
		    /(cos(thetaMin)-cos(thetaMax));
	}
	(*weightdict)["PrimaryEnergy"] = primary.GetEnergy();
	(*weightdict)["PrimaryType"] = (double)primary.GetType();
	(*weightdict)["OverSampling"] = (double)oversampling_;

	frame->Put(weightdictname_, weightdict);
}

inline bool
IsEquivalent(double p1, double p2)
{
	return (p1 == p2 || (std::isnan(p1) && std::isnan(p2)));
}

inline bool
IsEquivalent(const I3Position &p1, const I3Position &p2)
{
	return (IsEquivalent(p1.GetX(),p2.GetX()) &&
	    IsEquivalent(p1.GetY(),p2.GetY()) &&
	    IsEquivalent(p1.GetZ(),p2.GetZ()));
}

inline bool
IsEquivalent(const I3Particle &p1, const I3Particle &p2)
{
	return (p1.GetID() == p2.GetID()) ||
	   (p1.GetType() == p2.GetType() &&
	    p1.GetEnergy() == p2.GetEnergy() &&
	    p1.GetDir() == p2.GetDir() &&
	    IsEquivalent(p1.GetPos(), p2.GetPos())
	);
}

I3MCTree::iterator
EnsureNode(I3MCTreePtr mctree, I3MCTree::iterator root, I3Particle &child)
{
    // Check if a particle is a daughter. If not, add it.
    // Use IsEquivalent for testing particle equality because CORSIKA
	if (IsEquivalent(*root, child))
		return root;
	I3MCTree::sibling_iterator i = mctree->end_sibling(), p_iter = mctree->end_sibling();
	for (i = mctree->children(root); i != mctree->end_sibling(); i++)
		if (IsEquivalent(*i, child)) {
			p_iter = i;
			break;
		}
	if (p_iter != mctree->end_sibling())
		return p_iter;
	else
		return mctree->append_child(root, child);
}

void
I3CORSIKAReader::ProcessParticleBlock(float *corsika_block, I3FramePtr frame,
    std::vector<I3Particle> &daughters,
    I3MCTreePtr mctree, I3ParticleIDMapPtr weights, const I3Particle &primary,
    double core_displace[2])
{
	for (int i = 0; i < 39; i++) {
		int start = (7+thinned_)*i;
		int id = corsika_block[start + 1]/1000;
		if (id == 0)
			break;

		// Skip extra muon info
		if (id == 75 || id == 76)
			continue;
		
		// Extended-history entries have negative particle IDs
		// http://digbib.ubka.uni-karlsruhe.de/volltexte/documents/2545953
		if (id < 0)
			ancestors_++;

		I3Particle part;
		if (id > 0) {
			part.SetShape(I3Particle::StartingTrack);
			part.SetPdgEncoding(CorsikaToPDG(id));
			part.SetLocationType(I3Particle::InIce);
		} else {
			part.SetShape(I3Particle::Null);
			part.SetPdgEncoding(CorsikaToPDG(-id));
			part.SetLocationType(I3Particle::Anywhere);
		}

          I3Direction dir(corsika_block[start + 2],
                  corsika_block[start + 3],
                  -corsika_block[start + 4]);

          if (ehistory_){
             //ARRANG keyword conflicts with EHISTORY option. 
             //Hence rotation of Primary is being done in the corsika-reader.
              dir.RotateZ(-1.0*(2.0*I3Constants::pi - 2.076942)*I3Units::rad);
          }

		part.SetDir(dir);

          I3Position temp_pos(corsika_block[start+5]*I3Units::cm,
                  corsika_block[start+6]*I3Units::cm, obs_level_);
          if (ehistory_){
              //ARRANG keyword conflicts with EHISTORY option. 
              //Hence rotation of Primary is being done by hand here.
              temp_pos.RotateZ(-1.0*(2.0*I3Constants::pi - 2.076942)*I3Units::rad);
          }
          
		if (curved_obs_) {
			double theta, phi, D;
			// Normal particles are recorded at the observation
			// level, extended-history entries at production level
			D = EarthRadius + I3Constants::OriginElev +
			    (id > 0 ?
			    obs_level_ : corsika_block[start+7]*I3Units::cm);
			theta = hypot(
			  temp_pos.GetX() + core_displace[0],
			  temp_pos.GetY() + core_displace[1]
			  )/D;
			phi = atan2(temp_pos.GetY() + core_displace[1],
			  temp_pos.GetX() +core_displace[0]);
			// Only the mother particle has a position; the
			// grandmother only has a column depth.
			if (id > 0 || ancestors_ == 1)
				part.SetPos(
				    D*sin(theta)*cos(phi),
				    D*sin(theta)*sin(phi),
				    D*cos(theta) - EarthRadius -
				    I3Constants::OriginElev
				);
		} else if (id > 0 || ancestors_ == 1) {
			part.SetPos(temp_pos.GetX() + core_displace[0],
			  temp_pos.GetY() + core_displace[1],
			  obs_level_);
		}
		// Normal entries have an explicit time, but extended-history
		// entries do not. Assume speed-of-light propagation from the
		// primary vertex.
		if (id > 0)
			part.SetTime(corsika_block[start + 7]*I3Units::ns +
			    primary.GetTime());
		else
			part.SetTime(primary.GetTime() +
			    (part.GetPos()-primary.GetPos()).Magnitude()/
			    I3Constants::c);
		
		// Calculate energy assuming E=cp
		part.SetEnergy(I3Units::GeV*sqrt(
		    pow(corsika_block[start + 2], 2) + 
		    pow(corsika_block[start + 3], 2) +
		    pow(corsika_block[start + 4], 2)));

		if (id < 0) {
			// De-duplicate references to the primary that ignore
			// the rest mass in the energy calculation
			if (std::abs(part.GetEnergy()-primary.GetEnergy()) < 1)
				part = primary;
			if (ancestors_ == 1) {
				mother_ = part;
			}	
			else if (ancestors_ == 2)
				grandmother_ = part;
			continue;
		}

		// Check against list
		if (particles_to_write_.size() > 0 && 
		    std::find(particles_to_write_.begin(),
		    particles_to_write_.end(), part.GetType()) ==
		    particles_to_write_.end()) {
			ancestors_ = 0;
			continue;
		}
		
		// Record thinning weight if it exists and is non-trivial
		if (thinned_ && (corsika_block[start + 8] != 1.f))
			(*weights)[part.GetID()] = corsika_block[start + 8];
		
		// No extended history: defer tree insertion for speed.
		if (ancestors_ == 0) {
			daughters.push_back(part);
		} else if (ancestors_ == 2) {
			// Write extended history if we have it
			// NB: occasionally you will see nu_mu with eta as a parent,
			// or nu_e with a pion as a parent, even though such processes
			// decays either forbidden or highly suppressed. These actually
			// come from the decay of an intermediate muon, which is does
			// not appear since the history deals exclusively with
			// hadron-hadron and photonuclear interactions.
			I3MCTree::iterator node = mctree->begin();
			node = EnsureNode(mctree, node, grandmother_);
			node = EnsureNode(mctree, node, mother_);
			mctree->append_child(node, part);
		} else {
			log_fatal_stream("This particle claims to have " << ancestors_ <<
			    " ancestors. This can't be.");
		}
		
		ancestors_ = 0;
	}
}

int
I3CORSIKAReader::FillFrameFromCORSIKA(I3FramePtr frame)
{
	BOOST_STATIC_ASSERT(sizeof(float) == 4);
	float corsika_block[313];
	char type[5] = "";

	// NB: In the normal mode of operation, CORSIKA gives us just the particles
	// at ground level. Since the cost of each call to I3MCTree::append_child()
	// is proportional to the number of particles already in the tree, it is
	// much faster (500x for 100k particles) to keep an intermediate vector of
	// ground-level particles and attach them all to the tree at once with
	// I3MCTree::append_children(). In extended-history mode each ground-level
	// particle record includes its parent and grandparent particles. Since
	// a parent may have multiple daughters the entries must be de-duplicated
	// as we go anyhow, and we fall back to inserting the entries one by one.
	I3MCTreePtr mctree(new I3MCTree);
	std::vector<I3Particle> daughters;
	I3ParticleIDMapPtr weights(new I3ParticleIDMap);
	I3Particle primary;
	double core_displacement[2];
	
	while (1) {
		if (ReadNextBlock(corsika_block, type) != 0)
			return 1;

		if (strcmp(type, "RUNH") == 0) {
			runComplete_ = false;
			run_number_ = corsika_block[2];
			if (corsika_block[93] > 0)
				nevents_ = corsika_block[93];
			if (corsika_block[5] > 1)
				log_fatal("Only one observation level "
				    "supported. This file has %d.\n",
				    (int)corsika_block[5]);
			obs_level_ = corsika_block[6]*I3Units::cm -
			    I3Constants::OriginElev;
			weightMap_.clear();
		} else if (strcmp(type, "RUNE") == 0) {
			runComplete_ = true;
			if (nevents_ >= 0 && corsika_block[3] != nevents_)
				log_fatal("Number of events in this file (%d) "
				    "does not match configured number (%d). As "
				    "a result, all weight entries written to "
				    "disk were lies. Please correct and try "
				    "again.", int(corsika_block[3]), nevents_);
		} else if (checkOnly_)  {
			if (strcmp(type, "EVTH") == 0)
				lastEventID_ = corsika_block[2];
			return 1;
		} else if (strcmp(type, "EVTH") == 0) {
			ProcessEventHeader(corsika_block, frame, mctree,
			    primary, core_displacement);
		} else if (strcmp(type, "EVTE") == 0) {
			if (weights->size() > 0)
				frame->Put("CorsikaParticleWeights", weights);
			// Add a block of ground-level particles if we have it
			mctree->append_children(primary, daughters);
			frame->Put("I3MCTree", mctree);
			return 0;
		} else if (strcmp(type, "LONG") == 0) {
			continue;
		} else if (strcmp(type, "RUNE") == 0) {
			runComplete_ = true;
			if (corsika_block[3] != nevents_)
				log_fatal("Number of events in this file (%d) "
				    "does not match configured number (%d). As "
				    "a result, all weight entries written to "
				    "disk were lies. Please correct and try "
				    "again.", int(corsika_block[3]), nevents_);
			return 1;
		} else {
			ProcessParticleBlock(corsika_block, frame, daughters, mctree,
			    weights, primary, core_displacement);
		}
	}
}

int
I3CORSIKAReader::ReadNextBlock(float corsika_block[313], char type[5])
{
	unsigned offset = 0;
	if (corsika_ifs_.peek() == EOF) {
		if (!runComplete_ && filenames_iter_ != files_.begin())
			log_fatal("%s is truncated (event %u did not end)!",
			    (filenames_iter_-1)->c_str(), lastEventID_);
		if (filenames_iter_ == files_.end()) {
			RequestSuspension();
			return EOF;
		}
		if (!corsika_ifs_.empty())
			corsika_ifs_.pop();
		corsika_ifs_.reset();
		assert(corsika_ifs_.empty());

		current_filename_ = file_stager_->GetReadablePath(*filenames_iter_);
		filenames_iter_++;
		I3::dataio::open(corsika_ifs_, *current_filename_);
		// in an unpadded file, the first 4 bytes are "RUNH"
		corsika_ifs_.read((char *)&corsika_block[1], 4);
		padded_ = (memcmp((char *)&corsika_block[1], "RUNH", 4) != 0);
		// we already have the first 4 bytes of the block
		if (!padded_)
			offset = 1;
		sub_blocks_ = 0;
		
		log_info_stream("Opened "<<(thinned_ ? "thinned " : "")<<"CORSIKA file "<<(*current_filename_));
	}

	// NOTE: CORSIKA files have no defined endianness, which is delightful,
	// so just assume that the endianness of this system matches the
	// endianness of the system that produced the file. 
	corsika_ifs_.read((char *)(&corsika_block[1+offset]),
	    ((thinned_ ? 312 : 273)-offset)*sizeof(float));
	memcpy(type, &corsika_block[1], 4);
	type[4] = 0;
	if (++sub_blocks_ % 21 == 0 && padded_) {
		char junk[8];
		corsika_ifs_.read((char *)&junk, 8);
	}

	return 0;
}
