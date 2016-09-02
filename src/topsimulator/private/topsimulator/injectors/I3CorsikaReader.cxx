/**
 * Copyright (C) 2008
 * The IceCube collaboration
 * ID: $Id: $
 *
 * @file I3CorsikaReader.cxx
 * @version $Rev: $
 * @date $Date: $
 * @author Tilo Waldenmaier
 */

#include <topsimulator/injectors/I3CorsikaReader.h>
#include <dataclasses/I3Constants.h>
#include <dataclasses/I3Direction.h>
#include <icetray/open.h>
#include <time.h>
#include <boost/assign.hpp>
#include <inttypes.h>

using namespace std;

I3CorsikaReader::I3CorsikaReader():
  isThinned_(false),
  isCurved_(false),
  particleDataSize_(0)
{
}


I3CorsikaReader::~I3CorsikaReader()
{
    CloseFile();
}


// Code for converting CORSIKA particle types into PDG particle types:
// This typedef and conversion function stolen from the corsika-reader project.
typedef std::map<int, int> CORSIKAtoPDGMap_t;  

static const CORSIKAtoPDGMap_t CORSIKAtoPDGMap = boost::assign::list_of<std::pair<int, int> >
  (1,     22)     // Gamma
  (2,     -11)    // e+
  (3,     11)     // e-
  (5,     -13)    // mu-
  (6,     13)     // mu+
  (7,     111)    // pi_0
  (8,     211)    // pi+
  (9,     -211)   // pi-
  (10,    130)    // K_L
  (11,    321)    // K+
  (12,    -321)   // K-
  (13,    2112)   // neutron
  (14,    2212)   // proton
  (15,    -2212)  // anti-proton
  (16,    310)    // K_S
  (17,    221)    // eta
  (18,    3122)   // lambda
  (19,    3222)   // sigma+
  (20,    3212)   // sigma 0
  (21,    3112)   // sigma-
  (22,    3322)   // xi 0
  (23,    3312)   // xi-
  (24,    3334)   // omega-
  (25,    -2112)  // anti-neutron
  (26,    -3122)  // lambda-bar
  (27,    -3112)  // sigma- bar
  (28,    -3212)  // sigma 0 bar
  (29,    -3222)  // sigma+ bar
  (30,    -3322)  // xi 0 bar
  (31,    -3312)  // xi+
  (32,    -3334)  // omega+
  (50,    223)    // small omega
  (51,    112)    // rho 0
  (52,    213)    // rho-plus
  (53,    -213)   // rho-minus
  (54,    2224)   // delta ++
  (55,    2214)   // delta +
  (56,    2114)   // delta 0
  (57,    1114)   // delta -
  (58,    -2224)  // delta ++ bar
  (59,    -2214)  // delta + bar
  (60,    -2114)  // delta 0 bar
  (61,    -1114)  // delta - bar
  (62,    313)    // k*0 (892)
  (63,    323)    // k*+ (892)
  (64,    -323)   // k*- (892)
  (65,    -313)   // k*0 bar (892)
  (66,    12)     // nu_e
  (67,    -12)    // anti-nu_e
  (68,    14)     // nu_mu
  (69,    -14)    // anti-nu_mu
  // 71-76 are special CORSIKA interaction tracking
  (116,   421)    // D0
  (117,   411)    // D+
  (118,   -411)   // D-
  (119,   -421)   // D0 bar
  (120,   431)    // D_s +
  (121,   -431)   // D_s -
  (122,   441)    // eta_c
  (123,   423)    // D*0 (2007)
  (124,   413)    // D*+ (2010)
  (125,   -413)   // D*- (2010)
  (126,   -423)   // D*0 bar (2007)
  (127,   433)    // D*s+
  (128,   -433)   // D*s-
  (130,   443)    // J/psi
  (131,   -15)    // tau+
  (132,   15)     // tau-
  (133,   16)     // nu_tau
  (134,   -16)    // anti-nu_tau
  (137,   4122)   // lambda_c+
  (138,   4232)   // xi_c+
  (139,   4132)   // xi_c0
  (140,   4222)   // sigma_c++
  (141,   4212)   // sigma_c+
  (142,   4112)   // sigma_c0
  (143,   4322)   // xi_c prime +
  (144,   4312)   // xi_c prime 0
  (145,   4332)   // omega_c 0
  (149,   -4122)  // lambda_c-
  (150,   -4232)  // xi_c-
  (151,   -4132)  // xi_c0 bar
  (152,   -4222)  // sigma_c--
  (153,   -4212)  // sigma_c-
  (154,   -4112)  // sigma_c0 bar
  (155,   -4322)  // xi_c prime -
  (156,   -4312)  // xi_c prime 0 bar
  (157,   -4332)  // omega_c 0 bar
  (161,   4224)   // sigma_c * ++
  (162,   4214)   // sigma_c * +
  (163,   4114)   // sigma_c * 0
  (171,   -4224)  // sigma_c * --
  (172,   -4214)  // sigma_c * -
  (173,   -4114)  // sigma_c * 0 bar
  ;

