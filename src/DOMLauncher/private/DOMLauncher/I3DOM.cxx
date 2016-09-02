/**
 * class: I3InIceDOMObject
 *
 * @version $Id: $
 *
 * @date: $Date: $
 *
 * @author Samuel Flis <samuel.d.flis@gmail.com>
 *
 * (c) 2011,2012 IceCube Collaboration
 */

#include <cfloat>
#include <queue>
#include <boost/foreach.hpp>

#include "phys-services/I3RandomService.h"
#include "dataclasses/I3DOMFunctions.h"
#include "dataclasses/physics/I3DOMLaunch.h"
#include "dataclasses/calibration/I3DOMCalibration.h"
#include "dataclasses/status/I3DOMStatus.h"
#include "dataclasses/geometry/I3OMGeo.h"

#include "DOMLauncher/I3DOM.h"
using namespace domlauncherutils::detail;
double const I3DOM::HLCReadoutTime                   = 6400*I3Units::ns;//[ns]
double const I3DOM::ATWDReadoutDigitizeTime          = 29000*I3Units::ns;//[ns]
double const I3DOM::ATWDRestartTime                  = 225*I3Units::ns;//[ns]
double const I3DOM::ATWDClearTime                    = 950*I3Units::ns;//[ns]
double const I3DOM::clockCycle                       = 25*I3Units::ns;//[ns]
double const I3DOM::delayLine                        = 75*I3Units::ns;//[ns]
double const I3DOM::lcWait                           = 350*I3Units::ns;//[ns]
double       I3DOM::beaconLaunchRate                 = 0.6*I3Units::hertz;
///Number of samples/bins in FADC digitization.
unsigned int const I3DOM::nFADCBins                  = 256;
///Number of samples/bins in ATWD digitization.
unsigned int const I3DOM::nATWDBins                  = 128;
///Number of samples/bins to compute coarse charge stamp.
unsigned int const I3DOM::nFADCCoarseChargeStampBins = 16;
///Threshold in number of counts to trigger digitization of the next ATWD channel.
unsigned int const I3DOM::ATWDThreshold              = 768;//counts
///The dynamic range of the FADC and ATWD digitization in number of counts.
unsigned int const I3DOM::digitizerDynamicRange      = 1023;// 2^10-1=1023
///Mean of the gaussian electronic noise for the FADC, taken from DOMsimulator.
double const I3DOM::FADCNoiseMean                    = 0;//counts
///Variance of the gaussian electronic noise for the FADC, taken from DOMsimulator.
double const I3DOM::FADCNoiseVariance                = 0.5;//counts
///Mean of the gaussian electronic noise for the ATWD, taken from DOMsimulator.
double const I3DOM::ATWDNoiseMean                    = 0;//counts
///Variance of the gaussian electronic noise for the ATWD, taken from DOMsimulator.
double const I3DOM::ATWDNoiseVariance                = 0.8;//counts
double I3DOM::globalTimeStatic_ = NAN;
std::map<I3DOMCalibration::DroopedSPETemplate, InterpolatedSPETemplatePtr> I3DOM::speTemplateMapStatic_;

I3DOM::I3DOM(boost::shared_ptr<I3RandomService> rng,
          const OMKey& om,
          double &globalTime,
          domlauncherutils::PulseTemplateMap &speTemplateMap) :
  waveform_(25*I3Units::ns),//25 ns bins are used when merging pulses
  rng_(rng),
  domId_(om),
  mergePulses_(true),
  pulses_(0),
  speTemplateMap_(speTemplateMap),
  lcHighTime_(NAN),
  globalTime_(globalTime),
  domTime_(NAN),
  ATWDa_alive_(true),
  ATWDb_alive_(true),
  busy_(false),
  busyTo_(NAN),
  ATWDaDeadTo_(NAN),
  ATWDbDeadTo_(NAN),
  pulseTemplatePeak_(NAN),
  timeOfPulseTemplatePeak_(NAN),
  discriminatorThreshold_(NAN),
  useTabulation_(true),
  droopedPulseTemplates_(true)
{}

