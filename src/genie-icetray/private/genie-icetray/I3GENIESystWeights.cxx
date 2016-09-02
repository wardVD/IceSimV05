/**
 * Helper class for calculation of event weights based on genie
 * systematic uncertainties.
 *
 * Writted By: dadwyer@lbl.gov 2014/04/14
 * Last Updated: dadwyer@lbl.gov 2014/04/14
 */

#include "genie-icetray/I3GENIESystWeights.h"
#include "ReWeight/GSystSet.h"
#include "ReWeight/GSyst.h"
#include "ReWeight/GReWeightNuXSecNCEL.h"
#include "ReWeight/GReWeightNuXSecCCQE.h"
#include "ReWeight/GReWeightNuXSecCCRES.h"
#include "ReWeight/GReWeightNuXSecCOH.h"
#include "ReWeight/GReWeightNonResonanceBkg.h"
#include "ReWeight/GReWeightFGM.h"
#include "ReWeight/GReWeightDISNuclMod.h"
#include "ReWeight/GReWeightResonanceDecay.h"
#include "ReWeight/GReWeightFZone.h"
#include "ReWeight/GReWeightINuke.h"
#include "ReWeight/GReWeightAGKY.h"
#include "ReWeight/GReWeightNuXSecCCQEvec.h"
#include "ReWeight/GReWeightNuXSecNCRES.h"  
#include "ReWeight/GReWeightNuXSecDIS.h"

using namespace genie;

//___________________________________________________________________________
// Constructor
I3GENIESystWeights::I3GENIESystWeights()
{
    ;
}


//___________________________________________________________________________
// Destructor
I3GENIESystWeights::~I3GENIESystWeights()
{
    ;
}

//___________________________________________________________________________
// Set the list of systematics to calculate, by name (See GENIE GSyst.h)
void I3GENIESystWeights::SetSystematics(const std::vector<std::string>& systNames)
{
  systNames_ = systNames;
}

//___________________________________________________________________________
// Set the syst steps to calculate, in units of sigma (e.g. [0,-2,-1,1,2]) 
void I3GENIESystWeights::SetSteps(const std::vector<double>& systSteps)
{
  systSteps_ = systSteps;
}


