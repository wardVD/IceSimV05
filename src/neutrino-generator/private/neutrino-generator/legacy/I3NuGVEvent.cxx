
#include "neutrino-generator/legacy/I3NuGVEvent.h"
#include "neutrino-generator/utils/Utils.h"
#include "neutrino-generator/utils/Calculator.h"
#include "neutrino-generator/Steering.h"
#include <string>
#include <iostream>

using namespace std;

//______________________________________________________________
void I3NuGVEvent::FillMCWeights(I3FramePtr frame)
{
  // get writable copy of weightmap in frame
  // this function make a copy of existing weight map and
  // replace old one with the new weight map
  std::string weightname = steer_ptr_->GetWeightDictName();
  I3MapStringDoublePtr wmap = 
         nugen::Utils::PutWeightDict(frame, weightname);

  // check wmap
  I3MapStringDouble::iterator iter;

  //Here We fill event-wise weight related variables.
  //first copy 
  const double injectionR         = steer_ptr_->GetInjectionRadius();
  const double activeLengthBefore = steer_ptr_->GetActiveHeightBefore();
  const double activeLengthAfter  = steer_ptr_->GetActiveHeightAfter();
  const double cylinderR          = steer_ptr_->GetCylinderRadius();
  const double cylinderH          = steer_ptr_->GetCylinderHeight();

  if (injectionR > 0) {
     (*wmap)["CylinderRadius"] = injectionR;
     (*wmap)["CylinderHeight"] = activeLengthBefore + activeLengthAfter;
  } else {
     (*wmap)["CylinderRadius"] = cylinderR;
     (*wmap)["CylinderHeight"] = cylinderH;
  }

  //(*wmap)["AutoExtension"]     = autoextend;
  (*wmap)["NEvents"] = steer_ptr_->GetNGen();

  /**
   * By default the weight is set to 0
   */
  (*wmap)["OneWeight"] = 0.;
  (*wmap)["OneWeightPerType"] = 0.;
  (*wmap)["TotalWeight"] = 0.;

  nugen::ParticlePtr inicep = GetInIceParticle();
  if (inicep) {

    // fill InIce energy. InIce neutrino direction is same as primary.
    (*wmap)["InIceNeutrinoEnergy"] = inicep->GetEnergy();
    (*wmap)["InIceNeutrinoType"] = inicep->GetType();

    // fill interaction info
    nugen::InteractionInfo &intinfo = inicep->GetInteractionInfoRef();
    intinfo.FillInteractionInfo(wmap);

    // fill interation geom info
    nugen::InteractionGeo  &intgeo = inicep->GetInteractionGeoRef();
    intgeo.FillInteractionGeo(wmap);
    
    // Calculate and store one weight to I3WeightDict.
    nugen::Utils::StoreOneWeight(wmap, intinfo);

  }

  SetMCWeightDictPtr(wmap); 
}