I3DOM::I3DOM(I3RandomServicePtr rng, const OMKey& om):
  waveform_(25*I3Units::ns),//25 ns bins are used when merging pulses
  rng_(rng),
  domId_(om),
  mergePulses_(true),
  pulses_(0),
  speTemplateMap_(speTemplateMapStatic_),
  lcHighTime_(NAN),
  globalTime_(globalTimeStatic_),
  domTime_(NAN),
  ATWDa_alive_(true),
  ATWDb_alive_(true),
  busy_(false),
  busyTo_(NAN),
  ATWDaDeadTo_(NAN),
  ATWDbDeadTo_(NAN),
  pulseTemplatePeak_(NAN),
  timeOfPulseTemplatePeak_(NAN),
  discriminatorThreshold_(NAN),
  useTabulation_(true),
  droopedPulseTemplates_(true)
{}

I3DOM::I3DOM():
  waveform_(25*I3Units::ns),//25 ns bins are used when merging pulses
  mergePulses_(true),
  pulses_(0),
  speTemplateMap_(speTemplateMapStatic_),
  lcHighTime_(NAN),
  globalTime_(globalTimeStatic_),
  domTime_(NAN),
  ATWDa_alive_(true),
  ATWDb_alive_(true),
  busy_(false),
  busyTo_(NAN),
  ATWDaDeadTo_(NAN),
  ATWDbDeadTo_(NAN),
  pulseTemplatePeak_(NAN),
  timeOfPulseTemplatePeak_(NAN),
  discriminatorThreshold_(NAN),
  useTabulation_(true),
  droopedPulseTemplates_(true)
{}

void I3DOM::DOMCalibration::Configure(const I3DOMCalibration& cal){
  frontEndImpedance = cal.GetFrontEndImpedance();
  atwdGains[0] = cal.GetATWDGain(0);
  atwdGains[1] = cal.GetATWDGain(1);
  atwdGains[2] = cal.GetATWDGain(2);
  atwdDeltaT[0] = cal.GetATWDDeltaT(0);
  atwdDeltaT[1] = cal.GetATWDDeltaT(1);

  for(int chip(0); chip < 2; ++chip)
    for(int channel(0); channel < 3; ++channel)
      for(int bin(0); bin < 128; ++bin)
        atwdBinCalib[chip][channel][bin] = cal.GetATWDBinCalibSlope(chip,channel,bin);

  for(int chip(0); chip < 2; ++chip)
      for(int channel(0); channel < 3; ++channel){
        // 0.5 counts are added to the baseline since the baseline measurement
        // is biased w.r.t true baseline by 0.5 counts due to flooring in the
        // digitizing process.
        atwdBeaconBaseline[chip][channel] = cal.GetATWDBeaconBaseline(chip,channel) + 0.5;
      }
  fadcGain = cal.GetFADCGain();
  fadcDeltaT = cal.GetFADCDeltaT();
  // The fADC baseline is biased in the same way as the ATWD (see above comment)
  // and therefore 0.5 counts are added.
  fadcBeaconBaseline = cal.GetFADCBeaconBaseline() + 0.5;
}

void I3DOM::DOMStatus::Configure(const I3DOMStatus& stat){
  lcWindowPre = stat.lcWindowPre;
  lcWindowPost = stat.lcWindowPost;
  statusATWDa = stat.statusATWDa;
  statusATWDb = stat.statusATWDb;
  lcMode = stat.lcMode;
  txMode = stat.txMode;
  lcSpan = stat.lcSpan;
  domGainType = stat.domGainType;

  if(lcSpan > 2 ){
    log_error("The current cable corrections aren't correct for LC spans > 2.");
  }
}

