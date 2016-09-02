// $id$

#include "vuvuzela/Vuvuzela.h"
#include <cmath>

/* ******************************************************************** */ 
/* Constructor                                                          */
/** Adds the default values for each of the configuration parameters
 *
 *  \returns ctx The ctx from icetray
 *//******************************************************************* */ 
Vuvuzela::Vuvuzela(const I3Context& ctx) : I3Module(ctx)
{
  
  inputHitSeriesMapName_="I3MCPESeriesMap";
  AddParameter("InputHitSeriesMapName","Name of the map to add hits to",inputHitSeriesMapName_);
  
  outputHitSeriesMapName_="";
  AddParameter("OutputHitSeriesMapName","Output map with noise hits",outputHitSeriesMapName_);

  startWindow_=-10 * I3Units::microsecond;  
  AddParameter("StartWindow","Time before MC hits to be filled with noise",startWindow_);
  
  endWindow_=10 * I3Units::microsecond;
  AddParameter("EndWindow","Time after MC hits to be filled with noise",endWindow_);
  
  useEventHeaderBounds_ = false;
  AddParameter("UseEventHeaderTimeWindow", "Times are typically used relative to the first and last pre-existing hit in the map. This option will force times to be relative to t=0 and last for the length of the eventheader", useEventHeaderBounds_);
  
  inputI3EventHeaderName_ = "I3EventHeader";
  AddParameter("InputI3EventHeaderName", "Name of I3EventHeader", inputI3EventHeaderName_);
  
  icetop_=true;
  AddParameter("IceTop","Generate noise for IceTop",icetop_);
  
  inIce_=true;
  AddParameter("InIce","Generate noise for InIce",inIce_);

  scaleFactor_=1.0;
  AddParameter("ScaleFactor","Factor to scale noise up or down for ALL DOMs", scaleFactor_);

  deepCoreScaleFactor_=1.0;
  AddParameter("DeepCoreScaleFactor","Factor to scale noise up or down for DeepCore DOMs", deepCoreScaleFactor_);

  AddParameter("DOMsToExclude","List of OMKeys of DOMs we don't want to simulate",excludeList_);

  thermalRate_= 173.456*I3Units::hertz;
  AddParameter("ThermalRate", "Constant thermal noise rate to use.", thermalRate_);

  decayRate_= 56.943*I3Units::hertz; 
  AddParameter("DecayRate", "Constant radioactive decay rate to use.", decayRate_);

  scintillationMean_=4.395;
  AddParameter("ScintillationMean", "Constant mean of the lognormal describing scintillation in Log10(dt/ns)", scintillationMean_);

  scintillationSigma_=1.777;
  AddParameter("ScintillationSigma", "Constant sigma of the lognormal describing scintillation in Log10(dt/ns)", scintillationSigma_);

  scintillationHits_=8.072; 
  AddParameter("ScintillationHits", "Constant Mean number of hits from the scintillation lognormal component", scintillationHits_);

  useIndividual_=true;
  AddParameter("UseIndividual", "Use the individual DOM Parameters?", useIndividual_);

  simulateNewDoms_=false;
  AddParameter("SimulateNewDOMs", "Should DOMs with no calibration entry be simulated (ie, for PINGU)?", simulateNewDoms_);

  AddParameter("RandomService","Random Number Generator",randomService);

  disableLowDTcutoff_ = true;
  AddParameter("DisableLowDTCutoff", "Should hits with dt < 2 microseconds be added?", disableLowDTcutoff_);

  randomServiceName_ = std::string("I3RandomService");
  AddParameter("RandomServiceName","Name of RNG in the context",randomServiceName_);

  AddOutBox("OutBox");
}

/* ******************************************************************** */ 
/* Destructor                                                           */
/** The destructor for Vuvuzela.
*//******************************************************************* */ 
Vuvuzela::~Vuvuzela()
{
}

/* ******************************************************************** */ 
/* Configure                                                            */
/** Gets the values of each parameter. They're as follows:
 *
 * InputHitSeriesMap The name of the input hit series. If empty, will produce a noise-only hit series.
 * OutputHitSeriesMap The name of the output hit series. If empty, will replace the InputHitSeriesMap.
 * StartWindow The amount of time relative to the first pre-existing hit to simulate. Negative for "Before first hit".
 * EndWindow The amount of time relative to the last pre-existing hit to simulate. Positive for "After last hit".
 * IceTop Simulate noise in IceTop?
 * InIce Simulate noise in InIce?
 * ScaleFactor Used to scale the thermal and nonthermal rates.
 * DeepCoreScaleFactor Used to scale the noise rates for the HQE DOMs.
 * DOMsToExclude A list of OMKeys to skip simulating.
 * RandomService The name of the I3RandomService.
 * ThermalRate Thermal noise rate.
 * DecayRate The rate of decays that cause clusters.
 * ScintillationMean The mean of the lognormal describing scintillation in Log10(dt/ns)
 * ScintillationSigma The sigma of the lognormal describing scintillation in Log10(dt/ns)
 * ScintillationHits The expected number of hits from a cluster.
 * UseIndividual Use individual numbers for each DOM?
 * DisableLowDTCutoff Removes the truncation at 2 microseconds for the timing distribution of noise hits 
 *//******************************************************************* */ 
