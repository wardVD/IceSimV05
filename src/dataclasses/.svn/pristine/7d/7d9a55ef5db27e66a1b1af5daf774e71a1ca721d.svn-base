#include <icetray/serialization.h>
#include <dataclasses/physics/I3MCHit.h>
#include <dataclasses/physics/I3Particle.h>
I3MCHit::~I3MCHit() { }

void I3MCHit::SetParticleID(const I3Particle& p) { 
  particleID_ = p.GetMinorID(); 
  particleMajorID_ = p.GetMajorID();
}


template <class Archive>
void I3MCHit::save (Archive &ar, const unsigned version) const
{
  //always write the latest version
  ar & make_nvp("time",time_);
  ar & make_nvp("hitID",hitID_);
  ar & make_nvp("NPE", npe_);
  ar & make_nvp("Charge", charge_);
  ar & make_nvp("ParticleID", particleID_);
  ar & make_nvp("ParticleMajorID", particleMajorID_);
  ar & make_nvp("CherenkovDistance", cherenkovDistance_);
  ar & make_nvp("HitSource", source_);
}

template <class Archive>
void I3MCHit::load (Archive &ar, const unsigned version)
{
  if (version>i3mchit_version_)
    log_fatal("Attempting to read version %u from file but running version %u of I3MCHit class.",version,i3mchit_version_);
  
  ar & make_nvp("time",time_);
  ar & make_nvp("hitID",hitID_);
  if(version>2){
    ar & make_nvp("NPE", npe_);
    ar & make_nvp("Charge", charge_);    
  }else{
    double weight;
    ar & make_nvp("Weight", weight);
    npe_ = static_cast<uint64_t>(weight);
  }
  ar & make_nvp("ParticleID", particleID_);
  if(version>0)
    ar & make_nvp("ParticleMajorID", particleMajorID_);
  ar & make_nvp("CherenkovDistance", cherenkovDistance_);
  if(version>1)
    ar & make_nvp("HitSource", source_);

}

std::ostream& operator<<(std::ostream& oss, const I3MCHit& h){
  oss << "[ I3MCHit::" << std::endl
      << "  Time          :" << h.GetTime() << std::endl
      << "  HitID         :" << h.GetHitID() << std::endl
      << "  NPE           :" << h.GetNPE()<< std::endl
      << "  Charge        :" << h.GetCharge() << std::endl
      << "  ParticleID    :" << h.GetParticleMajorID()<<"/"<< h.GetParticleMinorID()<<std::endl
      << "  CherenkovDist :" << h.GetCherenkovDistance() << std::endl
      << "  HitSource     :" << h.GetHitSource()<<" ]" << std::endl;
  return oss;
}

I3_SPLIT_SERIALIZABLE(I3MCHit);
I3_SERIALIZABLE(I3MCHitSeriesMap);