bool I3DOM::Configure(const I3DOMCalibration& cal,
                      const I3DOMStatus& stat,
                      double threshold){

  domCal_.Configure(cal);
  domStat_.Configure(stat);

  speMean_ = SPEMean(stat,cal);
  transitTime_ = TransitTime(stat,cal);
  for(int chip(0); chip < 2; ++chip)
    atwdSamplingRate_[chip] = ATWDSamplingRate (chip, stat, cal) / I3Units::ns;

  I3DOMCalibration::DroopedSPETemplate discTemplate = cal.DiscriminatorPulseTemplate(true);

  double max = 0;
  int index = 0;
  //Fast scan to find the maximum with precision of 1 ns
  for( int i = -40; i<40; i++){
    if(discTemplate(i) > max) {
      max = discTemplate(i);
      index = i;
    }
  }

  //Finding the maximum with bisection method with 1e-6 ns precision
  const double epsilon = 0.000001;
  const double h = epsilon*0.1;
  double lPoint = index -3;//left point [ns]
  double rPoint = index +3;//right point [ns]
  double interval = rPoint - lPoint;
  double mPoint = interval/2 + lPoint;

  while(interval > epsilon){
    mPoint = interval/2 + lPoint;
    double mSlope = discTemplate(mPoint) - discTemplate(mPoint+h);
    double rSlope = discTemplate(rPoint) - discTemplate(rPoint+h);
    if(mSlope*rSlope < 0) lPoint = mPoint;
    else rPoint = mPoint;
    interval = rPoint - lPoint;
  }
  double pulseTemplatePeak = discTemplate(mPoint);
  timeOfPulseTemplatePeak_ = mPoint;

  discriminatorThresholdFraction_ = threshold/( discTemplate(mPoint) *
                                                speMean_ *
                                                domCal_.frontEndImpedance );

  discriminatorThreshold_ = pulseTemplatePeak * discriminatorThresholdFraction_;

  discriminatorDelay_ = transitTime_ - delayLine;

  FetchSPETemplates(cal);

  bool success(true);
  if(std::isnan(discriminatorThreshold_)){
    log_error("NaN discriminator threshold on DOM %s.",domId_.str().c_str());
    success = false;
  }

  if(std::isnan(transitTime_)){
    log_error("Transit time is NAN for DOM %s: DOM discarded",domId_.str().c_str());
    success = false;
  }
  //In the future we might have individual RapCal time calibration uncertainties for each
  //but for now this will do. (2014: S. Klien suggested 2 ns)
  rapCalUncertainty_ = 2*I3Units::ns;
  rapCalShift_ = rng_->Gaus(0,rapCalUncertainty_);

  //Setting a first clock cycle phase
  currentClockPhase_ = rng_->Uniform() * clockCycle;
  //Setting the beacon launch phase
  beaconLaunchPhase_ = int64_t(rng_->Uniform()/beaconLaunchRate);
  //Moving it so it matches the clock cyclce phase.
  beaconLaunchPhase_ -= int64_t(beaconLaunchPhase_)%int64_t(clockCycle)-currentClockPhase_;
  return success;
}

void I3DOM::Reset(bool full){
    //for normal simulation (one event one frame) the DOM will be
    //fully reset but for Multi Frame Events only the
    //generated DOMLaunches will be cleared.

    if(full){//erasing the memory of the DOM
        ATWDa_alive_ = true;
        ATWDb_alive_ = true;
        busy_ = false;
        discrXings_.clear();
        busyTo_ = NAN;
        ATWDaDeadTo_ = NAN;
        ATWDbDeadTo_ = NAN;
        lcHighTime_ = NAN;
        globalTime_ = NAN;
        domTime_ = NAN;
        pulses_ = 0;
        //This should be fixed to change continuosly for long frames.
        rapCalShift_ = rng_->Gaus(0,rapCalUncertainty_);
        currentClockPhase_ = rng_->Uniform() * clockCycle;
        beaconLaunchPhase_ = int64_t(rng_->Uniform()/beaconLaunchRate);
        beaconLaunchPhase_ -= int64_t(beaconLaunchPhase_)%int64_t(clockCycle)-currentClockPhase_;
        waveform_.Clear();

    }
    else{

        waveform_.Buffer(globalTime_);
    }


    //Always clear the DOMLaunches.
    domLaunches_.clear();
}