void Vuvuzela::Configure()
{
  GetParameter("InputHitSeriesMapName", inputHitSeriesMapName_);
  GetParameter("OutputHitSeriesMapName", outputHitSeriesMapName_);
  GetParameter("StartWindow", startWindow_);
  GetParameter("EndWindow", endWindow_);
  GetParameter("UseEventHeaderTimeWindow", useEventHeaderBounds_);
  GetParameter("InputI3EventHeaderName", inputI3EventHeaderName_);
  GetParameter("IceTop", icetop_);
  GetParameter("InIce", inIce_);
  GetParameter("ScaleFactor", scaleFactor_);
  GetParameter("DeepCoreScaleFactor", deepCoreScaleFactor_);
  GetParameter("DOMsToExclude", excludeList_);
  GetParameter("RandomService", randomService);
  GetParameter("RandomServiceName", randomServiceName_);
  GetParameter("ThermalRate", thermalRate_);
  GetParameter("DecayRate", decayRate_);
  GetParameter("ScintillationMean", scintillationMean_);
  GetParameter("ScintillationSigma", scintillationSigma_);
  GetParameter("ScintillationHits", scintillationHits_);
  GetParameter("UseIndividual", useIndividual_);
  GetParameter("SimulateNewDOMs", simulateNewDoms_);
  GetParameter("DisableLowDTCutoff", disableLowDTcutoff_);

  //   Check to see if valid values were passed
  if (endWindow_ < startWindow_)
    log_fatal("End time for generation is earlier than start!");

  if (endWindow_ - startWindow_ > 100 * I3Units::microsecond)
    log_debug("Window is longer than 100 microseconds. "
        "Make sure you know what you're doing!");

  // If using individual parameters, sanity check the values
  if (!useIndividual_){
    if (thermalRate_ < 0 || decayRate_ < 0)
      log_fatal("The thermal and non-thermal rate must be => 0!");
    
    if (scintillationHits_ < 0)
      log_fatal("The non-thermal mean must be => 0!");
  }

  if (inputHitSeriesMapName_.size() == 0 && outputHitSeriesMapName_.size() == 0)
    log_fatal("No input or output hit names configured.");

  if (inputHitSeriesMapName_.size() == 0)
    log_warn("No input map given. Producing noise-only events.");

  if (!disableLowDTcutoff_)
    log_warn("Low DT cutoff enabled! This is now considered abnormal!");

  if (useEventHeaderBounds_)
	  log_warn("Noise will be generated according to the length of the eventheader, i.e. "
		       "range = endtime - starttime. This is abnormal and probably wrong!");

  // Configure I3RandomService
  if (!randomService){
    randomService = context_.Get<I3RandomServicePtr>(randomServiceName_);
    if (!randomService)
      log_fatal("No Random Service configured!");
  }

  bufferMap.clear();
  firstTime = true;
  nhits = 0;
  bufferTime = 0 * I3Units::second;
}

/* ******************************************************************** */ 
/* Finish                                                               */
/** Runs after the final event. Prints out the number of total number 
 *  of hits added to all frames as well as the total livetime of noise 
 *  hits added.
 *//******************************************************************* */ 
void Vuvuzela::Finish()
{
  log_info("Produced %i MCPEs in %f seconds ", nhits, bufferTime/I3Units::second);
}


/* ******************************************************************** */ 
// DAQ                                                                  */
/** Runs the Vuvuzela module, adding noise to the frame. 
 *
 *  \param frame The frame to process
 *//******************************************************************* */ 
