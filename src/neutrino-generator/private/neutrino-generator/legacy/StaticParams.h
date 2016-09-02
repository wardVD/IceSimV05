#ifndef NuGStaticParams_H
#define NuGStaticParams_H
/**
 *   Copyright  (C) 2005
 *   The IceCube Collaboration
 *   $Id: $
 *
 *   @file StaticParams.cxx
 *   @version $Revision: $
 *   @date    $Date:     $ 
 *   @author Kotoyo Hoshina <kotoyo.hoshina@icecube.wisc.edu>
 * 
 *   @brief StaticParams
 *   provides static params for neutrino-generator.
 *   All params must be static.
 *   Use StaticParamsSetter to change these values.
 */

#include <string>
#include <map>
#include <neutrino-generator/legacy/I3NuGVEvent.h>
#include <neutrino-generator/utils/EnumTypeDefs.h>

namespace nugen {


class StaticParams
{

 public: 

  // string option to select propagation mode
  static PropagationMode gPROPAGATION_MODE;

  // number of steps to calculate column depth
  static int gNSTEPS;    
  static int gNSTEPS_FINAL;    

  // fine step size used to calculate column depth
  static double gFINE_STEP_SIZE;

  // tolerance for length, used for loop in length
  static double gLENGTH_TOLERANCE;

  // tolerance for change in density
  static double gDENSITY_TOLERANCE;

  // threshold value of weight exponential term
  static double gEXPTERM_THRESHOLD;

  // option for choosing interaction position calculation
  static int gINTPOS_SAMPLE_OPT;

  // option for choosing interaction interaction weight calculation
  static int gINTERACT_WEIGHT_OPT;

  // options for choosing crosssection*columndepth calculation
  static int gCROSSSECTIONxCOLUMNDEPTH_OPT;
  static double gCROSSSECTIONxCOLUMNDEPTH_OPT_STEPSIZE;

  // option whether re-calculate impact param for each daughters or not
  static int gIMPACTPARAM_OPT;

 private:
  StaticParams() {}
  StaticParams(const StaticParams &) {}

};

}

#endif //StaticParams_H