int32_t I3CorsikaReader::CorsikaToPDG(int corsika_id)
{
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


void I3CorsikaReader::CloseFile()
{
    corsFile_.close();
    corsFile_.clear();
    
    subBlock_.Reset();
    showerInfo_ = I3CorsikaShowerInfoPtr();
    
    currentRunNumber_ = -1;
    currentEvtNumber_ = -1;
    currentPartIndex_ = -1;
    
    lastEvtBlockPtr_ = 0;
    lastEvtSubBlockCounter_ = 0;
    
    particleDataSize_ = 0;
    isThinned_ = false;
}


void I3CorsikaReader::SetFileList(const std::vector<std::string>& fileNameList)
{
    CloseFile();
  
    fileIndex_ = -1;
    fileNameList_.assign(fileNameList.begin(), fileNameList.end());
}


bool I3CorsikaReader::OpenFile(const std::string& filename)
{
    // Close and reset previous buffers
    CloseFile();
    
    // Open new CORSIKA file
    corsFile_.open(filename.c_str(), ios::in|ios::binary);
    if(!corsFile_)
    {
        log_error("Unable to open CORSIKA file %s for reading!", filename.c_str());
        CloseFile();
        return false;
    }

    log_info("Opened CORSIKA file %s", filename.c_str());

    // Check for file binary structure
    CorsikaSubBlock tmpBlock0(CorsikaSubBlock::BLOCKSIZE_UNTHINNED, 0);
    corsFile_ >> tmpBlock0;
    int blockAdditionalChars = tmpBlock0.findRUNH();
    if (blockAdditionalChars < 0) {
        log_error("Failed to find RUNH. Is this a CORSIKA file?");
        CloseFile();
        return false;
    }
    corsFile_.seekg(0, std::ios::beg);
    
    //********************
    // CHECK for THINNING
    //********************
    
    // Use blocksize without thinning
    CorsikaSubBlock tmpBlock1(CorsikaSubBlock::BLOCKSIZE_UNTHINNED, blockAdditionalChars);
    CorsikaSubBlock tmpBlock2(CorsikaSubBlock::BLOCKSIZE_UNTHINNED, 0); // this is not the beginning of a block
    
    corsFile_ >> tmpBlock1;
    corsFile_ >> tmpBlock2;
    
    //tmpBlock1.Dump();
    //tmpBlock2.Dump();
    
    if(tmpBlock1.BlockType()==CorsikaSubBlock::RUNH && 
       tmpBlock2.BlockType()==CorsikaSubBlock::EVTH)
    {
        subBlock_.Reset(CorsikaSubBlock::BLOCKSIZE_UNTHINNED, blockAdditionalChars);
        particleDataSize_ = 7; // 7 REAL entries per particle
        isThinned_ = false;
    }
    else
    {
        // Change the block size
        tmpBlock1.Reset(CorsikaSubBlock::BLOCKSIZE_THINNED, blockAdditionalChars);
        tmpBlock2.Reset(CorsikaSubBlock::BLOCKSIZE_THINNED, 0);
        
        // Jump back to beginning
        corsFile_.seekg(0, ios::beg); 
        corsFile_ >> tmpBlock1;
        corsFile_ >> tmpBlock2;
        
        if(tmpBlock1.BlockType()==CorsikaSubBlock::RUNH &&
           tmpBlock2.BlockType()==CorsikaSubBlock::EVTH)
        {
            subBlock_.Reset(CorsikaSubBlock::BLOCKSIZE_THINNED, blockAdditionalChars);
            particleDataSize_ = 8; // 8 REAL entries per particle
            isThinned_ = true;
        }
        else
        {
            log_error("Failed to detect thinning mode. Cannot read file!");
            CloseFile();
            return false;
        }
    }
    
    log_info("   Thinning mode is %s", isThinned_?"THINNED":"NOT THINNED");
    
    // Spool back to the beginning of the CORSIKA file
    corsFile_.clear();
    corsFile_.seekg(0, ios::beg); 
    
    if(corsFile_.good())
    {
        return true;
    }
    else
    {
        CloseFile();
        return false;
    }
}


bool I3CorsikaReader::NextSubBlock()
{
    // Reset the particle index VERY IMPORTANT!
    currentPartIndex_ = -1;
    
    ios::pos_type currentBlockPtr=0;
    int maxFileIndex = fileNameList_.size() -1;
    do
    {
        if(!corsFile_.is_open())
        {
            fileIndex_++;
            if(fileIndex_ <= maxFileIndex)
            {
                OpenFile(fileNameList_[fileIndex_]);
            }
        }
        
        if(corsFile_.is_open())
        {
            currentBlockPtr = corsFile_.tellg();
            corsFile_ >> subBlock_;
            
            if(corsFile_.eof()) CloseFile();
        }
    } while(!corsFile_.is_open() && fileIndex_ < maxFileIndex);
    
    
    switch(subBlock_.BlockType())
    {
    case CorsikaSubBlock::RUNH:
    {
        if(currentRunNumber_ >= 0)
        {
            log_error("Previous run %d was not properly closed!", currentRunNumber_);
        }
        
        // Reset event counter
        eventCounter_ = 0;
        
        lastEvtBlockPtr_  = 0;
        currentRunNumber_ = static_cast<int>(subBlock_[2]);
        break;
    }
    
    case CorsikaSubBlock::RUNE:
    {
        int runNumber = static_cast<int>(subBlock_[2]);
        
        if(runNumber!=currentRunNumber_)
        {
            log_fatal("Wrong run rumber!");
            return false;
        }
        
        // Check number of events
        int numEvents = static_cast<int>(subBlock_[3]);
        if(eventCounter_>0 && numEvents != eventCounter_)
        {
            log_error("Read %d from %d events in run %d.", eventCounter_, numEvents, currentRunNumber_);
        }
        
        lastEvtBlockPtr_  = 0;
        currentRunNumber_ = -1;
        break;
    }
    
    case CorsikaSubBlock::EVTH:
    {
        int runNumber = static_cast<int>(subBlock_[44]);
        if(runNumber!=currentRunNumber_)
        {
            log_fatal("Wrong run rumber!");
            return false;
        }
        
        if(currentEvtNumber_ > 0)
        {
            log_error("Previous event %d was not properly closed!", currentEvtNumber_);
        }
        
        // Reset particle counter
        particleCounter_ = 0;
        
        lastEvtBlockPtr_ = currentBlockPtr;
        lastEvtSubBlockCounter_ = subBlock_.SubBlockCounter();
        currentEvtNumber_ = static_cast<int>(subBlock_[2]);
        
        // Create a new shower info object
        showerInfo_ = I3CorsikaShowerInfoPtr(new I3CorsikaShowerInfo());
        showerInfo_->crsRunID   = runNumber;
        showerInfo_->crsEventID = currentEvtNumber_;
        
        // Save start time for CPU time estimates
#ifndef NDEBUG
        time(&startTime_);
#endif
        break;
    }

    case CorsikaSubBlock::LONG:
    {
        FillLongProfile(subBlock_);
        break;
    }

    case CorsikaSubBlock::EVTE:
    {
        int evtNumber = static_cast<int>(subBlock_[2]);
        if(evtNumber!=currentEvtNumber_)
        {
            log_fatal("Wrong event rumber!");
            return false;
        }

        // Total number of particles
        int64_t numParticles = static_cast<int64_t>(subBlock_[7]);
            
        if(particleCounter_>0 && numParticles != particleCounter_)
        {
            // Calculate the readout error for large particle numbers
            float nPart0 = subBlock_[7];
            int32_t nPart1 = *(reinterpret_cast<int32_t*>(&nPart0));
            nPart1++;
            float nPart2 = *(reinterpret_cast<float*>(&nPart1));
            int64_t nPart3 = static_cast<int64_t>(nPart0);
            int64_t error = static_cast<int64_t>(nPart2) - nPart3;
            if((numParticles+error)<particleCounter_ || (numParticles-error)>particleCounter_)
            {
	      log_warn("Read %" PRId64 " from %" PRId64 " +/- %" PRId64 " particles in event %d.", particleCounter_, numParticles, error, currentEvtNumber_);
            }
        }
            
        if(showerInfo_)
        {
            // Fill Gaisser-Hillas fit results in shower info
            // For IceTop, we do NKG and EGS64, so the fit is applied to both e.m. and muons
            // LONGI T 20.0 T F has to be used (see CORSIKA manual)
            showerInfo_->ghMaxNum     = static_cast<double>(subBlock_[256]);//No. particles at max development
            showerInfo_->ghStartDepth = static_cast<double>(subBlock_[257]);//*(I3Units::g/(I3Units::cm2));//Starting depth of development
            showerInfo_->ghMaxDepth   = static_cast<double>(subBlock_[258]);//*(I3Units::g/(I3Units::cm2));//Location of shower max
            showerInfo_->ghRedChiSqr  = static_cast<double>(subBlock_[262]);//chi2 of GH fit

            log_debug("    Gaisser-Hillas fit."); 
            log_debug("    Nmax: %.2f", showerInfo_->ghMaxNum);
            log_debug("    t0: %.2f", (showerInfo_->ghStartDepth));///(I3Units::g/I3Units::cm2));
            log_debug("    tmax: %.2f", (showerInfo_->ghMaxDepth));///(I3Units::g/I3Units::cm2));
            log_debug("    Chi2: %.2f", showerInfo_->ghRedChiSqr);
        }
            
        currentEvtNumber_ = -1;
            
        break;
    }
    
    default: break;   
    }
    
    return (subBlock_.BlockType()!=CorsikaSubBlock::UNKNOWN);
}



bool I3CorsikaReader::NextRun(int& runID)
{
    if(currentRunNumber_>=0)
    {
        log_fatal("Moving to next run before previous run %d was finished!", currentRunNumber_);
    }
    
    do
    {
        NextSubBlock();
    } while(corsFile_.is_open() && subBlock_.BlockType()!=CorsikaSubBlock::RUNH);
    
    if(subBlock_.BlockType()==CorsikaSubBlock::RUNH)
    {
        runID = currentRunNumber_;
        
        //
        // Print out some run information
        //
        log_info("Reading CORSIKA run %d with the following settings:", runID);
        log_info("   CORSIKA version            : %.3f", subBlock_[4]);
        log_info("   Spectral index             : %.2f", subBlock_[16]);
        log_info("   Energy range               : [%.3f, %.3f] PeV",
             (subBlock_[17]*I3Units::GeV)/I3Units::PeV,
             (subBlock_[18]*I3Units::GeV)/I3Units::PeV);
        log_info("   Energy cutoff for hadrons  : %.3f MeV", (subBlock_[21]*I3Units::GeV)/I3Units::MeV);
        log_info("   Energy cutoff for muons    : %.3f MeV", (subBlock_[22]*I3Units::GeV)/I3Units::MeV);
        log_info("   Energy cutoff for electrons: %.3f MeV", (subBlock_[23]*I3Units::GeV)/I3Units::MeV);
        log_info("   Energy cutoff for photons  : %.3f MeV", (subBlock_[24]*I3Units::GeV)/I3Units::MeV);
        
        
        //
        // Determine lowest observation level
        //
        obsLevel_  = -1;
        zObsLevel_ = NAN;
        int numObsLevels = static_cast<int>(subBlock_[5]);
        for(int i=1; i<=numObsLevels; i++)
        {
            double zObsLevel = subBlock_[5+i]*I3Units::cm - I3Constants::OriginElev;
            
            if(i==1 || zObsLevel<zObsLevel_)
            {
                obsLevel_  = i;
                zObsLevel_ = zObsLevel; 
            }
        }
        
        //
        // Check if observation level is valid
        //
        if(finite(zObsLevel_) && obsLevel_>=1)
        {
            log_info("   Lowest observation level   : %.2f m  (%.2f m a.s.l.)",
                 zObsLevel_/I3Units::m, (zObsLevel_ + I3Constants::OriginElev)/I3Units::m);
        }
        else
        {
            log_error("Failed to determine lowest observation level. Aborting run!");
            return false;
        }
          
        //
        // Check if observation level makes sense for IceTop (only warn if not)
        //
        if(zObsLevel_ < I3Constants::zIceTop)
        {
            log_warn("Observation level is %.2f m below IceTop elevation!",
                 (I3Constants::zIceTop - zObsLevel_)/I3Units::m);
        }
        
        if(zObsLevel_ > (I3Constants::zIceTop + 10.0*I3Units::m))
        {
            log_warn("Observation level is %.2f m above IceTop elevation!",
                 (zObsLevel_ - I3Constants::zIceTop)/I3Units::m);
        }
        
        return true;
    }
    else
    {
        return false;
    }
}

bool I3CorsikaReader::NextEvent(I3Particle& primary, int& evtID)
{
    // Return false if no run is current
    if(currentRunNumber_<0) return false;

    log_debug("    Getting next event");
    
    if(currentEvtNumber_>0 && particleCounter_>0)
    {
        log_fatal("Moving to next event before previous event %d was finished!", currentEvtNumber_);
    }
    
    do
    {
        NextSubBlock();
    } while(currentRunNumber_>=0 && subBlock_.BlockType()!=CorsikaSubBlock::EVTH);
    
    if(subBlock_.BlockType()==CorsikaSubBlock::EVTH)
    {
        evtID = currentEvtNumber_;
	isCurved_ = bool(subBlock_[79]);
        
        primary.SetShape(I3Particle::Primary);
#ifdef I3PARTICLE_SUPPORTS_PDG_ENCODINGS
        primary.SetPdgEncoding(CorsikaToPDG(static_cast<int>(subBlock_[3])));
#else
        primary.SetType(I3Particle::ParticleType(static_cast<int>(subBlock_[3])));
#endif
    
        // Set total energy (including mass)
        primary.SetEnergy(subBlock_[4]*I3Units::GeV);
        primary.SetSpeed(I3Constants::c); // Approximately
        
        // Determine direction in IceCube coordinates
        double zenith  = subBlock_[11]*I3Units::rad;
        double azimuth = subBlock_[12]*I3Units::rad + I3Constants::pi;
        double arrang  = subBlock_[93]*I3Units::rad;
        I3Direction dir(zenith, azimuth - arrang);
        primary.SetDir(dir.GetX(), dir.GetY(), dir.GetZ());
        
	const double zStart = fabs(subBlock_[7])*I3Units::cm-I3Constants::OriginElev;

	// Extrapolate primary track to observation level and calculate
	// hypothetical track length
	double length = (zObsLevel_ - zStart)/primary.GetDir().GetZ();
	
	if(subBlock_[7]>0)
	{
	    // time cero is at the point of first interaction
	    primary.SetTime(length/I3Constants::c);
	}
	else
	{
	    // time cero is at an altitude of 112.8292 m a.s.l., hardwired in CORSIKA
	    const double l = -(112.8292*I3Units::km - I3Constants::OriginElev - zObsLevel_)/primary.GetDir().GetZ();
	    primary.SetTime(l/I3Constants::c);
	}
	
        
        primary.SetPos(0, 0, zObsLevel_);
        primary.SetLength(length);  // Can be used to calculate the first interaction height
        
        // Fill shower info
        if(showerInfo_)
        {
            showerInfo_->firstIntHeight = zStart + I3Constants::OriginElev; // height a.s.l.
            showerInfo_->obsLevelHeight = zObsLevel_ + I3Constants::OriginElev; // height a.s.l.
        }
        
        return true;
    }
    else
    {
        return false;
    }
}
    
bool I3CorsikaReader::NextParticle(I3Particle& particle, double& weight, const std::vector<int>& ignoretypes)
{
  double hadgen;
  return NextParticle(particle, weight, hadgen, ignoretypes);
}


bool I3CorsikaReader::NextParticle(I3Particle& particle, double& weight, double& hadgen, const std::vector<int>& ignoretypes)
{
    // Return false if no run or event is current
    if(currentRunNumber_<0 || currentEvtNumber_<1) return false;
    
    int index, particleCode;
    bool particleFound = false;
    
    int eventNumber = currentEvtNumber_;
    do
    {
        if(currentPartIndex_<0 || currentPartIndex_>=38) NextSubBlock();
    
        if(subBlock_.BlockType()==CorsikaSubBlock::PART)
        {
            currentPartIndex_++;
            
            index = currentPartIndex_*particleDataSize_;
            particleCode = static_cast<int>(std::floor(subBlock_[index+1]/1000.0));
            
            // Check if trailling zeros or additional muon info
            if(particleCode==0  ||
               particleCode==75 ||
               particleCode==76) continue;
            
            // Check if particle is a bunch of cerenkov photons
            if(subBlock_[index+1] >= 99e5) continue;
            
            // KR adds this for special "ignore all muons" test runs
            // (Do not use in normal running!)
            if(std::find(ignoretypes.begin(), ignoretypes.end(), particleCode) != ignoretypes.end()) continue;

            // Count all real particles on any observation level
            particleCounter_++;

            // Check observation level
            int level = static_cast<int>(subBlock_[index+1])%10;
            if(level!=obsLevel_) continue;
            
            particleFound = true;
        }
    } while(!particleFound && currentEvtNumber_==eventNumber && subBlock_.BlockType()!=CorsikaSubBlock::UNKNOWN);
    
    
    if(particleFound)
    {
#ifdef I3PARTICLE_SUPPORTS_PDG_ENCODINGS
        particle.SetPdgEncoding(CorsikaToPDG(particleCode));
#else
        particle.SetType(I3Particle::ParticleType(particleCode));
#endif  
        particle.SetShape(I3Particle::TopShower);
    
        // Direction
        float px = subBlock_[index+2];
        float py = subBlock_[index+3];
        float pz = subBlock_[index+4];
        float p = sqrt(px*px + py*py + pz*pz);
        
        particle.SetDir(px/p, py/p, -pz/p);
        
        // Position
	double x = subBlock_[index+5]*I3Units::cm;
	double y = subBlock_[index+6]*I3Units::cm;
	double z = zObsLevel_;
	if (isCurved_) {
	  // When in curved mode, CORSIKA uses a different definition of (x,y,z)
	  const double earthRadius = 6.371315e6*I3Units::m;
	  const double r = earthRadius + I3Constants::OriginElev + zObsLevel_;
	  const double theta = sqrt(x*x+y*y)/r;
	  const double phi = atan2(y,x);
	  const double d = r*sin(theta);
	  x = d*cos(phi);
	  y = d*sin(phi);
	  z -= r*(1-cos(theta));
	}
	particle.SetPos(x, y, z);
        
        // Time
        particle.SetTime(subBlock_[index+7]*I3Units::ns);
        
        // Energy/Speed 
        particle.SetEnergy(p*I3Units::GeV); // (only exact for massless particles)
        particle.SetSpeed(I3Constants::c);  // Approximately
        
        // Set particle weight if thinning is activated
        if(isThinned_) weight = subBlock_[index+8];
        else           weight = 1.0;

        hadgen = (static_cast<int>(subBlock_[index+1])%1000)/10;

    // Check if the simulation is still running by counting particles
#ifndef NDEBUG
        if(particleCounter_%1000000==0)
        {
            time(&endTime_);
            double diffTime = difftime(endTime_,startTime_)/60.0;
            log_trace("Particle %" PRId64 " of event %d is beeing processed! Elapsed time: %.2f minutes", particleCounter_, currentEvtNumber_, diffTime);
        }
#endif
            
        return true;
    }
    else
    {
        return false;
    }
}


void I3CorsikaReader::FillLongProfile(const CorsikaSubBlock& longBlock)
{
    if(!showerInfo_) return;
    
    for(int index=1; index<=26; ++index)
    {
        CorsikaLongStep longStep;
        longStep.depth = static_cast<double>(longBlock[10*index + 4]);//*(I3Units::g/I3Units::cm2);
      
        // Exit loop if end of longitudinal profile (trailing zeros) is reached  
        if(!showerInfo_->longProfile.empty() && longStep.depth==0) break;
        
        longStep.numGamma     = static_cast<uint64_t>(longBlock[10*index + 5]);
        longStep.numEPlus     = static_cast<uint64_t>(longBlock[10*index + 6]);
        longStep.numEMinus    = static_cast<uint64_t>(longBlock[10*index + 7]);
        longStep.numMuPlus    = static_cast<uint64_t>(longBlock[10*index + 8]);
        longStep.numMuMinus   = static_cast<uint64_t>(longBlock[10*index + 9]);
        longStep.numHadron    = static_cast<uint64_t>(longBlock[10*index + 10]);
        longStep.numCharged   = static_cast<uint64_t>(longBlock[10*index + 11]);
        longStep.numNuclei    = static_cast<uint64_t>(longBlock[10*index + 12]);
        longStep.numCherenkov = static_cast<uint64_t>(longBlock[10*index + 13]);
        
        showerInfo_->longProfile.push_back(longStep);
        
        showerInfo_->firstIntDepth = static_cast<double>(longBlock[7]);//*(I3Units::g/I3Units::cm2);
    }
}


void I3CorsikaReader::Rewind()
{
    if(currentRunNumber_>0 && lastEvtBlockPtr_>0)
    {
        // Rewind the CORSIKA file just to the beginning
        // of the event.
        corsFile_.seekg(lastEvtBlockPtr_);
        subBlock_.SubBlockCounter(lastEvtSubBlockCounter_-1);
        currentEvtNumber_ = -1;
    }
    else
    {
        log_error("Cannot rewind event!");
    }
}
