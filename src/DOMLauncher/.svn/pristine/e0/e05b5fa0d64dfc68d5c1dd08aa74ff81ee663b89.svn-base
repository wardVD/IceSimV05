#include "DOMLauncher/InterpolatedSPETemplate.h"

using namespace std;

void InterpolatedSPETemplate::Tabulate(int nDenseSteps,
				       int nCoarseSteps,
				       double startPoint,
				       double changePointLinear,
				       double changePointQuadratic,
				       double endPoint)
{

  hasTabulated_ = true;

  nDenseSteps_ =  nDenseSteps;
  nCoarseSteps_ = nCoarseSteps;

  startPoint_ = startPoint;
  changePointLinear_ = changePointLinear;
  changePointQuadratic_ = changePointQuadratic;

  denseStepSize_ = (changePointLinear-startPoint_)/nDenseSteps_;
  coarseStepSize_ = (changePointQuadratic - changePointLinear_)/nCoarseSteps_;
  nQuadraticSteps_ = sqrt((endPoint - changePointQuadratic)/coarseStepSize_);

  quadraticStepScale_  = coarseStepSize_;
  endPoint_ = changePointQuadratic + nQuadraticSteps_ * nQuadraticSteps_ * nCoarseSteps_;

  invDenseStepSize_ = 1/denseStepSize_;
  invCoarseStepSize_ = 1/coarseStepSize_;
  invQuadraticStepScale_ = 1/quadraticStepScale_;

  // resizing the vectors to allocate the right amount of entries.
  tabulatedPulseDerivative_.resize(nDenseSteps_+nCoarseSteps_+nQuadraticSteps_);
  tabulatedPulse_.resize(nDenseSteps_+nCoarseSteps_+nQuadraticSteps_+1);

  tabulatedPulse_[0] = pulseTemplate_(startPoint_); //first table entry

  // tabulating dense linear region starting at t=0 and stopping at t=changePointLinear_
  double t = startPoint_;
  for(uint i = 1 ; i < nDenseSteps_; i++){
      t += denseStepSize_;
      tabulatedPulse_[i] = pulseTemplate_(t);
      tabulatedPulseDerivative_[i-1] = (tabulatedPulse_[i]-tabulatedPulse_[i-1]) * invDenseStepSize_;
  }

  // tabulating coarse linear region starting at t=changePointLinear_ and
  // stopping at t=changePointQuadratic_
  for(uint i = nDenseSteps_; i < nCoarseSteps_ + nDenseSteps_; i++){
      t += coarseStepSize_;
      tabulatedPulse_[i] = pulseTemplate_(t);
      tabulatedPulseDerivative_[i-1] =
      (tabulatedPulse_[i]-tabulatedPulse_[i-1]) * invCoarseStepSize_;
  }

  //tabulating quadratic region starting at t=changePointQuadratic_ and stopping at t=endPoint_
  double t_last= t;
  double t_1 = t_last;
  uint step = 2;
  t += coarseStepSize_;

  for(uint i = nDenseSteps_ + nCoarseSteps_; i < nCoarseSteps_+nDenseSteps_+nQuadraticSteps_; i++){
    double stepSize = t - t_last;
    t_last = t;
    tabulatedPulse_[i] = pulseTemplate_(t);
    tabulatedPulseDerivative_[i-1] = (double(tabulatedPulse_[i]) - tabulatedPulse_[i-1]) / stepSize;
    t = t_1 + step*step*quadraticStepScale_;
    step++;
  }
}

double InterpolatedSPETemplate::TabulationError(){
  double t=0;
  double stepSize=0.1;
  double error2Sum = 0;
  double max = 0;
  double errorSumDense = 0;
  double errorSumCoarse = 0;
  double errorSumQuadratic = 0;

  while(t<changePointLinear_){
    if(pulseTemplate_(t)>max)max = pulseTemplate_(t);
    double error = this->operator()(t) - pulseTemplate_(t);
    error *=error;
    errorSumDense += error;
    t+=stepSize;
  }

  while(t<changePointQuadratic_){
    double error = this->operator()(t) - pulseTemplate_(t);
    error *=error;
    errorSumCoarse += error;
    t+=stepSize;
  }

  stepSize=100;
  while(t<endPoint_){
    double error = this->operator()(t) - pulseTemplate_(t);
    error *=error;
    errorSumQuadratic += error;
    t+=stepSize;
  }

  error2Sum = errorSumDense + errorSumCoarse + errorSumQuadratic;
  log_debug("error2Sum Dense: %f",errorSumDense/max);
  log_debug("error2Sum Dense/steps: %f",errorSumDense*stepSize/changePointLinear_/max);

  log_debug("error2Sum Coarse: %f",errorSumCoarse/max);
  log_debug("error2Sum Coarse/steps: %f",errorSumCoarse*stepSize/(changePointQuadratic_ - changePointLinear_)/max);

  log_debug("error2Sum Quadratic: %f",errorSumQuadratic/max);
  log_debug("error2Sum Quadratic/steps: %f",errorSumQuadratic*stepSize/(endPoint_ -changePointQuadratic_)/max);

  log_debug("error2Sum: %f",error2Sum/max);
  log_debug("error2Sum/steps: %f",error2Sum*stepSize/endPoint_/max);
  return error2Sum/max;
}
