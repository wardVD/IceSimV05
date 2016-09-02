#include <icetray/I3Tray.h>
#include <icetray/Utility.h>
#include "diplopia/I3PolyplopiaExp.h"
#include "diplopia/DiplopiaUtils.h"
#include "dataclasses/I3String.h"
#include "dataclasses/physics/I3Particle.h"
#include "simclasses/I3MMCTrack.h"
#include <dataclasses/physics/I3EventHeader.h>
#include "dataclasses/I3Time.h"
#include <phys-services/I3SummaryService.h>
#include <algorithm>


 ////// this module can basically run in two ways
  
  // 1) in thecase where fixed_length_interval_ is -1.0 , we go the standard way by just looking forward for timewindow_ from the last particle starting point (30 microseconds are enough here), to check whether another particle falls in it or not
  
  // 2) delta_t_ will always be reset to zero once a frame is pushed, so we can safely compare it to fixed_length_interval_ (if fixed_length_interval_ is > 0)


using namespace std;

typedef I3Map<I3ParticleID,I3MapStringDoublePtr> WeightMapMap;
I3_POINTER_TYPEDEFS(WeightMapMap);
I3_SERIALIZABLE(WeightMapMap);

I3_MODULE(I3PolyplopiaExp);

// constructor
I3PolyplopiaExp::I3PolyplopiaExp(const I3Context& ctx) : 
  I3Module(ctx),
  mcTreeName_("I3MCTree"),
  mmcTrackName_("MMCTrackList"),
  weightSumName_("WeightSum"),
  mergePEs_(false),
  timeWindow_(30000.0),
  dynamicWindow_(0.0),
  tau_(0.0),
  delta_t_(0.0),
  clock_(0.0),
  corsika_rate_(0.0),
  eventCounter_(0),
  mergedEventCounter_(0),
  fixed_length_interval_(-1.0),
  last_zero_diff_(0.0),
  zero_diff_sum_(0.0),
  weightMap_("CorsikaWeightMap")
{
  AddParameter("TimeWindow","Coincident event time window",timeWindow_);

  AddParameter("MCTreeName",
				  "Name of I3MCTree objects to merge",
				  mcTreeName_);

  AddParameter("MMCTrackListName",
				  "Name of MMCTrackList objects to merge",
				  mmcTrackName_);

  AddParameter("Rate", "Event rate (zero if diplopia should get this from summary service)", 0.0);
  AddParameter("WeightMap", "map of weights to merge", weightMap_);
  AddParameter("MergeMCPEs", "Merge photo-electrons (legacy)", mergePEs_);
  AddParameter("MCPESeriesName",
				  "Name of MCPESeriesName objects to merge (if present)",
				  hitSeriesName_);
  AddParameter("WeightSumName", "Name of weightsum in summary service", weightSumName_);
  AddOutBox("OutBox");
}

// configure module and get parameters from IceTray
void I3PolyplopiaExp::Configure()
{  
  GetParameter("TimeWindow",timeWindow_);
  GetParameter("MCTreeName", mcTreeName_);
  GetParameter("MMCTrackListName", mmcTrackName_);
  
  double sampling_rate;  
  GetParameter("Rate", sampling_rate);
  GetParameter("WeightMap", weightMap_);
  GetParameter("WeightSumName", weightSumName_);
  GetParameter("MergeMCPEs", mergePEs_);
  GetParameter("MCPESeriesName", hitSeriesName_); 
  
  fixed_length_interval_start_=fixed_length_interval_;
  randomService_  = context_.Get<I3RandomServicePtr>("I3RandomService");

  log_info("event rate will be calculated from summary service");
  I3SummaryServicePtr summary = context_.Get<I3SummaryServicePtr>(); 
  if (!summary) {
	 log_fatal("no summary services found");
  }

  names_["MMCTrackName"] = mmcTrackName_;
  names_["MCTreeName"] = mcTreeName_;
  names_["Weights2"] = weightMap_;
  if (mergePEs_) { 
          names_["I3MCPESeriesName"] = hitSeriesName_; 
  }
  
  corsika_rate_ = (*summary)[weightSumName_]/(*summary)["TimeScale"] * I3Units::hertz;

  if (!(sampling_rate > 0.0) ) { 
     sampling_rate = corsika_rate_;
  } 
  
  log_info("corsika rate=%g Hz",corsika_rate_/I3Units::hertz);
  log_info("sampling rate=%g Hz",sampling_rate/I3Units::hertz);
  
  tau_ = 1.0/sampling_rate;
  log_debug("tau = %g nsec.",tau_);
}