void I3DOM::FetchSPETemplates(const I3DOMCalibration& domcal){

  bool drooped = droopedPulseTemplates_;

  //Fetching discriminator pulse template
  I3DOMCalibration::DroopedSPETemplate tmpTemplate =
    domcal.DiscriminatorPulseTemplate(drooped);

  std::map<I3DOMCalibration::DroopedSPETemplate, InterpolatedSPETemplatePtr>::iterator
    speTemplate = speTemplateMap_.find(tmpTemplate);

  if(speTemplate == speTemplateMap_.end()){

    InterpolatedSPETemplatePtr tmpIntSPETemplPtr = InterpolatedSPETemplatePtr
      (new InterpolatedSPETemplate(domcal.DiscriminatorPulseTemplate(drooped)));

    speTemplateMap_[tmpTemplate] = tmpIntSPETemplPtr;
    discSPETemplatePtr_ = tmpIntSPETemplPtr;

    if(useTabulation_){
      discSPETemplatePtr_->Tabulate(5000,
                                    1000,
                                    -10.0*I3Units::ns,
                                    60.0*I3Units::ns,
                                    6400.0*I3Units::ns,
                                    1*I3Units::second);
    }
  }else{
    discSPETemplatePtr_ = speTemplate->second;
  }

  //Fetching ATWD pulse templates
  for(int i = 0; i<3; i++){

    tmpTemplate = domcal.ATWDPulseTemplate(i, drooped);
    speTemplate = speTemplateMap_.find(tmpTemplate);

    if(speTemplate == speTemplateMap_.end() ){

      InterpolatedSPETemplatePtr tmpIntSPETemplPtr = InterpolatedSPETemplatePtr
        (new InterpolatedSPETemplate(domcal.ATWDPulseTemplate(i, drooped)));
      speTemplateMap_[ tmpTemplate ] = tmpIntSPETemplPtr;

      if(useTabulation_)
        tmpIntSPETemplPtr->Tabulate(5000,
                                    1000,
                                    -10.0*I3Units::ns,
                                    80.0*I3Units::ns,
                                    6400.0*I3Units::ns,
                                    1*I3Units::second );
      atwdSPETemplatePtr_[i] = tmpIntSPETemplPtr;
    }else{
      atwdSPETemplatePtr_[i] = speTemplate->second;
    }
  }

  //Fetching FADC pulse template
  speTemplate = speTemplateMap_.find(domcal.FADCPulseTemplate(drooped));
  if(speTemplate == speTemplateMap_.end()){

    InterpolatedSPETemplatePtr tmpIntSPETemplPtr =
      InterpolatedSPETemplatePtr(new InterpolatedSPETemplate(domcal.FADCPulseTemplate(drooped)));

    speTemplateMap_[domcal.FADCPulseTemplate(drooped)] = tmpIntSPETemplPtr;
    fadcSPETemplatePtr_ = tmpIntSPETemplPtr;

    if(useTabulation_){
      fadcSPETemplatePtr_->Tabulate(3000,
                                    2000,
                                    0*I3Units::ns,
                                    400.0*I3Units::ns,
                                    6400.0*I3Units::ns,
                                    1*I3Units::second
                                    );
    }
  }else{
    fadcSPETemplatePtr_ = speTemplate->second;
  }
}
/**
* The I3DOM::Discriminator is responsible to simulate the discriminator by
* creating DiscCrosss for each clockCycle the input signal is above threshold. These
* are then put in a global discriminator crossings stream that defines the time
* evolution of the event.
*/
void I3DOM::Discriminator(const std::vector<I3MCPulse>& pulses,
                          domlauncherutils::DCStream& dcStream){

    pulses_ = &pulses;
    if(!(pulses.size()>0)){
        log_warn("Empty I3MCPulse vector for DOM %s!",domId_.str().c_str());
        return;
    }
    const double TIMESTEP = 0.5*I3Units::ns;
    const double CLOCK_CYCLE_PHASE = currentClockPhase_;
    double end_sampleTime = pulses.back().time + clockCycle;
    log_trace("Simulating discriminator for DOM %s until time %lf with %d number of pulses",domId_.str().c_str(),end_sampleTime,int(pulses.size()));

    waveform_.SetPulses(pulses);
    double time = pulses[0].time;
    for(size_t pulse_index = 0, n_pulses = pulses.size(); pulse_index < n_pulses; ++pulse_index){
        double pulse_time = pulses[pulse_index].time;
        double nextHitTime = (pulse_index < n_pulses-1) ? pulses[pulse_index+1].time + clockCycle: end_sampleTime;
        double last_amplitude = -DBL_MAX;
        if(pulse_time < time)
            continue;
        else
           time = pulse_time;

        //Start scanning
        while(time < nextHitTime){

            double amplitude = waveform_.WaveFormAmplitude(time,(*discSPETemplatePtr_));
            //Generate discriminator threshold crossing if amplitude above threshold
            if(amplitude > discriminatorThreshold_){
                DiscCross discrx;
                //Fastforwarding to the next clock cycle and determining the triggering
                //clock cycle at the same time!
                time += clockCycle - int64_t(time)%int64_t(clockCycle) - (time - int64_t(time));
                //The Actual trigger gets a time stamp that is one clock Cycle later due to electronics
                //and to get the correct global time of the time stamp one has to also add the CLOCK_CYCLE_PHASE.
                discrx.time = time  + CLOCK_CYCLE_PHASE + clockCycle + discriminatorDelay_;

                discrx.type = dlud::Discriminator;
                discrx.DOM = domId_;

                if(!std::isnan(discrx.time)){
                    dcStream.push_back(discrx);
                }
                break;
            }
            //If the amplitude is falling and we have passed the peak of the current
            //pulse we jump to the next one
            if(amplitude <= last_amplitude && (time - pulse_time) > timeOfPulseTemplatePeak_)
                break;

            last_amplitude = amplitude;
            time += TIMESTEP;
        }
    }
}

