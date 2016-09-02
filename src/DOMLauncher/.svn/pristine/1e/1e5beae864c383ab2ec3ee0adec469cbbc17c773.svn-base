#ifndef DOMLAUNCHER_INTERPOLATEDSPETEMPLATE_H_INCLUDED
#define DOMLAUNCHER_INTERPOLATEDSPETEMPLATE_H_INCLUDED
// ___header files___
// I3 headers
#include "dataclasses/calibration/I3Calibration.h"
#include "icetray/I3Units.h"

//std headers
#include <iostream>
#include <vector>
#include <cfloat>

/**
  * class: InterpolatedSPETemplate
  *
  * @version $Id: $
  *
  * @date: $Date: $
  *
  * @author Samuel Flis <samuel.d.flis@gmail.com>
  *
  * \brief This class provides an interpolation of an I3DOMCalibration::DroopedSPETemplate which
  * gives a significant speedup when evaluating DroopedSPETemplates.
  *
  * The InterpolatedSPETemplate class acts as a wrapper of an I3DOMCalibration::DroopedSPETemplate
  * and from the point of template evaluation has the same interface as an
  * I3DOMCalibration::DroopedSPETemplate. The class is initialized with an
  * I3DOMCalibration::DroopedSPETemplate.
  *
  * Linear interpolation is used to interpolate the pulse because of its simple implementation and
  * fast speed. To capture the pulse template with sufficient precision efficiently over the range
  * of 1 s (1e9 ns) three different interpolation step lengths are used. A dense region of
  * linear steps covering the pulse peak itself after which a coarse (sparse) region of linear
  * steps comes where the droop recovery is still significant. At a couple of micro seconds
  * after the pulse peak the interpolation uses quadratic steps until the end of the interpolation
  * range.
  * (c) 2012 IceCube Collaboration
  */

class InterpolatedSPETemplate{
 public:
  /// Initializes pulseTemplate with made up numbers.
 InterpolatedSPETemplate():
  pulseTemplate_(SPETemplate(), SPETemplate(), 0.1, 0.1, 0.1),
    hasTabulated_(false)
      {}

  /// A constructor that initilizes the pulse template with base
 InterpolatedSPETemplate(const I3DOMCalibration::DroopedSPETemplate &base):
  pulseTemplate_(base),
    hasTabulated_(false)
      {}

  ///Provides a method to set the SPETemplate
  ///\param pulseTemplate the I3DOMCalibration::DroopedSPETemplate to be interpolated.
  void SetTemplate(I3DOMCalibration::DroopedSPETemplate pulseTemplate){
    pulseTemplate_ = pulseTemplate;
  }
  ///Tabulates the DroopedSPETemplate which have been provided so it can be later interpolated.
  ///If this is not done a call to the ()-operator will just evaulate the pulse template
  ///directly.
  ///\param nDenseSteps the number of dense linear bins in the interpolation
  ///\param nCoarseSteps the number of coarse linear bins in the interpolation
  ///\param changePointLinear the time at which the bin size changes from dense to coarse
  ///\param changePointQuadratic the time at which the bin size changes from coarse to quadratic
  ///\param endPoint the time for the last bin in the interpolation. After this time the pulse
  ///                template will be evaluated directly instead
  void Tabulate(int nDenseSteps,
                int nCoarseSteps,
                double startPoint,
                double changePointLinear,
                double changePointQuadratic,
                double endPoint);
  /// This member function returns the amplitude of the pulse at time \param t
  double operator()(double t);
  ///A testing function that prints out the square sum of the difference between
  ///the interpolated pulse template and the real pulse template.
  double TabulationError();

 private:
  ///This is the actual interpolation function which is called by double operator()(double t)
  double Interpolate(double t);
  ///Stores the tabulated pulse template
  std::vector<float> tabulatedPulse_;
  ///Stores the derivatives of the tabulated pulse template (linear approx.)
  std::vector<float> tabulatedPulseDerivative_;

  I3DOMCalibration::DroopedSPETemplate pulseTemplate_;

  double denseStepSize_;
  ///The inverse (1/denseStepSize_) is also stored which eliminates unecessary divisions and
  ///therefore speeds up the lookup in the interpolation.
  double invDenseStepSize_;


  double coarseStepSize_;
  ///The inverse (1/coarseStepSize_) is also stored which eliminates unecessary divisions and
  ///therefore speeds up the lookup in the interpolation.
  double invCoarseStepSize_;

  double quadraticStepScale_;
  double invQuadraticStepScale_;

  double startPoint_;
  double changePointLinear_;
  double changePointQuadratic_;
  double endPoint_;

  uint64_t nDenseSteps_;
  uint64_t nCoarseSteps_;
  uint64_t nQuadraticSteps_;

  bool hasTabulated_;

};


inline double InterpolatedSPETemplate::Interpolate(double t){
  if(t < startPoint_)//for causality reasons the pulse template is defined to be zero for t<startPoint_
    return 0;
  else if(t < changePointLinear_){
    uint64_t lowerIndex = uint64_t((t - startPoint_ ) * invDenseStepSize_);//determining the index for time t
    return tabulatedPulseDerivative_[lowerIndex] * (t - lowerIndex * denseStepSize_ - startPoint_) +
    tabulatedPulse_[lowerIndex];
  }
  else if(t < changePointQuadratic_){
     //determing the internal index for time t (only for coarse part of the table)
    uint64_t internalIndex = uint64_t((t - changePointLinear_) * invCoarseStepSize_);
    //determing the global index for time t (the whole table)
    uint lowerIndex = internalIndex + nDenseSteps_ - 1;
    return tabulatedPulseDerivative_[lowerIndex] *
    (t - coarseStepSize_ * internalIndex - changePointLinear_) + tabulatedPulse_[lowerIndex];
  }
  else if(t < endPoint_){
     //determing the internal index for time t
     //(only need integer value for the square root)(only for quadratic part of the table)
    uint64_t internalIndex =  uint64_t(sqrt( (t - changePointQuadratic_) * invQuadraticStepScale_));
    uint64_t lowerIndex = uint64_t(internalIndex + nDenseSteps_ + nCoarseSteps_ - 1 );
    return tabulatedPulseDerivative_[lowerIndex] *
    (t - (internalIndex) * (internalIndex) * quadraticStepScale_ - changePointQuadratic_) +
    tabulatedPulse_[lowerIndex];
  }
  else{//When t > endPoint_, which should be rare, the original DroopedSPETemplate is evaluated
    return pulseTemplate_(t);
  }

}

inline double InterpolatedSPETemplate::operator()(double t){
  if(hasTabulated_)
    return Interpolate(t);
  else
    return pulseTemplate_(t);
}

typedef boost::shared_ptr<InterpolatedSPETemplate> InterpolatedSPETemplatePtr;
#endif