void Vuvuzela::DAQ(I3FramePtr frame)
{
  // Load the GCD first
  const I3Geometry& geometry = frame->Get<I3Geometry>();
  const I3Calibration& calib = frame->Get<I3Calibration>();
  
  // Get the hit series
  I3MCPESeriesMapConstPtr inputHitMap;
  std::pair<double, double> range(0,0);
  
  // Name passed for hit series
  if ( inputHitSeriesMapName_.size() ) {
    if (! frame->Has(inputHitSeriesMapName_)) {
      log_fatal("No hitSeriesMap with the name %s is present in the frame!",
                inputHitSeriesMapName_.c_str());
    }
    else {
      // Want to get the time range for the hit series while we're here
      inputHitMap = frame->Get<I3MCPESeriesMapConstPtr>(inputHitSeriesMapName_);
      if (!useEventHeaderBounds_) {
	range = GetTimeRange(inputHitMap); 
      }
    }
  }
  // Noise-Only mode
  else {   
    inputHitMap = I3MCPESeriesMapConstPtr(new I3MCPESeriesMap());
  }
  
  if (useEventHeaderBounds_) {
    if (!(frame->Has(inputI3EventHeaderName_))){
      log_fatal("No I3EventHeader with the name %s is present in the frame!", 
      inputI3EventHeaderName_.c_str());
    }
    else {
      I3EventHeaderConstPtr eventheader = frame->Get<I3EventHeaderConstPtr>(inputI3EventHeaderName_);
      startWindow_ = 0;
      endWindow_ = eventheader->GetEndTime() - eventheader->GetStartTime();
    }

  }
 
  // Set the bounds for the noise sampling
  double start = range.first + startWindow_;
  double stop = range.second + endWindow_;
  
  // The first time the DAQ method is called, fill the buffer with long dt hits
  if (firstTime){
    GetGoodDoms(geometry, calib);

    GetNoiseHits(calib,
                 -0.1 * I3Units::second,
                 0 * I3Units::second);
    
    firstTime = false;
    nhits = 0;
    log_trace("Filled the hit buffer.");
  }
  
  // Get the noise hits for this event
  I3MCPESeriesMapConstPtr noiseMap = GetNoiseHits(calib,
						  start, stop);
  
  // Add the generated hits to the old hitmap
  I3MCPESeriesMapConstPtr outputHitMap = AddHitMaps(inputHitMap, noiseMap);
  
  // If no output name is given, overwrite the input hit map. 
  if (outputHitSeriesMapName_.size() == 0) {
    frame->Delete(inputHitSeriesMapName_);
    frame->Put(inputHitSeriesMapName_, outputHitMap);
  }
  // otherwise, use the new name
  else {
    frame->Put(outputHitSeriesMapName_, outputHitMap);
  }
  
  // Increment the file livetime
  bufferTime += (stop-start);
  
  PushFrame(frame, "OutBox");
  
}


/* ******************************************************************** */ 
/* GetGoodDoms                                                          */
/** Loops through the geometry and saves a list of the good doms
 *  to simulate. This is only done once to avoid the std::find used
 *  in the excludeList search
 *
 *  \param geometry The I3Geometry used to loop over all DOMs.
 *  \returns A vector of OMKeys of DOMs to simulate.
 *//******************************************************************* */ 
void Vuvuzela::GetGoodDoms(const I3Geometry& geometry,
			   const I3Calibration& calibration)
{

  I3OMGeoMap::const_iterator geoIter;
  for( geoIter  = geometry.omgeo.begin(); 
       geoIter != geometry.omgeo.end(); 
       ++geoIter ){
    
    OMKey dom = geoIter->first;
    
    // Check for AMANDA/IceCube/InIce
    if( geoIter->second.omtype == I3OMGeo::AMANDA ) continue;
    if(!inIce_ && geoIter->second.omtype == I3OMGeo::IceCube ) continue;
    if(!icetop_ && geoIter->second.omtype == I3OMGeo::IceTop ) continue;
    
    //Don't generate hits for any DOMs we want to exclude
    // Uses STL binary_search method to search for this DOM in the exclude list
    if( std::find(excludeList_.begin(), excludeList_.end(), dom) != excludeList_.end() ){
      log_trace("EXCLUDING DOM(%d,%d) FROM SIMULATION!", 
		dom.GetString(), dom.GetOM());
      continue;
    }
    
    // Get information from the Calibration to set individual noise rates for every DOM
    if (!simulateNewDoms_){
      if (calibration.domCal.find(dom) == calibration.domCal.end()) continue;
    }
    
    // DOM is not AMANDA, is not in the excludeList, and has a calibration record.
    // Save this DOM to simulate
    goodDOMs.push_back(dom);

  } 

}



/* ******************************************************************** */ 
/* GetNoiseHits                                                        */
/** Fills the hit map with thermal and nonthermal noise
 *  for the entire period from start to stop and for each DOM.
 *
 *  \param calibration The I3Calibration for the file. Used to get HQE/QE status
 *  \param start The time for the event to begin
 *  \param stop The time for the event to end
 *  \returns An I3Map of OMKey to a vector of hit times to be used for sampling.
 *//******************************************************************* */ 