bool I3DOM::AddBeaconLaunches(double startTime, double endTime,
    domlauncherutils::DCStream& dcStream){

    double t = startTime;
    //Figure out when the first beacon launch of the frame occurs
    t +=  beaconLaunchPhase_ - int64_t(t)%int64_t(beaconLaunchPhase_) - (t - int64_t(t));
    bool beaconAdded = false;
    //Adding the beacon launches if they are within the end time of the frame (last MCPulse)
    while(t<endTime){
        beaconAdded = true;
        DiscCross discrx;
        discrx.time = t;
        discrx.type = dlud::CPU_REQUESTED;
        discrx.DOM = domId_;
        dcStream.push_back(discrx);
        t +=  1.0/beaconLaunchRate - int64_t(t)%int64_t(1.0/beaconLaunchRate) - (t - int64_t(t));
    }
    return beaconAdded;
}

void I3DOM::AddTrigger(DiscCross &discrx){
    globalTime_ = discrx.time; //Updating the global time

    //Updating the DOM state to the current global time
    UpdateState(globalTime_);
    //Checking if we can make any launches with previous discriminator
    //crossings.
    EvaluateLaunchPossibility(false,false);

    //A CPU forced launch. Technically a CPU forced launch is not
    //triggered by a discriminator crossing therefore the treatment
    //of it, is a little bit special. It is important that it doesn't
    //set the LC signal high.
    if(discrx.type == CPU_REQUESTED){
        //DOM isn't busy we have a full readout launch
        if( ( domStat_.lcMode == I3DOMStatus::LCOff ||
                (discrXings_.empty()) ) && !busy_){
            Launch(discrx);
        }
        //Return before any of the LC logic code is run.
        return;
    }

    //updating the LC signal
    if(std::isnan(lcHighTime_)){
        lcHighTime_ = globalTime_;
    }else{
        //Updating the LC High time if the discrx is outside the LC
        //deadtime window for LC signals (lcWait).
        if(lcHighTime_ + globalTime_ > lcWait){
            lcHighTime_ = globalTime_;
        }
    }


    bool lcNeighbor(false);
    for(uint i = 0; i < domNeighbors_.size(); i++){
        if(domNeighbors_[i]->CheckLCHigh() > globalTime_  - domStat_.lcWindowPre)
            lcNeighbor = true;
    }

    UpdateState(globalTime_);

    //If any of the LC-lines are high and the
    //DOM isn't busy we have a HLC launch
    if( ( domStat_.lcMode == I3DOMStatus::LCOff ||
            (lcNeighbor && discrXings_.empty()) ) && !busy_){
            // If the I3DOM is in LC , not busy and no previous discrxs exist the
            // launch decision is easy.
            discrx.type = HLC;
            Launch(discrx);
    }
    else if(lcNeighbor && !busy_ ){
        // If the DOMobject is in LC and not busy, but older discrxs exists
        // then all older DiscCrosss have to be checked first before a proper launch decision
        // can be made. This is done in CheckLCHigh.
        discrXings_.push_back(discrx);
        EvaluateLaunchPossibility(false,false);
    }
    else if(!busy_){
        // If the I3DOM is not in LC and isn't busy the discrx is kept but
        // no decision is made.
        discrXings_.push_back(discrx);
    }

}

