#include "recclasses/I3PortiaPulse.h"
#include <icetray/serialization.h>

template <class Archive>
void I3PortiaPulse::serialize(Archive& ar, unsigned version)
{
  ar & make_nvp("I3FrameObject", base_object<I3FrameObject>(*this));

  ar & make_nvp("I3RecoPulse", recoPulse_);
  ar & make_nvp("BaseLine",    BaseLine_);
  ar & make_nvp("LaunchTime",  LaunchTime_);
  ar & make_nvp("PeakBinTime", PeakBinTime_);
  ar & make_nvp("T50",         Time50_);
  ar & make_nvp("T80",         Time80_);
  ar & make_nvp("TOT",         TOT_);
  ar & make_nvp("LETime",      LETime_);
  ar & make_nvp("NPE",         NPE_);
  ar & make_nvp("Charge",      IntegratedCharge_);
  ar & make_nvp("Amplitude",   Amplitude_);
  ar & make_nvp("LCBit",       LCBit_);
  ar & make_nvp("pmtGain",     PMTGain_);
  ar & make_nvp("PositionX",   PositionX_);
  ar & make_nvp("PositionY",   PositionY_);
  ar & make_nvp("PositionZ",   PositionZ_);
  
  if(version > 0){
    ar & make_nvp("BinNumber",      BinNumber_);
    ar & make_nvp("BinSize",        BinSize_);
  }else {
    BinNumber_ = 0;
    BinSize_ = 0.0;
  }

  if(version > 1){
    ar & make_nvp("StartTime",  StartTime_);
    StartTime_ = 0.0;
  }

  //for future use    
  /*
    if(version > 1)
    ar & make_nvp("pmtGain", PMTGain_);
    else PMTGain_ = 1;
  */
  
}

I3_SERIALIZABLE(I3PortiaPulse);
I3_SERIALIZABLE(I3PortiaPulseMap);
