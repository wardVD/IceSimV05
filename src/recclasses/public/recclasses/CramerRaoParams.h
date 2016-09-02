/**
 * $Id$
 * @author Jan Luenemann <jan.luenemann@uni-mainz.de>
 * @author Kai Schatto
 */

#ifndef RECCLASSES_CRAMERRAOPARAMS_H
#define RECCLASSES_CRAMERRAOPARAMS_H

#include <icetray/I3FrameObject.h>
#include "dataclasses/Utility.h"

static const unsigned cramerraoparams_version_ = 2;

/**
 *  @brief Stores covariance matrix from CramerRao module (see project cramer-rao).
 */
class CramerRaoParams : public I3FrameObject
{
 public:
  double cramer_rao_theta; ///< estimated standard deviation
  double cramer_rao_phi; ///< estimated standard deviation
  double variance_theta; ///< element of inverse of information matrix
  double variance_phi; ///< element of inverse of information matrix
  double variance_x; ///< element of inverse of information matrix
  double variance_y; ///< element of inverse of information matrix
  double covariance_theta_phi; ///< element of inverse of information matrix
  double covariance_theta_x; ///< element of inverse of information matrix
  double covariance_theta_y; ///< element of inverse of information matrix
  double covariance_phi_x; ///< element of inverse of information matrix
  double covariance_phi_y; ///< element of inverse of information matrix
  double covariance_x_y; ///< element of inverse of information matrix

  double cramer_rao_theta_corr; ///< obsolete
  double cramer_rao_phi_corr; ///< obsolete
  double llh_est; ///< obsolete
  double rllh_est; ///< obsolete

  /// status flags of calculation
  enum CramerRaoStatus {
    NotSet           = -1,
    OK               = 0 ,
    MissingInput     = 10,
    SingularMatrix   = 20,
    InsufficientHits = 30,
    OneStringEvent   = 40,
    OtherProblems    = 50
   
  };
  
  /// status of calculation
  CramerRaoStatus status; 

  CramerRaoParams() : 
    cramer_rao_theta(NAN),
    cramer_rao_phi(NAN),
    variance_theta(NAN), 
    variance_phi(NAN),
    variance_x(NAN),
    variance_y(NAN),
    covariance_theta_phi(NAN),
    covariance_theta_x(NAN),
    covariance_theta_y(NAN),
    covariance_phi_x(NAN),
    covariance_phi_y(NAN),
    covariance_x_y(NAN),
    cramer_rao_theta_corr(NAN), 
    cramer_rao_phi_corr(NAN),
    llh_est(NAN),
    rllh_est(NAN),
    status(NotSet)
    {}

  // needs a virtual destructor implemented in the .cxx file
  virtual ~CramerRaoParams();

 private:
  // serialization routine
  friend class boost::serialization::access;
  template <class Archive>
    void serialize(Archive& ar, unsigned version);
};

BOOST_CLASS_VERSION(CramerRaoParams, cramerraoparams_version_);

// optional but gives some boost::shared_ptr typedefs
I3_POINTER_TYPEDEFS(CramerRaoParams);

#endif