void I3DOM::EvaluateLaunchPossibility(bool dom, bool force){
  //Go through DOMtriggers to see what launch decisions can be made.
  for(uint i = 0; i < discrXings_.size();i++){
    //Update DOM state with respect to the disc. cross time
    UpdateState(discrXings_[i].time);
    //Disc. crossings which occur when the DOM is busy are thrown away.
    if(busy_){
      discrXings_.erase(discrXings_.begin()+i);
      i--;
    }
    else if(globalTime_ - discrXings_[i].time < domStat_.lcWindowPost && dom){
        if(( (ATWDa_alive_ && domStat_.statusATWDa == I3DOMStatus::On) ||
            (ATWDb_alive_ && domStat_.statusATWDb == I3DOMStatus::On)) &&
            !busy_
            ){
            discrXings_[i].type = HLC;
            Launch(discrXings_[i]);
        }
        else
            discrXings_.erase(discrXings_.begin()+i);
        i--;
    }
    else if(globalTime_ - discrXings_[i].time > domStat_.lcWindowPost || force){

      if((ATWDa_alive_ || ATWDb_alive_) && !busy_){

        discrXings_[i].type = SLC;
        Launch(discrXings_[i]);
      }
      discrXings_.erase(discrXings_.begin()+i);
      i--;
    }
    else{
      break;
    }
  }

  //VERY IMPORTANT!! Updates the status of the DOM to
  //the global time to set the LCHigh flag correct before the function returns.
  UpdateState(globalTime_);

}

void I3DOM::UpdateState(double t){

  if(t > ATWDaDeadTo_)
    ATWDa_alive_ = true;
  else
    ATWDa_alive_ = false;

  if(t > ATWDbDeadTo_)
    ATWDb_alive_ = true;
  else
    ATWDb_alive_ = false;

  if(std::isnan(ATWDaDeadTo_) && domStat_.statusATWDa == I3DOMStatus::On)
    ATWDa_alive_ = true;

  if(std::isnan(ATWDbDeadTo_) && domStat_.statusATWDb == I3DOMStatus::On)
    ATWDb_alive_ = true;

  if(t > busyTo_ && (ATWDa_alive_ || ATWDb_alive_) )
    busy_ = false;
  else
    busy_ = true;

  //If the DOM has no triggers all times are still NAN
  if(std::isnan(busyTo_) )
    busy_ = false;

  domTime_ = t;
}

