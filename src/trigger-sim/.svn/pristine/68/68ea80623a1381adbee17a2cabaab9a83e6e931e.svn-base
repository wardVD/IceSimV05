 /**
 * class: I3GlobalTriggerSim
 *
 * Version $Id: $
 *
 * (c) 2009 IceCube Collaboration
 * @file I3GlobalTriggerSim.cxx
 * @date $Date: $
 * @author olivas
 **/
#include <boost/foreach.hpp>

#include <trigger-sim/modules/I3GlobalTriggerSim.h>
#include <trigger-sim/algorithms/GlobalTriggerSim.h>
#include <icetray/I3TrayHeaders.h>
#include <icetray/I3Module.h>
#include <icetray/I3Units.h>
#include <dataclasses/physics/I3EventHeader.h>
#include <dataclasses/calibration/I3Calibration.h>
#include <dataclasses/status/I3DetectorStatus.h>
#include <dataclasses/status/I3TriggerStatus.h>
#include <dataclasses/I3Time.h>
#include <dataclasses/I3Double.h>
#include <icetray/I3Bool.h>
#include <dataclasses/I3TreeUtils.h>
#include <dataclasses/physics/I3Trigger.h>
#include <dataclasses/physics/I3TriggerHierarchy.h>
#include <dataclasses/physics/I3DOMLaunch.h>
#include <phys-services/I3RandomService.h>

using namespace std;

I3_MODULE(I3GlobalTriggerSim);

I3GlobalTriggerSim::I3GlobalTriggerSim(const I3Context& ctx) 
  : I3Module(ctx),
    eventID_(0),
    i3TriggName_("I3Triggers"),
    globalTriggName_("I3TriggerHierarchy"),
    filterMode_(true),
    i3ReadoutWindowBefore_(NAN),
    i3ReadoutWindowAfter_(NAN),
    i3ReadoutWindowOffset_(NAN),
    time_increment_(0.),
    run_id_(-1),
    dt_(0.)
{
 
   AddParameter("I3TriggerName","InIce Trigger name in frame",i3TriggName_);
   AddParameter("GlobalTriggerName","Output global trigger name in frame",globalTriggName_);
   AddParameter("FilterMode","Filter events that do not satisfy the trigger condition",filterMode_);
   AddParameter("I3ReadoutWindowBefore", "Readout window before the trigger",i3ReadoutWindowBefore_); 
   AddParameter("I3ReadoutWindowAfter", "Readout window after the trigger",i3ReadoutWindowAfter_);
   AddParameter("I3ReadoutWindowOffset", "Readout window offset the trigger",i3ReadoutWindowOffset_);

   domLaunchSeriesMapNames_.push_back("InIceRawData");
   domLaunchSeriesMapNames_.push_back("IceTopRawData");
   AddParameter("I3DOMLaunchSeriesMapNames",
		"This holds the DOM launches used to determine event lengths for untriggered events.",
		domLaunchSeriesMapNames_);
   AddParameter("TimeIncrement", "Time increment between frames", time_increment_);
   AddParameter("FromTime","If set I3Times will be uniformly spread from this time, until ToTime.", I3TimePtr());
   AddParameter("ToTime","If set I3Times will be uniformly spread to this time.", I3TimePtr());
   AddParameter("RunID","ID of the run.", run_id_);
 
   AddOutBox("OutBox");
}

void I3GlobalTriggerSim::Configure()
{
   log_debug("Configuring the I3GlobalTriggerSim");

   GetParameter("I3TriggerName", i3TriggName_);
   GetParameter("GlobalTriggerName", globalTriggName_);
   GetParameter("FilterMode",filterMode_);
   GetParameter("I3ReadoutWindowBefore",i3ReadoutWindowBefore_);
   GetParameter("I3ReadoutWindowAfter",i3ReadoutWindowAfter_);
   GetParameter("I3ReadoutWindowOffset",i3ReadoutWindowOffset_);
   GetParameter("I3DOMLaunchSeriesMapNames",domLaunchSeriesMapNames_);		 
   GetParameter("TimeIncrement", time_increment_);
   GetParameter("RunID", run_id_);

   I3TimePtr from_time;
   I3TimePtr to_time;
   GetParameter("FromTime", from_time);
   GetParameter("ToTime", to_time);

   if(from_time && to_time){      
     time_range_ = boost::optional<std::pair<I3Time, I3Time> >(std::pair<I3Time, I3Time>(*from_time, *to_time));
     if(!context_.Get<I3RandomServicePtr>()){
       log_fatal("Specifying a FromTime and ToTime also requires a random service.");
     }
   }

   if(time_increment_ > 0. && time_range_){
     log_fatal("Configuration error.  TimeIncrement and TimeRange are incompatible.  Choose one.");
   }

   if(run_id_ < 0){
     // Downstream analyses require the (RunID, EventID) pair to be unique, since when
     // combining events in some post-processing analysis framework, like ROOT or hdf5,
     // there needs to be a way to tell which entries came from different events.
     // So we require that the user set the RunID accordingly.  It's up to them to 
     // ensure it's different for each run.
     log_error("The RunID must be set by the user and you should change this with each run.");
     log_error("Be nice and don't choose 0 or 1 either.");
     log_fatal("Throwing to prevent downstream foot-shooting.");
   }
}

