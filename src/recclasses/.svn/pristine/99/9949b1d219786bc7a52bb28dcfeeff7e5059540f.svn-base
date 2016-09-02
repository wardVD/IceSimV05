/**
 * @brief Transformation of I3TopLateralFitParams to I3LaputopParams.
 *
 * The following code implements an overload for I3Frame::Get
 * that transparently converts an I3TopLateralFitParams stored
 * in a frame into a I3LaputopParams. 
 * So, in client code, you just do
 *   frame->Get<I3LaputopParamsConstPtr>(my_key)
 * and it will work for both old and new productions. Neat, huh?
 */
#include "recclasses/I3TopLateralFitParams.h"
#include "recclasses/I3LaputopParams.h"
#include <icetray/I3Frame.h>
#include <icetray/I3FrameObject.h>
#include <dataclasses/physics/I3Particle.h>
#include <boost/shared_ptr.hpp>
#include <cmath>

template <>
I3LaputopParamsConstPtr
I3Frame::Get(const std::string& name, bool quietly, void*, void*) const
{
  I3FrameObjectConstPtr fobj =
    this->Get<I3FrameObjectConstPtr>(name, quietly);

  if (!fobj) // name does not exist in frame
    return I3LaputopParamsConstPtr();

  if (I3LaputopParamsConstPtr params =
      boost::dynamic_pointer_cast<const I3LaputopParams>(fobj)) {
    // all good, no conversion needed
    return params;
  }

  if (I3TopLateralFitParamsConstPtr p =
      boost::dynamic_pointer_cast<const I3TopLateralFitParams>(fobj)) {
    // convert I3TopLateralFitParams into I3LaputopParams
    I3LaputopParamsPtr params(new I3LaputopParams);
    std::string particle_name = name.substr(0, name.size() - 6);
    I3ParticleConstPtr particle =
      this->Get<I3ParticleConstPtr>(particle_name);
    if (particle)
      params->FillFromI3Particle(*particle);
    using namespace Laputop;
    params->typeLDF_ = LDF::DLP;
    params->typeFrontDelay_ = FrontDelay::GaussParabola;
    params->typeEnergy_ = Energy::ICRC2015_H4a_E27;
    params->SetParameter(Parameter::Log10_S125,
      std::log10(p->S125), p->Log10_S125_err);
    params->SetParameter(Parameter::Beta,
      p->Beta, p->Beta_err);
    params->SetValue(Parameter::Age, p->Age);
    params->chi2_LDF_ = p->Chi2 * p->Ndf;
    params->ndf_LDF_ = p->Ndf;
    params->chi2_Time_ = p->Chi2_time * p->Ndf;
    params->ndf_Time_ = p->Ndf;
    params->logLikelihood_ = p->Llh;
    params->logLikelihood_Silent_ = p->Llh_silent;
    params->nMini_ = p->NMini;
    params->SetError(Parameter::Xc, p->Xc_err);
    params->SetError(Parameter::Yc, p->Yc_err);
    params->SetCovariance(Parameter::Xc, Parameter::Yc,
      p->Corr_Xc_Yc * p->Xc_err * p->Yc_err);
    params->SetCovariance(Parameter::Xc, Parameter::Log10_S125,
      p->Corr_Xc_Log10_S125 * p->Xc_err * p->Log10_S125_err);
    params->SetCovariance(Parameter::Xc, Parameter::Beta,
      p->Corr_Xc_Beta * p->Xc_err * p->Beta_err);
    params->SetCovariance(Parameter::Yc, Parameter::Log10_S125,
      p->Corr_Yc_Log10_S125 * p->Yc_err * p->Log10_S125_err);
    params->SetCovariance(Parameter::Yc, Parameter::Beta,
      p->Corr_Yc_Beta * p->Yc_err * p->Beta_err);
    params->SetCovariance(Parameter::Log10_S125, Parameter::Beta,
      p->Corr_Log10_S125_Beta * p->Log10_S125_err * p->Beta_err);
    return params;
  }

 // none of the conversions worked
 return I3LaputopParamsConstPtr();
}
