//----------------------------------------------------------------------
// D. Jason Koskinen
// August 2012
//
// This module takes the vuvuzela parameters from an external text file
// and adds them to the DetectorStatus frame of a GCD file that does
// not have the values. The new I3FrameObject is a map of OMKey to 
// NoiseParams.
//----------------------------------------------------------------------

#include <fstream>

#include "vuvuzela/Inject.h"
#include "icetray/OMKey.h"

/* ******************************************************************** */ 
/* Constructor                                                          */
/** Adds the default values for each of the configuration parameters
 *
 *  \returns ctx The context from icetray
 *//******************************************************************* */ 
Inject::Inject(const I3Context& ctx) : I3ConditionalModule(ctx)
{

  InNoiseFileName_ = "$I3_SRC/vuvuzela/resources/data/parameters.dat";
  AddParameter("InputNoiseFile", "Name of the text file containing the vuvuzela noise parameters for the DOMs", InNoiseFileName_);

  AddOutBox("OutBox");

  log_debug("Enter vuvuzela Inject::Inject().");
}

/* ******************************************************************** */ 
/* Destructor                                                           */
/** The destructor for Vuvuzela.
*//******************************************************************* */ 
Inject::~Inject()
{
}


/* ******************************************************************** */ 
/* Configure                                                            */
/** Gets the values of each parameter. They're as follows:
 *
 * InputNoiseFile The .dat file containing the noise parameters for each DOM
 * OutputName The name of the NoiseParamsMap that will be added to the D frame
 *//******************************************************************* */ 
void Inject::Configure()
{
  GetParameter("InputNoiseFile", InNoiseFileName_);
}

/* ******************************************************************** */ 
// Calibration                                                       */
/** Runs the Inject module, adding a NoiseParamsMap to the Calibration
 *  frame. The object will be visible to all subsequent frames.
 *
 *  \param frame The frame to process
 *//******************************************************************* */ 
void Inject::Calibration(I3FramePtr frame){
  log_debug(" Entering vuvuzela Inject::Calibration()");

  // Get the calibration
  I3Calibration calib = frame->Get<I3Calibration>();
  //------------------------------
  // The format for the input noise file is expected to be 
  //  String OM ThermalRate DecayRate ScintillationHits ScintillationMean ScintillationSigma 
  // each separated by a space. Units are in Hz (not 1/ns) and Log10(dt/ns)
  //------------------------------

  std::ifstream cinn;
  cinn.open(InNoiseFileName_.c_str());

  int stng, OM;
  double ThermalRate, DecayRate, ScintillationMean, ScintillationSigma, ScintillationHits;

  //------------------------------
  // Read in the vuvuzela noise parameters
  // from an external .txt file and add
  // the values to an OMKey, NoiseParams
  // map which is stored as an I3FrameObject
  // in the Calibration frame.
  //------------------------------
  while ( cinn.good()){
    cinn >> stng;
    cinn >> OM;
    cinn >> ThermalRate;
    cinn >> DecayRate;
    cinn >> ScintillationHits;
    cinn >> ScintillationMean;
    cinn >> ScintillationSigma;

    log_trace(" OMKey(%i, %i): ThermalRate=%f, DecayRate=%f, ScintillationMean=%f, ScintillationSigma=%f, ScintillationHits=%f",
	      stng, OM, ThermalRate, DecayRate, ScintillationMean, ScintillationSigma, ScintillationHits);

    
    OMKey dom(stng, OM);
    I3Map<OMKey,I3DOMCalibration>::iterator calibIter = calib.domCal.find(dom);
    
    // Don't generate noise for DOMs that have no calibration/status record
    if(calibIter == calib.domCal.end()) continue;
    
    calibIter->second.SetDomNoiseThermalRate(ThermalRate * I3Units::hertz);
    calibIter->second.SetDomNoiseDecayRate(DecayRate * I3Units::hertz);
    calibIter->second.SetDomNoiseScintillationMean(ScintillationMean);
    calibIter->second.SetDomNoiseScintillationSigma(ScintillationSigma);
    calibIter->second.SetDomNoiseScintillationHits(ScintillationHits);
  }
  
  I3CalibrationConstPtr newcalib = I3CalibrationConstPtr(new I3Calibration(calib));
  frame->Delete("I3Calibration");
  frame->Put("I3Calibration", newcalib);

  PushFrame( frame, "OutBox");
  log_debug(" Exiting vuvuzela Inject::Calibration()");

}

I3_MODULE(Inject);