void I3DOM::Launch(const DiscCross& discrx){

    I3DOMLaunch domLaunch;
    short nDigitizations = 0;

    ATWDChip chip;
    //Verify which ATWD is free
    if(ATWDa_alive_ && domStat_.statusATWDa == I3DOMStatus::On){
            chip = ATWDa;
    }
    else if(ATWDb_alive_ && domStat_.statusATWDb == I3DOMStatus::On){
            chip = ATWDb;
    }
    else{//If no ATWD is available no launch is possible.
        return;
    }

    if(discrx.type == HLC || discrx.type == CPU_REQUESTED){

        //Normal HLC launch
        if(discrx.type == HLC){
            //ATWD digitization
            nDigitizations = 1;
            if(ATWDDigitization(discrx, 0, chip, domLaunch.GetRawATWD(0))){//digitizing channel 0
                nDigitizations++;
                if(ATWDDigitization(discrx, 1, chip, domLaunch.GetRawATWD(1))){//digitizing channel 1
                    nDigitizations++;
                    ATWDDigitization(discrx, 2, chip, domLaunch.GetRawATWD(2));//digitizing channel 2
                }
            }
            domLaunch.SetTriggerType(I3DOMLaunch::SPE_DISCRIMINATOR_TRIGGER);
            domLaunch.SetLCBit(true);

        }
        else{//CPU requested launch (known as beacon launch)
            nDigitizations = 3;
            ATWDDigitization(discrx, 0, chip,  domLaunch.GetRawATWD(0));
            ATWDDigitization(discrx, 1, chip,  domLaunch.GetRawATWD(1));
            ATWDDigitization(discrx, 2, chip,  domLaunch.GetRawATWD(2));
            domLaunch.SetTriggerType(I3DOMLaunch::CPU_REQUESTED);
            domLaunch.SetLCBit(false);
        }

        FADCDigitization(discrx, domLaunch.GetRawFADC());

        //Calculating the dead time of the ATWD. The more digitizations the less clear time.
        //There are 4 ATWD channels. (only 3 are simulated)
        double ATWDDeadTime = domTime_ + nDigitizations * ATWDReadoutDigitizeTime +
        ATWDRestartTime + (4-nDigitizations)*ATWDClearTime;

        //Set which ATWD chip read out and set ATWD dead times
        if(chip == ATWDa){
            domLaunch.SetWhichATWD(I3DOMLaunch::ATWDa);
            ATWDaDeadTo_ = ATWDDeadTime;
            ATWDa_alive_  = false;
        }
        else {
            domLaunch.SetWhichATWD(I3DOMLaunch::ATWDb);
            ATWDbDeadTo_ = ATWDDeadTime;
            ATWDb_alive_  = false;
        }

        busyTo_ = discrx.time + HLCReadoutTime;

    }
    else if(discrx.type == SLC){

        //Setting the ATWD dead time. Flushing 4-nDigitizations channels (IceTop SLC launches
        //reads out ATWD) and restarting after cable corrected LC time window and decision time.
        double ATWDDeadTime = domTime_ + cableCorrection_ + 5*clockCycle +
        nDigitizations * ATWDReadoutDigitizeTime + (4-nDigitizations)*ATWDClearTime + ATWDRestartTime;

        //Which ATWD chip read out and set ATWD dead timess
        if(chip == ATWDa){
            domLaunch.SetWhichATWD(I3DOMLaunch::ATWDa);
            ATWDaDeadTo_ = ATWDDeadTime;
        }
        else {
            domLaunch.SetWhichATWD(I3DOMLaunch::ATWDb);
            ATWDbDeadTo_ = ATWDDeadTime;
        }
        //The DOM is busy during the LC time window plus cable
        //correction time plus 5 clock cycles of LC decision time plus
        //2 clock cycles.
        busyTo_ = discrx.time + cableCorrection_ + (5 + 2)*clockCycle;

        domLaunch.SetTriggerType(I3DOMLaunch::SPE_DISCRIMINATOR_TRIGGER);
        domLaunch.SetLCBit(false);
    }
    else{
        log_fatal("A launch occured without a launch decision!!! I will cowardly terminate because "
                "something went terribly wrong!");
    }

    MakeCoarseChargeStamp(discrx, chip, domLaunch);
    busy_ = true;
    //Setting the launch time plus adding a rapCal time shift to account for the
    //uncertainty of RapCal time calibration
    domLaunch.SetStartTime(discrx.time + rapCalShift_);
    domLaunches_.push_back(domLaunch);
}