void I3GlobalTriggerSim::DAQ(I3FramePtr frame)
{
   log_debug("Entering I3GlobalTriggerSim::DAQ()");

   const I3DetectorStatus& detStat = frame->Get<I3DetectorStatus>();

   GlobalTriggerSim gts(detStat);
   I3TriggerReadoutConfig roc;
   roc.readoutTimeMinus = i3ReadoutWindowBefore_;
   roc.readoutTimePlus = i3ReadoutWindowAfter_;
   roc.readoutTimeOffset = i3ReadoutWindowOffset_;
   gts.SetDefaultReadoutConfig(roc);

   I3TriggerHierarchyConstPtr i3Triggers = frame->Get<I3TriggerHierarchyConstPtr>(i3TriggName_);

   std::vector<I3Trigger> triggers;
   I3TriggerHierarchy::iterator i;
   for( i = i3Triggers->begin(); i != i3Triggers->end(); i++)
     triggers.push_back(*i);

   I3TriggerPairVector tTriggers;
   gts.InsertThroughputTriggers( triggers, tTriggers );

   I3TriggerHierarchyPtr gTriggers = gts.Merge(tTriggers); 

   /*
    * Determine the size of the event. For triggered events
    * the start and end times are going to be determined by
    * the trigger times.  For untriggered events it's going to
    * be determined by the span of the launches
    */
   std::vector<double> start_times;
   std::vector<double> stop_times;
   if(gTriggers && gTriggers->size()){
     //we have a trigger
     //Global header times
     for(I3TriggerHierarchy::iterator i = gTriggers->begin() ;
         i != gTriggers->end(); i++){
       if(i->GetTriggerKey().GetSource() == TriggerKey::GLOBAL){
         //it's ok that throughput that are daughters of merged
         //are also consider.  their times will always be less than
         //the merged window and we just want the max and min
         start_times.push_back(i->GetTriggerTime());
         stop_times.push_back(i->GetTriggerTime() + i->GetTriggerLength());
       }
     }
   }else{     
     BOOST_FOREACH(std::string& s,domLaunchSeriesMapNames_){
       I3DOMLaunchSeriesMapConstPtr old_launchmap = frame->Get<I3DOMLaunchSeriesMapConstPtr>(s);
       if(old_launchmap){
         BOOST_FOREACH(I3DOMLaunchSeriesMap::const_reference pair, *old_launchmap){
           BOOST_FOREACH(const I3DOMLaunch& l, pair.second){
             start_times.push_back(l.GetStartTime());
             stop_times.push_back(l.GetStartTime() + 6.4*I3Units::microsecond);
           }
         }
       }else{
         log_debug("There was no DOMLaunch map %s in the frame.", s.c_str());
       }
     }
   }
   
   if(! (start_times.size() || stop_times.size()) ){
     log_debug("No hits in this event.");
     PushIf(false,frame);
     return;
   }
   
   // delete the old I3EventHeader
   // one will exist already when re-triggering
   if(frame->Get<I3EventHeaderConstPtr>()){
     frame->Delete("I3EventHeader");
   }
   
   double earliest = *min_element(start_times.begin(),start_times.end());
   double latest = *max_element(stop_times.begin(),stop_times.end());
   /*
    * Now that earliest and latest are set we can set the start and stop times 
    * of the events accordingly and build an I3EventHeader
    */
   I3EventHeaderPtr header(new I3EventHeader);
   header->SetRunID(run_id_);
     
   // global trigger is relative to whatever the GCD DetectorStatus.start_time
   // this is the fastest changing frames of the GCD, so we use this time
   I3Time start_time;
   I3Time end_time;
   if(time_range_){
     I3RandomServicePtr rng = context_.Get<I3RandomServicePtr>();
     double d_time_range = time_range_->first - time_range_->second;
     double dt = rng->Uniform(d_time_range);
     start_time = time_range_->first + earliest + dt;
   }else{
     I3DetectorStatusConstPtr dstatus = frame->Get<I3DetectorStatusConstPtr>("I3DetectorStatus");
     start_time = dstatus->startTime + dt_ + earliest;
   }
   end_time = start_time + latest;
   
   // We want each frame to be separated by this shift.
   // So 100ns, 200ns, 300ns, etc... for the first, second, and third frame
   dt_ += time_increment_;
   
   header->SetStartTime(start_time);
   header->SetEndTime(end_time); 
   header->SetEventID(eventID_++);
   
   /*
    * Fill the frame
    */
   frame->Put(header);
   frame->Put(globalTriggName_, gTriggers);
   
   // If there are no triggers, return false
   PushIf(!gTriggers->empty() ,frame);
}
