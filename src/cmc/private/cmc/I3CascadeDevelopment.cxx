/**
 *@brief Implementation I3CascadeDevelopment class
 *
 * copyright  (C) 2007
 * the icecube collaboration
 *
 * $Id: I3CascadeDevelopment.cxx 88468 2012-05-19 03:35:25Z olivas $
 *
 * @version $Revision: 88468 $
 * @date $LastChangedDate: 2012-05-18 22:35:25 -0500 (Fri, 18 May 2012) $
 * @author Bernhard Voigt <bernhard.voigt@desy.de>   Last changed by: $LastChangedBy: olivas $
 */

//Standard C/C++ includes
#include <vector>

// icetray includes
#include "icetray/I3Units.h"

// local includes
#include "I3CascadeDevelopment.h"

// class constants
const double I3CascadeDevelopment::RADIATION_LENGTH = 36. * I3Units::cm;

// implementation of GetShowerLength
double I3CascadeDevelopment::GetLength() {
  return static_cast<double >(energyLossProfile_.size());
}

// Evaluation of energy loss between two given points in units of radiation length
double I3CascadeDevelopment::GetEnergyDeposit(int a, int b) {
  if (a < 0) { 
    log_warn("Lower index out of range (a=%i). Set to 0", a);  
    a = 0;
  }

  if (b > static_cast<int >(energyLossProfile_.size())) {
    log_trace("Higher index out of range (b=%i). Set to vector size (%zi)", b, energyLossProfile_.size());
    b = energyLossProfile_.size();
  }

  // if unreasonalbe numbers are given, return 0
  if (b < a) {
    return 0.;
  }

  // add up energy deposits between a and b (excluding contribution from b)
  std::vector<double>::iterator iter = energyLossProfile_.begin() + a;
  // end at b
  std::vector<double>::iterator end = energyLossProfile_.begin() + b;
  double sum = 0.;
  for (; iter != end; iter++) {
    sum += *iter;
  }
  return sum;
}



  

