/** *   Copyright  (C) 2005
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
 */

#include "icetray/I3Units.h"
#include "neutrino-generator/legacy/StaticParams.h"

using namespace nugen;

//-----------------------------------------
// Propagation Mode
//
// nugen::LEGACY       : use legacy Event.
// nugen::NOPROPWEIGHT : no propagation weight with WeightedEvent. 
//                Some neutrino may be absorbed by the Earth. 
// nugen::NCGRWWEIGHTED : CC interaction is forbidden and 
//                always NC or GR interacion is chosen if
//                interactions occurred during propagation.
//                Propagation weight is applied.
//                No neutrino is absorbed by the Earth.
// !! CAUTION !! for Tau gen, use NoWeihgt option to activate 
// tau regeneration.

//-----------------------------------------
nugen::PropagationMode StaticParams::gPROPAGATION_MODE = nugen::NOPROPWEIGHT;   

//-----------------------------------------
// number of steps to calculate column depth
//-----------------------------------------
int StaticParams::gNSTEPS    = 10000;   
int StaticParams::gNSTEPS_FINAL  = 1000;   

//-----------------------------------------
// fine step size for interaction column depth
// calculation
// Don't change the step size unless you know
// well about the parameter
//-----------------------------------------
double StaticParams::gFINE_STEP_SIZE = (1 * I3Units::m);

//-----------------------------------------
// tolerance for length.
// used in for loop in length in order to 
// give tolerance for roundoff errors
//-----------------------------------------
double StaticParams::gLENGTH_TOLERANCE = (1e-6 * I3Units::m);

//-----------------------------------------
// tolerance for density change
// default : +-5%
//-----------------------------------------
double StaticParams::gDENSITY_TOLERANCE = 0.05;

//-----------------------------------------
// threshold value of weight exponential term
// to swith apploximation weight form
// (to avoid round-off error in 1.0 - near equal 1.0 ~= 0)
//-----------------------------------------
double StaticParams::gEXPTERM_THRESHOLD = 1e-5;

//-----------------------------------------
// option for sampling method for interaction position
// 0 : sample flat in length [m]
// 1 : sample flat in columnd depth [g/m2]
// see Particle::SetFinalInteractionPosition
//-----------------------------------------
int StaticParams::gINTPOS_SAMPLE_OPT = 1;   

//-----------------------------------------
// option for interaction weight calculation
// 0 : use (total_crosssection * 1.0e-31) * 
//         (total_column_depth / PMASS) * Psurv;  (legacy)
// 1 : use (1 - Psurv)*norm*Psurv
// where
// Psurv = surviving probability from entrance of detection volume
//         to interaction position
// 1-Psurv = interaction probability within detection volume
// norm = normalization factor of Psurv function
// norm * Psurv compensates linear sampling of interaction vertex
// in meter.
// see Particle::SetFinalInteractionPosition
//-----------------------------------------
int StaticParams::gINTERACT_WEIGHT_OPT = 1;   

//-----------------------------------------
// option for calculationg crosssection * columndepth
// 0 : get total columndepth and multiply crosseection at 
//     the specified point (legacy)
// 1 : calculate columndepth * crosssection at each step and 
//     accumulate them over total steps
// see WeightedEvent::SetFinalInteractionPosition and
//     WeightedEvent::CalculateMaximumTotalInteractionProbability
//-----------------------------------------
int StaticParams::gCROSSSECTIONxCOLUMNDEPTH_OPT = 0;   
double StaticParams::gCROSSSECTIONxCOLUMNDEPTH_OPT_STEPSIZE = (10.0 * I3Units::m) ;   

//-----------------------------------------
// option to choose whether re-calculate impact param for each
// daughters or not
// 0 : always use primary's impact parameters (legacy)
// 1 : re-calculate for each daughters
// see Calculator::SetFinalInteractionPosition and
//     WeightedEvent::CalculateInteractionGeometry
//-----------------------------------------
int StaticParams::gIMPACTPARAM_OPT = 0;   


