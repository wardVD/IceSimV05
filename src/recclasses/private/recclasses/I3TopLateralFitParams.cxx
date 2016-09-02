/**
 * Copyright (C) 2006
 * The IceCube collaboration
 * ID: $Id: I3TopLateralFitParams.cxx 141473 2016-02-04 02:13:37Z hdembinski $
 *
 * @file I3TopLateralFitParams.cxx
 * @version $Rev: 28706 $
 * @date $Date: 2016-02-03 20:13:37 -0600 (Wed, 03 Feb 2016) $
 * @author $Author: kislat $
 */

#include <icetray/serialization.h>
#include <recclasses/I3TopLateralFitParams.h>
#include <cmath>

double p10(const double x) { return std::pow(10., x); }

template <class Archive>
void I3TopLateralFitParams::serialize(Archive& ar, unsigned version)
{
  if (version>i3toplateralfitparams_version_)
    log_fatal("Attempting to read version %u from file but running version %u of I3TopLateralFitParams.",
	      version,i3toplateralfitparams_version_);

  ar & make_nvp("I3FrameObject", base_object<I3FrameObject>(*this));
  ar & make_nvp("Xc_err",  Xc_err );
  ar & make_nvp("Yc_err",  Yc_err );
  ar & make_nvp("S125",  S125 );
  ar & make_nvp("Log10_S125_err", Log10_S125_err );
  ar & make_nvp("S50",  S50 );
  if (version >= 3) {
    ar & make_nvp("S70",  S70 );
    ar & make_nvp("S80",  S80 );
  }
  ar & make_nvp("S100",  S100 );
  if (version >= 3) ar & make_nvp("S150",  S150 );
  ar & make_nvp("S180",  S180 );
  ar & make_nvp("S250",  S250 );
  if(version>0) ar & make_nvp("S500",  S500 );
  ar & make_nvp("Sint",  Sint );
  ar & make_nvp("Sint_err", Sint_err );
  if(version>0 && version < 4) ar & make_nvp("E_50", E_50 );
  if(version<4) ar & make_nvp("E_100", E_100 );
  if(version<4) ar & make_nvp("E_125", E_125 );
  if(version<4) ar & make_nvp("E_180", E_180 );
  if(version>0 && version< 4 ) ar & make_nvp("E_500", E_500 );
  if(version <4) ar & make_nvp("E_firstguess", E_firstguess );
  if(version >=4) ar & make_nvp("E_proton", E_proton );
  if(version >=4) ar & make_nvp("E_iron", E_iron );
  ar & make_nvp("Beta", Beta );
  ar & make_nvp("Beta_err", Beta_err );
  ar & make_nvp("Dir_err", Dir_err );   
  ar & make_nvp("Age", Age );
  ar & make_nvp("Llh", Llh );
  if(version>0) ar & make_nvp("Llh_silent", Llh_silent );
  ar & make_nvp("Chi2", Chi2 );
  ar & make_nvp("Ndf", Ndf );
  if (version >= 3) {
    ar & make_nvp("RLogL", RLogL );
    ar & make_nvp("NMini", NMini );
    ar & make_nvp("Chi2_time", Chi2_time );
  }
  ar & make_nvp("Corr_Xc_Yc", Corr_Xc_Yc );
  ar & make_nvp("Corr_Xc_Log10_S125", Corr_Xc_Log10_S125 );
  ar & make_nvp("Corr_Xc_Beta", Corr_Xc_Beta );
  ar & make_nvp("Corr_Yc_Log10_S125", Corr_Yc_Log10_S125 );
  ar & make_nvp("Corr_Yc_Beta", Corr_Yc_Beta );
  ar & make_nvp("Corr_Log10_S125_Beta", Corr_Log10_S125_Beta );
  ar & make_nvp("SumVEM", SumVEM );
  ar & make_nvp("Rmax", Rmax );
  ar & make_nvp("Rmin", Rmin );
  ar & make_nvp("Rtrans", Rtrans );
  ar & make_nvp("LoudestStation", LoudestStation );
  if (version >= 2) {
    ar & make_nvp("SnowF", SnowFactor );
    ar & make_nvp("SnowF_err", SnowFactor_err );
  }

}

std::ostream& operator<<(std::ostream& os, const I3TopLateralFitParams& p) {
  os << "[ I3TopLateralFitParams::"
     << "\n            S125: " << p.S125;
  if (!std::isnan(p.Log10_S125_err)) {
    os << " -" << (p.S125 * (p10(p.Log10_S125_err) - 1.0))
       << " +" << (p.S125 * (1.0 - p10(-p.Log10_S125_err)));
  }
  os << "\n  Log10_S125_err: " << p.Log10_S125_err
     << "\n            Beta: " << p.Beta
     << "\n        Beta_err: " << p.Beta_err
     << "\n          Xc_err: " << p.Xc_err
     << "\n          Yc_err: " << p.Yc_err
     << "\n            Chi2: " << p.Chi2
     << "\n             Ndf: " << p.Ndf
     << "\n       Chi2_time: " << p.Chi2_time
     << "\n        E_proton: " << p.E_proton
     << "\n          E_iron: " << p.E_iron
     << "\n  LoudestStation: " << p.LoudestStation
     << "\n      SnowFactor: " << p.SnowFactor
     << "\n  SnowFactor_err: " << p.SnowFactor_err
     << "\n             S50: " << p.S50
     << "\n             S70: " << p.S70
     << "\n             S80: " << p.S80
     << "\n            S100: " << p.S100
     << "\n            S150: " << p.S150
     << "\n            S180: " << p.S180
     << "\n            S250: " << p.S250
     << "\n            S500: " << p.S500
     << "\n  (...more parameters omitted...) ]";
  return os;
}

I3_SERIALIZABLE(I3TopLateralFitParams);