bool I3DOM::ATWDDigitization(const DiscCross& discrx,
                             int channel,
                             int chip,
                             std::vector<int>& digitizedReadOut){

    if(std::isnan(speMean_)){
        log_warn("SPEMean is NAN for DOM %s:",domId_.str().c_str());
        return false;
    }
    if(std::isnan(domCal_.atwdGains[channel])){
        log_warn("ATWD gain is NAN for DOM %s:",domId_.str().c_str());
        return false;
    }

    double transitDeltaTime = transitTime_ + domCal_.atwdDeltaT[chip];
    double norm =  speMean_ * domCal_.atwdGains[channel] * domCal_.frontEndImpedance;
    //analog in the sense that each sample is stored with double floating point precision
    double analogReadOut[nATWDBins];
    bool overThreshold = false;
    double binLength = 1.0 / atwdSamplingRate_[chip];

    InterpolatedSPETemplate &atwdSPETemplate = *atwdSPETemplatePtr_[channel];

    //Making sure the array is reset to zero
    for(uint i = 0; i < nATWDBins; i++) analogReadOut[i] = 0;

    //filling raw waveform
    for(uint i = 0; i < nATWDBins; i++){
        double binTime = discrx.time + (i+1) * binLength;
        double t = binTime - transitDeltaTime;
        analogReadOut[i] +=  norm * waveform_.WaveFormAmplitude(t,atwdSPETemplate);
    }
    digitizedReadOut.resize(nATWDBins);

    //digitizing waveform
    for( uint64_t i = 0; i < nATWDBins; i++){
        analogReadOut[i] /= domCal_.atwdBinCalib[chip][channel][i];//convert to number of counts
        // Adding electronic noise. The parameters are taken from the old DOMSimulator.
        analogReadOut[i] += rng_->Gaus(ATWDNoiseMean, ATWDNoiseVariance);
        //Adding baseline for each bin.
        analogReadOut[i] += domCal_.atwdBeaconBaseline[chip][channel];
        analogReadOut[i] = int(analogReadOut[i]);//floor the number of counts
        //If the waveform is above threshold the function returns true to initiate another digitization.
        if(analogReadOut[i] > ATWDThreshold) overThreshold = true;
        //Floor the readout to the DynamicRange of the Digitizer.
        if(analogReadOut[i] > digitizerDynamicRange) analogReadOut[i] = digitizerDynamicRange;
        if(analogReadOut[i] < 0) analogReadOut[i] = 0;//there are no negative counts.
        digitizedReadOut[i] = int(analogReadOut[i]);
    }
    //If over threshold it returns true so that the digitization of the next channel initializes.
    return overThreshold;

}

void I3DOM::FADCDigitization(const DiscCross& discrx,
                             std::vector<int> &digitizedReadOut){

    double gain = domCal_.fadcGain;
    double transitDeltaTime = transitTime_ - domCal_.fadcDeltaT;
    //analog in the sense that each sample is stored with double floating point precision
    double analogReadOut[nFADCBins];
    double binLength = clockCycle;
    double norm = speMean_ * domCal_.frontEndImpedance;

    for(uint i = 0; i < nFADCBins; i++){
        analogReadOut[i] = 0;
    }//making sure the waveform is reset to zero.


    for(uint i = 0; i < nFADCBins; i++){
        double binTime = discrx.time + (i+1)*binLength;
        double t = binTime - transitDeltaTime;
        analogReadOut[i] += norm * waveform_.WaveFormAmplitude(t,*fadcSPETemplatePtr_);
    }
    digitizedReadOut.resize(nFADCBins);
    //digitizing waveform
    for( uint i = 0; i < nFADCBins; i++){
        analogReadOut[i] /= gain;// convert from GV to counts
        analogReadOut[i] += domCal_.fadcBeaconBaseline;// add baseline (in counts)
        // Adding electronic noise. The parameters are taken from the old DOMSimulator.
        analogReadOut[i] += rng_->Gaus(FADCNoiseMean, FADCNoiseVariance);
        analogReadOut[i] = int(analogReadOut[i]);//floor the number of counts
        if(analogReadOut[i] > digitizerDynamicRange) analogReadOut[i] = digitizerDynamicRange;//2^10-1
        if(analogReadOut[i] < 0) analogReadOut[i] = 0;
        digitizedReadOut[i] = int(analogReadOut[i]);
    }

}