void I3PolyplopiaExp::Merge(I3FramePtr frame)
{

    // Iterate over cached frames and merge objects
    unsigned int frameCounter =1;
    
    ///////////////////////
    // Put a dummy corsika weight in the frame, to be used by pure noise event, if it happens
    ///////////////////////
    
    
    I3MapStringDoubleConstPtr weights = frame->Get<I3MapStringDoubleConstPtr>(weightMap_); 
    I3MapStringDoublePtr newWeights =I3MapStringDoublePtr(new I3MapStringDouble(*weights));
    
    double dt = 0.0;
    double offsetTime = 0.0; 
 
    WeightMapMapPtr mapofmaps(new WeightMapMap());
    
    while (coinc_frames_.size() > 0) 
    {
    
      // Compute random time offset between events
      // The foward window should always lie between the end of the 
      // triggger window of the earliest event and the begining of the
      // window from the latest event
      I3FramePtr cframe = coinc_frames_.back();
      dt = deltas_.back();
      offsetTime += dt;

      vector<string>::iterator name_iter;

      DiplopiaUtils::MergeFrames(frame, cframe, names_, offsetTime);

      I3MCTreeConstPtr coincMCTree = cframe->Get<I3MCTreeConstPtr>(mcTreeName_);
      I3Particle primary = I3MCTreeUtils::GetPrimaries(*coincMCTree)[0];
      int minor_id=primary.GetMinorID();
      
      // Put the CorsikaWeight for this primary into the frame, which will be used by
      // CoincidenceAfterprocessing eventually
      
      I3MapStringDoubleConstPtr coincWeights = frame->Get<I3MapStringDoubleConstPtr>(weightMap_); 
      I3MapStringDoublePtr temp_weight=I3MapStringDoublePtr(new I3MapStringDouble(*coincWeights));
      
      char weight_name[256];
      sprintf(weight_name, "%d", minor_id);
      //frame->Put(weight_name,temp_weight);
      (*mapofmaps)[primary.GetID()] = temp_weight;
      log_trace("putting primary %d", minor_id);

      coinc_frames_.pop_back();
      deltas_.pop_back();

      frameCounter++;
      log_trace("end of loop"); 
    }
    frame->Put("CoincidentWeights",mapofmaps);

    // Now reset the time in the header
    if(!frame->Has("I3EventHeader")) {

      if (!mergedEventCounter_) // Only print this once
         log_warn("Did not find any Eventheader!");

    } else {
      I3EventHeaderConstPtr old_header = frame->Get<I3EventHeaderConstPtr>();
      I3Time start_time = old_header->GetStartTime();

      int64_t new_daqTime = start_time.GetUTCDaqTime() + int64_t(clock_*10);
      int new_year = start_time.GetUTCYear();

      I3EventHeaderPtr new_header=I3EventHeaderPtr(new I3EventHeader(*old_header));

      //. Setting start time of the new header
      // Workaround for GlobalTrigger.. GTrigger resets the time in the EventHeader and makes Coincidentafterprocesing useless
      frame->Put("dtime", I3TimePtr(new I3Time(new_year, new_daqTime)));
      new_header->SetStartTime(I3Time(new_year, new_daqTime));

      frame->Delete("I3EventHeader");
      frame->Put("I3EventHeader", new_header);        
    }

    (*newWeights)["Multiplicity"] = double(frameCounter);
    coincidenceHistogram_[frameCounter]++;

    frame->Delete(weightMap_);
    frame->Put(weightMap_,newWeights);

    log_debug("Merged %u events in %g window", frameCounter,delta_t_);

    // clear the frame vector and start over
    coinc_frames_.clear(); 
    mergedEventCounter_++;
    
    clock_ += delta_t_ ;// adjust the clock
    dynamicWindow_ = 0; // reset the time window
}

void I3PolyplopiaExp::DAQ(I3FramePtr frame)
{
  dynamicWindow_=delta_t_+timeWindow_;
  delta_t_ += randomService_->Exp( tau_ );
  eventCounter_++;

  /* 
   * Looking forward for timewindow_ from the last 
   * particle starting point (30 microseconds are enough here), to 
   * check whether another particle falls in it or not
   *
   */
  
  if ( ((delta_t_ > dynamicWindow_) && 
                          fixed_length_interval_<0  ) || 
       ((fixed_length_interval_ > 0) && 
                   (delta_t_ > fixed_length_interval_) ))
  {
    Merge(frame);
    PushFrame(frame,"OutBox");

  } else { // Cache frame into vector for event merging
    coinc_frames_.push_front(frame);
    deltas_.push_front(delta_t_);
    dynamicWindow_=delta_t_+timeWindow_;
  }
}


void 
I3PolyplopiaExp::Finish()
{

  if(coinc_frames_.size() > 0) // it may be that we still got frames in the buffer, so process them and push them
  {
    I3FramePtr frame(new I3Frame(*(coinc_frames_.back()))); // emulate having the frameptr "frame", so the code from above works

    coinc_frames_.pop_back(); // last frame added is same as 'frame'
    deltas_.pop_back();

    Merge(frame);
    PushFrame(frame,"OutBox");
  } 
  
  log_info("Merged %d events into %d", eventCounter_, mergedEventCounter_);  
  log_info("Coincidence multiplicities:");
  typedef I3Map<unsigned int, unsigned int> hist_t;
  BOOST_FOREACH(hist_t::value_type &i, coincidenceHistogram_)
       log_info("%u coincidences: %u",i.first,i.second);

  log_info("... done");  
}

