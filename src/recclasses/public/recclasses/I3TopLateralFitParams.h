/**
 * Copyright (C) 2006
 * The IceCube collaboration
 * ID: $Id: I3TopLateralFitParams.h 143992 2016-03-30 19:46:40Z hdembinski $
 *
 * @version $Rev: 28706 $
 * @date $Date: 2016-03-30 14:46:40 -0500 (Wed, 30 Mar 2016) $
 * @author $Author: kislat $
 */

#ifndef RECCLASSES_I3TOPLATERALFITPARAMS_H_
#define RECCLASSES_I3TOPLATERALFITPARAMS_H_

#include <icetray/I3FrameObject.h>
#include <icetray/I3Logging.h>
#include <boost/serialization/access.hpp>
#include <boost/serialization/version.hpp>
#include <cmath>
#include <ostream>

static const unsigned i3toplateralfitparams_version_ = 4;

/**
 * @brief DEPRECATED Stores description of an air shower in IceTop (see project toprec).
 *
 * This class is superceded by I3LaputopParams and only kept for backward-compatibility.
 */
class I3TopLateralFitParams : public I3FrameObject
{

  public:

  /**
   * The error on the core X.
   */
  double Xc_err;

  /**
   * The error on the core Y.
   */
  double Yc_err;
 
  /**
   * Pulse height in 125m distance from the core (Fit parameter: log10(S125)).
   */
  double S125;

  /**
   * Error of that.
   */
  double Log10_S125_err;

  /**
   * Pulse height in 50m distance from the core.
   */
  double S50;

  /**
   * Pulse height in 70m and 80m distance from the core.
   */
  double S70;
  double S80;

  /**
   * Pulse height in 100m distance from the core.
   */
  double S100;

  /**
   * Pulse height in 150m distance from the core.
   */
  double S150;

  /**
   * Pulse height in 180m distance from the core.
   */
  double S180;

  /**
   * Pulse height in 250m distance from the core.
   */
  double S250;

  /**
   * Pulse height in 500m distance from the core.
   */
  double S500;

  /**
   * The integral over the lateral distribution function.
   */
  double Sint;

  /**
   * Error of that.
   */
  double Sint_err;

  /**
   * Energy as determined by S_100, S_125, S180.
   */
  double E_50;
  double E_100;
  double E_125;
  double E_180;
  double E_500;

  /**
   * First Guess Energy as determined by SAF report formula.
   */

  double E_firstguess;

  /**
   * Energy for proton and iron assumption as determined by Bakhtiyar Ruzybayez (TODO: add ref.).
   */
  double E_proton;
  double E_iron;


  /**
   * The slope parameter of the lateral fit.
   */
  double Beta;

  /**
   * Error of that.
   */
  double Beta_err;

  /**
   * The age parameter of the shower. In case of DLP function,
   * this is converted as shown in the SAF report (s. Internal
   * Report Repository).
   */
  double Age;

  /**
   * Error of direction (from combined fit).
   */
  double Dir_err;

  /**
   * The Log-Likelihood of the fit, divided by the degrees of freedom.
   */
  double Llh;
  double Llh_silent;

  /**
   * The Chi square, divided by the degrees of freedom.
   */
  double Chi2;

  /**
   * The Chi square, divided by the degrees of freedom, for the timing fit.
   */
  double Chi2_time;

  /**
   * The number of degrees of freedom (after all cuts).
   */
  int Ndf;

  /**
   * Other gulliverish stuff!
   * Reduced log-likelihood, and number of minimizations
   */
  double RLogL;
  int NMini;

  /**
   * Sum of all VEM in all available Tanks.
   */
  double SumVEM;

  /**
   * Core radius of the closest used pulse
   */
  double Rmin;


  /**
   * Core radius of most distant used pulse
   */
  double Rmax;


  /**
   * Radius used in energy extraction.
   */
  double Rtrans;

  /**
   * Loudest Station in this event.
   */
  int LoudestStation;


  /**
   * Best-fit (or user-parameter) snow correction factor,
   * and its error
   */
   double SnowFactor;
   double SnowFactor_err;

  /**
   * The correlations of the fit variables. It is taken from the Minuit Error
   * Matrix Corr_1_2 = ErrMat[1][2] / Err1 / Err2
   */

  double Corr_Xc_Yc;
  double Corr_Xc_Log10_S125;
  double Corr_Xc_Beta;
  double Corr_Yc_Log10_S125;
  double Corr_Yc_Beta;
  double Corr_Log10_S125_Beta;

  I3TopLateralFitParams() :
    Xc_err(NAN),
    Yc_err(NAN),
    S125(NAN),
    Log10_S125_err(NAN),
    S50(NAN),
    S70(NAN),
    S80(NAN),
    S100(NAN),
    S150(NAN),
    S180(NAN),
    S250(NAN),
    S500(NAN),
    Sint(NAN),
    E_50(NAN),
    E_100(NAN),
    E_125(NAN),
    E_180(NAN),
    E_500(NAN),
    E_proton(NAN),
    E_iron(NAN),
    Beta(NAN),
    Beta_err(NAN),
    Dir_err(NAN),
    Llh(NAN),
    Llh_silent(NAN),
    Chi2(NAN),
    Chi2_time(NAN),
    Ndf(0),
    RLogL(NAN),
    NMini(0),
    SumVEM(NAN),
    Rmax(NAN),
    Rtrans(NAN),
    LoudestStation(-1),
    SnowFactor(NAN),
    SnowFactor_err(NAN),
    Corr_Xc_Yc(NAN),
    Corr_Xc_Log10_S125(NAN),
    Corr_Xc_Beta(NAN),
    Corr_Yc_Log10_S125(NAN),
    Corr_Yc_Beta(NAN),
    Corr_Log10_S125_Beta(NAN)
    {};

  virtual ~I3TopLateralFitParams() {};

 private:

  friend class boost::serialization::access;
  template <class Archive> void serialize(Archive& ar, unsigned version);

};

std::ostream& operator<<(std::ostream&, const I3TopLateralFitParams&);

BOOST_CLASS_VERSION(I3TopLateralFitParams, i3toplateralfitparams_version_);       
 
I3_POINTER_TYPEDEFS(I3TopLateralFitParams);

#endif