I3MCPESeriesMapConstPtr Vuvuzela::GetNoiseHits(const I3Calibration& calibration,
					       double start, double stop){
  
  I3MCPESeriesMap *noiseMap = new I3MCPESeriesMap();

  std::vector<OMKey>::iterator domIter;
  for(domIter = goodDOMs.begin(); domIter != goodDOMs.end(); ++domIter){
    
    OMKey dom = *domIter;
    I3Map<OMKey,I3DOMCalibration>::const_iterator calibIter = calibration.domCal.find(dom);
    
    // Create the variables for this DOM
    double currentThermalRate = thermalRate_;
    double currentDecayRate = decayRate_ ;
    double currentScintillationHits = scintillationHits_;
    double currentScintillationMean = scintillationMean_;
    double currentScintillationSigma = scintillationSigma_;
	
    // Get the values for thermal, decay, mean, sigma, and nhits
    if (useIndividual_ && (calibIter != calibration.domCal.end())){
      // Grab the individual DOM's parameters
      currentThermalRate = calibIter->second.GetDomNoiseThermalRate();
      currentDecayRate = calibIter->second.GetDomNoiseDecayRate();
      currentScintillationHits = calibIter->second.GetDomNoiseScintillationHits();
      currentScintillationMean = calibIter->second.GetDomNoiseScintillationMean();
      currentScintillationSigma = calibIter->second.GetDomNoiseScintillationSigma();
      
      // Check for NaNs
      if ( (std::isnan(currentThermalRate) || 
	    std::isnan(currentDecayRate) ||
	    std::isnan(currentScintillationHits) ||
	    std::isnan(currentScintillationMean) ||
	    std::isnan(currentScintillationSigma)) )
	{
	  if (simulateNewDoms_){
	    currentThermalRate = thermalRate_;
	    currentDecayRate = decayRate_ ;
	    currentScintillationHits = scintillationHits_;
	    currentScintillationMean = scintillationMean_;
	    currentScintillationSigma = scintillationSigma_;
	  }
	  else{
	    log_fatal("DOM %02i-%02i has no Vuvuzela parameters in GCD file! If you want to simply use the default values, enable SimulateNewDOMs.", 
		      dom.GetString(),
		      dom.GetOM());
	  }
	}

    }

    // Multiply the rates (not anything else) by the scale factor.
    currentThermalRate  *= scaleFactor_;
    currentDecayRate    *= scaleFactor_;

    // For DeepCore: Scale noise for DeepCore(HQE) DOMs
    if (  calibIter->second.GetRelativeDomEff() > 1 ){
      currentThermalRate *= deepCoreScaleFactor_;
      currentDecayRate    *= deepCoreScaleFactor_;
    }

    // Grab the buffer for this DOM
    std::set<double> bufferHits;
    if( !firstTime )
      bufferHits = bufferMap[dom];

    // Get the thermal hits
    MakeThermalHits(randomService, 
		    bufferHits,
		    bufferTime,
		    currentThermalRate, 
		    start, 
		    stop);
    
    // And the decay + scintillation hits
    MakeNonThermalHits(randomService, 
    		       bufferHits,
		       bufferTime,
		       currentDecayRate, 
		       currentScintillationHits,
		       currentScintillationMean,
		       currentScintillationSigma,
		       start, stop,
		       disableLowDTcutoff_);

    // At this point, the buffer is sorted so that we can pop_back with amortized O(1) time
    // Pop all of the hits in the right time range to the MCPESeries
    I3MCPESeries hitSeries;
    while(!bufferHits.empty()){
      
      // The time for this hit, corrected with start and buffer times
      double hitTime = *(bufferHits.begin()) - bufferTime + start; 

      if( hitTime > stop) break;
      
      // Now make an I3MCPESeries with the hits
      I3MCPE hit;
      hit.time = hitTime;
      hit.npe = 1;
      hitSeries.push_back(hit);

      // Increment the hit counter and remove the hit
      nhits++;
      bufferHits.erase(bufferHits.begin());
    }
    
    // Add the hit series to the map and save the buffer
    if(hitSeries.size() > 0) (*noiseMap)[dom] = hitSeries;
    bufferMap[dom] = bufferHits;
  }
  
  I3MCPESeriesMapConstPtr hitMapPtr(noiseMap);
  return hitMapPtr;

}

I3_MODULE(Vuvuzela);
