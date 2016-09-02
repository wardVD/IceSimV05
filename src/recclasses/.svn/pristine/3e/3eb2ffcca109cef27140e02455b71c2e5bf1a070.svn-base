#include <icetray/serialization.h>
#include <tableio/I3Converter.h>

#include "recclasses/CramerRaoParams.h"

CramerRaoParams::~CramerRaoParams()
{
}


template <class Archive>
void CramerRaoParams::serialize(Archive& ar,unsigned version)
{
  if (version>cramerraoparams_version_)
    log_fatal("Attempting to read version %u from file but running version %u of CramerRaoParams.",
     	      version,cramerraoparams_version_);

  // must serialize the base object
  ar & make_nvp("I3FrameObject", base_object<I3FrameObject>(*this));

  // and all the data members
  ar & make_nvp("cramer_rao_theta",cramer_rao_theta);
  ar & make_nvp("cramer_rao_phi",cramer_rao_phi);
  ar & make_nvp("status",status);
  if(version<=1){
    ar & make_nvp("cramer_rao_theta_corr",cramer_rao_theta_corr);
    ar & make_nvp("cramer_rao_phi_corr",cramer_rao_phi_corr);
    ar & make_nvp("llh_est",llh_est);
    ar & make_nvp("rllh_est",rllh_est);
  }
  if(version>=2){
    ar & make_nvp("variance_theta",variance_theta);
    ar & make_nvp("variance_phi",variance_phi);
    ar & make_nvp("variance_x",variance_x);
    ar & make_nvp("variance_y",variance_y);
    ar & make_nvp("covariance_theta_phi",covariance_theta_phi);
    ar & make_nvp("covariance_theta_x",covariance_theta_x);
    ar & make_nvp("covariance_theta_y",covariance_theta_y);
    ar & make_nvp("covariance_phi_x",covariance_phi_x);
    ar & make_nvp("covariance_phi_y",covariance_phi_y);
    ar & make_nvp("covariance_x_y",covariance_x_y);
  }
}

// this macro instantiates all the needed serialize methods
I3_SERIALIZABLE(CramerRaoParams);