//___________________________________________________________________________
// Configure this calculator
void I3GENIESystWeights::Configure()
{
  // Setup calculator for each systematic setting
  std::vector<std::string>::iterator curSystName = systNames_.begin();
  std::vector<std::string>::iterator endSystName = systNames_.end();
  for(;curSystName!=endSystName;curSystName++){
    log_info("Configuring %s",(*curSystName).c_str());
    rew::GSyst_t systId = rew::GSyst::FromString(*curSystName);
    // Initialize calculators
    std::vector< boost::shared_ptr<rew::GReWeight> > calculators;
    // Create one calculator for each tweaked value of this systematic
    std::vector<double>::iterator curStep = systSteps_.begin();
    std::vector<double>::iterator endStep = systSteps_.end();
    for(;curStep!=endStep;curStep++){
      // Configure this calculator
        boost::shared_ptr<rew::GReWeight> calc(new rew::GReWeight());
      // Add specific reweights, based on gRwght1Scan example script
      calc->AdoptWghtCalc( "xsec_ncel",       new GReWeightNuXSecNCEL      );
      calc->AdoptWghtCalc( "xsec_ccqe",       new GReWeightNuXSecCCQE      );
      calc->AdoptWghtCalc( "xsec_ccqe_vec",   new GReWeightNuXSecCCQEvec   );
      calc->AdoptWghtCalc( "xsec_ccres",      new GReWeightNuXSecCCRES     );
      calc->AdoptWghtCalc( "xsec_ncres",      new GReWeightNuXSecNCRES     );
      calc->AdoptWghtCalc( "xsec_nonresbkg",  new GReWeightNonResonanceBkg );
      calc->AdoptWghtCalc( "xsec_coh",        new GReWeightNuXSecCOH       );
      calc->AdoptWghtCalc( "xsec_dis",        new GReWeightNuXSecDIS       );
      calc->AdoptWghtCalc( "nuclear_qe",      new GReWeightFGM             );
      calc->AdoptWghtCalc( "nuclear_dis",     new GReWeightDISNuclMod      );
      calc->AdoptWghtCalc( "hadro_res_decay", new GReWeightResonanceDecay  );
      calc->AdoptWghtCalc( "hadro_fzone",     new GReWeightFZone           );
      calc->AdoptWghtCalc( "hadro_intranuke", new GReWeightINuke           );
      calc->AdoptWghtCalc( "hadro_agky",      new GReWeightAGKY            );

      GSystSet& syst = calc->Systematics();
      syst.Init( systId );

      // Shamelessly adopt fine-tuning code from gRwght1Scan. -DD
      if(systId == rew::kXSecTwkDial_MaCCQE) {
	// By default GReWeightNuXSecCCQE is in `NormAndMaShape' mode 
	// where Ma affects the shape of dsigma/dQ2 and a different param affects the normalization
	// If the input is MaCCQE, switch the weight calculator to `Ma' mode
	rew::GReWeightNuXSecCCQE* rwccqe =      
	  dynamic_cast<rew::GReWeightNuXSecCCQE*> (calc->WghtCalc("xsec_ccqe"));
	rwccqe->SetMode(rew::GReWeightNuXSecCCQE::kModeMa);
      }
      if(systId == rew::kXSecTwkDial_MaCCRES 
	 || systId == rew::kXSecTwkDial_MvCCRES) {
	// As above, but for the GReWeightNuXSecCCRES weight calculator
	rew::GReWeightNuXSecCCRES* rwccres = 
	  dynamic_cast<rew::GReWeightNuXSecCCRES*> (calc->WghtCalc("xsec_ccres"));
	rwccres->SetMode(rew::GReWeightNuXSecCCRES::kModeMaMv);
      }
      if(systId == rew::kXSecTwkDial_MaNCRES 
	 || systId == rew::kXSecTwkDial_MvNCRES) {
	// As above, but for the GReWeightNuXSecNCRES weight calculator
	rew::GReWeightNuXSecNCRES* rwncres = 
	  dynamic_cast<rew::GReWeightNuXSecNCRES*> (calc->WghtCalc("xsec_ncres"));  
	rwncres->SetMode(rew::GReWeightNuXSecNCRES::kModeMaMv);
      }
      if(systId == rew::kXSecTwkDial_AhtBYshape  
	 || systId == rew::kXSecTwkDial_BhtBYshape 
	 || systId == rew::kXSecTwkDial_CV1uBYshape 
	 || systId == rew::kXSecTwkDial_CV2uBYshape ) {
	// Similarly for the GReWeightNuXSecDIS weight calculator.
	// There the default behaviour is for the Aht, Bht, CV1u and CV2u Bodek-Yang
	// params to affects both normalization and dsigma/dxdy shape.
	// Switch mode if a shape-only param is specified.
	rew::GReWeightNuXSecDIS* rwdis = 
	  dynamic_cast<rew::GReWeightNuXSecDIS*> (calc->WghtCalc("xsec_dis"));  
	rwdis->SetMode(rew::GReWeightNuXSecDIS::kModeABCV12uShape);
      }

      // Reconfigure weight calculator with tweaked systematic value
      syst.Set( systId, (*curStep) );
      calc->Reconfigure();
      calculators.push_back(calc);
    }
    // Add calculators for this systematic to collection
    systCalculators_[(*curSystName)] = calculators;
  }
  return;
}


//___________________________________________________________________________
// Calculates the systematic weights for one event, and write to result dict
void I3GENIESystWeights::Calculate(const genie::EventRecord &event, 
				     I3GENIEResultDict &gstMap)
{
  std::map<std::string, std::vector< boost::shared_ptr<rew::GReWeight> > >::iterator curSyst = systCalculators_.begin();
  std::map<std::string, std::vector< boost::shared_ptr<rew::GReWeight> > >::iterator endSyst = systCalculators_.end();
  for(;curSyst!=endSyst;curSyst++){
    std::string weightName = "rw_";
    weightName += (*curSyst).first;
    std::vector< boost::shared_ptr<rew::GReWeight> >& calculators = (*curSyst).second;
    std::vector< boost::shared_ptr<rew::GReWeight> >::iterator curCalc = 
      calculators.begin();
    std::vector< boost::shared_ptr<rew::GReWeight> >::iterator endCalc = 
      calculators.end();
    std::vector<double> weights;
    for(;curCalc!=endCalc;curCalc++){
      // Calculate systematic weight for this event
      weights.push_back( (*curCalc)->CalcWeight(event) );
    }
    // Record systematic weights in genie result dict
    gstMap.insert(std::make_pair(weightName,weights));
  }

  return;
}
