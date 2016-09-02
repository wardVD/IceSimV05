#include <iostream>
#include <math.h>
#include <boost/foreach.hpp>

#include "icetray/I3TrayHeaders.h"
#include "icetray/I3Units.h"
#include "icetray/I3Bool.h"
#include "dataclasses/physics/I3TriggerHierarchy.h"
#include "dataclasses/status/I3TriggerStatus.h"
#include "dataclasses/status/I3DetectorStatus.h"
#include "dataclasses/geometry/I3Geometry.h"
#include "dataclasses/physics/I3DOMLaunch.h"
#include "dataclasses/TriggerKey.h"
#include "dataclasses/I3Constants.h"
#include "dataclasses/I3Double.h"
#include "dataclasses/I3Map.h"
#include "dataclasses/I3Vector.h"
#include "trigger-sim/utilities/DOMSetFunctions.h"
#include "trigger-sim/algorithms/TriggerContainer.h"
#include "trigger-sim/algorithms/SlowMPHit.h"
#include "trigger-sim/modules/SlowMonopoleTrigger.h"
#include "trigger-sim/utilities/DetectorStatusUtils.h"

/*
 * slow monopole trigger
 */
using namespace std;
using namespace I3Units;

using DetectorStatusUtils::tk_ts_pair_t;
using DetectorStatusUtils::_sourceID;
using DetectorStatusUtils::_typeID;
using DetectorStatusUtils::_configID;
using DetectorStatusUtils::GetTriggerStatus;

I3_MODULE(SlowMonopoleTrigger);
const TriggerKey::SourceID SOURCEID(TriggerKey::IN_ICE);
const TriggerKey::TypeID TYPEID(TriggerKey::SLOW_PARTICLE);

SlowMonopoleTrigger::SlowMonopoleTrigger(const I3Context& ctx) :
  I3Module(ctx),
  configIDParam_(INT_MIN),
  triggerName_("I3Triggers"),
  dataReadoutName_("InIceRawData"),
  domSetsName_("DOMSets"),
  save_additional_info_(false),
  // the following parameters are read from the GCD:
  t_proximity_(NAN),     // 2.5 microseconds
  t_min_(NAN),           // 0 microseconds
  t_max_(NAN),           // 500 microseconds
  deltad_(boost::optional<double>()),          // 100 meters  (not used if dc_algo = true)
  alpha_min_(boost::optional<double>()),       // 140 deg     (not used if dc_algo = false)
  dc_algo_(boost::optional<bool>()),           // true since IC2012, false for IC2011
  relv_(0.5),            // 0.5 (all values are per definition smaller than 3.0)
  min_tuples_(-1),       // take all by default. pole settings are: 5 for IC2012, 3 for IC2011
  max_event_length_(NAN) // 5000 microseconds
{
    AddOutBox("OutBox");
    
    AddParameter("DataReadoutName",
		 "This holds the DOM launches",
		 dataReadoutName_);
    
    AddParameter("TriggerName",
		 "Name to give to this triggers' records in the event",
		 triggerName_);
    
    AddParameter("TriggerConfigID",
		 "Config ID of the trigger.",
		 configIDParam_);
    
    AddParameter("DOMSetsName",
		 "Name of the I3MapKeyVectorInt defining the DomSets for each DOM.",
		 domSetsName_);
    
    AddParameter("AdditionalInformation",
     		 "Additional 3tuple information",
     		 save_additional_info_);
}

// Nothing special to clean up, so the default destructor should be fine.
SlowMonopoleTrigger::~SlowMonopoleTrigger(){
}

// transitions
void SlowMonopoleTrigger::Configure(){
    GetParameter("DataReadoutName", dataReadoutName_);
    GetParameter("TriggerName",triggerName_);
    GetParameter("TriggerConfigID",configIDParam_);
    GetParameter("DOMSetsName", domSetsName_);

    if(configIDParam_ != INT_MIN)
      configID_ = configIDParam_;
}

void SlowMonopoleTrigger::Finish(){
}

void SlowMonopoleTrigger::DAQ(I3FramePtr frame){
  
  log_debug("starting new event");

  // Get the DOMSets from the frame
  I3MapKeyVectorIntConstPtr domSets = 
    frame->Get<I3MapKeyVectorIntConstPtr>(domSetsName_);

  // clear global variables before entering new frame
  trigger_container_vector.clear();
  trigger_list.clear();

  // per frame variables
  SlowMPHitVector one_hit_list;
  SlowMPHitVector two_hit_list;
  double muon_time_window = -1;

  SlowMPHitVectorPtr hits(new SlowMPHitVector);
  
  if (!frame->Has(dataReadoutName_)) {
    log_debug("Frame does not contain an I3DOMLaunchSeriesMap named %s", 
	      dataReadoutName_.c_str());
    PushFrame( frame );
    return;
  }


  const I3DOMLaunchSeriesMap& dlsMap = frame->Get<I3DOMLaunchSeriesMap>(dataReadoutName_);
  
  /**
   * When DOMset is not defined (i.e. no entry in the trigger status) the default
   * is to use *ALL* DOMs *including* the DeepCore DOMs.
   */

  BOOST_FOREACH(I3DOMLaunchSeriesMap::const_reference r,dlsMap){
    BOOST_FOREACH(I3DOMLaunchSeries::const_reference launch,r.second){
      /**
       * Only consider DOMs whose HLC bit is set or when the threshold
       * is set to one then use all DOMs.  This is the MinBias case.
       */
      if(launch.GetLCBit()){
	/**
	 * For DeepCore triggers only consider DOMs in DOMset 4
	 */
	if( ( domset_ && DOMSetFunctions::InDOMSet(r.first,domset_.get(),domSets) ) ||
	    !domset_ ) {                                                      
	  // either if the whole detector or a specific domset are specified, 
	  // in both cases copy DomLaunches
	  SlowMPHitPtr hit = SlowMPHitPtr(new SlowMPHit);
	  hit->string = r.first.GetString();
	  hit->pos = r.first.GetOM();
	  hit->time = launch.GetStartTime();
	  hits->push_back(*hit);
	}
      }
    }
  }

  //Check to see if a trigger hierarchy already exists
  I3TriggerHierarchyPtr triggers;
  if(frame->Has(triggerName_)){
    const I3TriggerHierarchy& old_th = frame->Get<I3TriggerHierarchy>(triggerName_);
    triggers = I3TriggerHierarchyPtr(new I3TriggerHierarchy(old_th));
    frame->Delete(triggerName_);
  }else{
    triggers = I3TriggerHierarchyPtr(new I3TriggerHierarchy());
  }
  
  //Get Geometry from the frame
  I3GeometryConstPtr geo = frame->Get<I3GeometryConstPtr>("I3Geometry");
  if( ! geo )
  {
	  log_fatal("Can't get geometry from the frame!");
  }


  ////////////////////////////////////////////////////////////////////////
    ////////////////////////   calling trigger    /////////////////////
  ////////////////////////////////////////////////////////////////////////

  // Sort hits timewise
  sort(hits->begin(), hits->end());

  for(int i = 0; i < int(hits->size()); i++)
  {
	SlowMPHit payload = hits->at(i);
	RunTrigger(&one_hit_list, &two_hit_list, &muon_time_window, payload, geo);
  }

  // Final call of CheckTriggerStatus. This is only necessary in simulation.
  // In the DAQ module the data stream never ends, so the timing condition is fulfilled.
  CheckTriggerStatus(&two_hit_list, geo);
  log_debug("Trigger status checked. trigger_list.size(): %d", int(trigger_list.size()) );

  /// insert triggers into the hierarchy
  for (vector<I3Trigger>::iterator tIter = trigger_list.begin(); 
       tIter != trigger_list.end(); tIter++)
  {
	log_debug("writing triggers to the hierarchy: start: %f, length :%f.", 
		 tIter->GetTriggerTime(), tIter->GetTriggerLength() );
	triggers->insert(triggers->end(), *tIter);
  }

  // Now put everything into the frame
  frame->Put(triggerName_,triggers); 
  PushFrame( frame );

}

void SlowMonopoleTrigger::RunTrigger(SlowMPHitVector *one_hit_list__, 
				     SlowMPHitVector *two_hit_list__, 
				     double *muon_time_window__, 
				     SlowMPHit new_hit, 
				     const I3GeometryConstPtr &geo)
{
  if(one_hit_list__->size() == 0) // size is 0, so just add it to the list
    {
      one_hit_list__->push_back(new_hit);
    }
    else // not zero, so compare payload with current one_hit_list if any hlc pair can be formed
    {
      while( fabs(new_hit.time - one_hit_list__->front().time) > 1000.0)
	// makes no sense to compare HLC hits that are 
	//longer apart than 1000 nanoseconds, so remove first from list
        {
            one_hit_list__->erase(one_hit_list__->begin());

            if(one_hit_list__->size() == 0)
            {
                break;
            }
        }

        for(SlowMPHitVector::iterator one_hit_iter = one_hit_list__->begin(); 
	    one_hit_iter != one_hit_list__->end(); )
        {
        	// HLC Pair Check
            if(HLCPairCheck(*one_hit_iter, new_hit))  
	      // the geometry here was just a test and can be deleted later
            {
	      // the payload is the first hit from the HLC pair
            	SlowMPHit check_payload = *one_hit_iter;  
                if(two_hit_list__->size() == 0)        // the pair list is empty
                {
                    if(*muon_time_window__ == -1)
                    {
                        two_hit_list__->push_back(check_payload);
                    }
                    else
                    {
                        if(check_payload.time - *muon_time_window__ <= t_proximity_)
                        {
                            *muon_time_window__ = check_payload.time;
                        }
                        else
                        {
                            two_hit_list__->push_back(check_payload);
                            *muon_time_window__ = -1;
                        }
                    }
                }
                else // the pair list is not empty
                {
                    if(*muon_time_window__ == -1)
                    {
                        if(check_payload.time - two_hit_list__->back().time <= t_proximity_)
                        {
                            *muon_time_window__ = check_payload.time;
                            two_hit_list__->pop_back();
                        }
                        else
                        {
                            if((check_payload.time - two_hit_list__->back().time < t_max_)
                               && (check_payload.time - two_hit_list__->front().time 
				   < max_event_length_))
                            {
                                two_hit_list__->push_back(check_payload);
                            }
                            else
                            {
			      // checks current two_hit_list for 3-tuples
                                CheckTriggerStatus(two_hit_list__, geo); 
                                two_hit_list__->push_back(check_payload);
                            }
                        }
                    }
                    else
                    {
                        if(check_payload.time - *muon_time_window__ <= t_proximity_)
                        {
                            *muon_time_window__ = check_payload.time;
                        }
                        else
                        {
                            *muon_time_window__ = -1;
                            if((check_payload.time - two_hit_list__->back().time < t_max_)
                               && (check_payload.time - two_hit_list__->front().time < max_event_length_))
                            {
                                two_hit_list__->push_back(check_payload);
                            }
                            else
                            {
                                CheckTriggerStatus(two_hit_list__, geo);
                                two_hit_list__->push_back(check_payload);
                            }
                        }
                    }
                }

		// deletes the hit and increments the iterator
                one_hit_iter = one_hit_list__->erase(one_hit_iter); 	
            }
            else  // no HLC found
            {
	      // just increment the iterator
                one_hit_iter++ ;
            }
        }

	// at the end add the current hitPayload for further comparisons
        one_hit_list__->push_back(new_hit); 

        if(two_hit_list__->size() > 0) // definitely cannot produce a trigger
        {
        	if( (one_hit_list__->front().time - two_hit_list__->back().time) > t_max_)
        	{
                CheckTriggerStatus(two_hit_list__, geo);
        	}
        }
    }
}

bool SlowMonopoleTrigger::HLCPairCheck(SlowMPHit hit1, SlowMPHit hit2)
{
	int string_nr1 = hit1.string;
	int string_nr2 = hit2.string;

    if(string_nr1 == string_nr2)
    {
        int om_nr1 = hit1.pos;
        int om_nr2 = hit2.pos;

        if( abs(om_nr1 - om_nr2) <= 2)
        {
            return true;
        }
    }
    return false;
}

void SlowMonopoleTrigger::CheckTriggerStatus(SlowMPHitVector *two_hit_list__, 
					     const I3GeometryConstPtr &geo)
{
    int list_size = two_hit_list__->size();
    if(list_size >= 3)
    {
        for(SlowMPHitVector::iterator iter_1 = two_hit_list__->begin(); 
	    iter_1 != two_hit_list__->end() -2 ; iter_1++ )
        {
        	for(SlowMPHitVector::iterator iter_2 = iter_1 + 1; 
		    iter_2 != two_hit_list__->end() - 1 ; iter_2++ )
        	{
        		for(SlowMPHitVector::iterator iter_3 = iter_2 + 1 ; 
			    iter_3 != two_hit_list__->end(); iter_3++ )
        		{
        			CheckTriple(*iter_1, *iter_2, *iter_3, geo);
        		}
        	}
        }
    }


    for(TriggerContainerVector::iterator trg_iter = trigger_container_vector.begin(); 
	trg_iter != trigger_container_vector.end(); trg_iter ++)
    {
    	TriggerContainer slowmptrigger = *trg_iter;

        // Form trigger with each slowmptrigger object, if num_tuples is fulfilled
        if(slowmptrigger.GetNTuples() >= min_tuples_)
        {
            log_debug("FOUND TRIGGER: start: %f, length :%f with %d tuples.",
		     slowmptrigger.GetTrigger()->GetTriggerTime(), 
		     slowmptrigger.GetTrigger()->GetTriggerLength(), 
		     slowmptrigger.GetNTuples() );
            trigger_list.push_back(*slowmptrigger.GetTrigger());
        }
    }

    trigger_container_vector.clear();
    two_hit_list__->clear();
}

void SlowMonopoleTrigger::CheckTriple(SlowMPHit hit1, 
				      SlowMPHit hit2,  
				      SlowMPHit hit3, 
				      const I3GeometryConstPtr &geo)
{
  double t_diff1 = hit2.time - hit1.time;
  double t_diff2 = hit3.time - hit2.time;
  if((t_diff1 > t_min_) && (t_diff2 > t_min_) && (t_diff1 < t_max_) && (t_diff2 < t_max_))
    {
      double t_diff3 = hit3.time - hit1.time;
      
      double p_diff1 = getDistance(hit1, hit2, geo);
      double p_diff2 = getDistance(hit2, hit3, geo);
      double p_diff3 = getDistance(hit1, hit3, geo);
      log_debug("    ->step2 - p_diff1: %f, p_diff2: %f, p_diff3: %f", 
		p_diff1, p_diff2, p_diff3);
      
      if ( !( (p_diff1 > 0) && (p_diff2 > 0) && (p_diff3 > 0) ))
        {
	  log_debug("exiting check triple because p_diff1: %f, p_diff2: %f, p_diff3: %f", 
		    p_diff1, p_diff2, p_diff3);
	  return;
        }
      
      double cos_alpha = ( pow(p_diff1,2) + pow(p_diff2,2) - pow(p_diff3,2) ) 
	/ ( 2*p_diff1*p_diff2 );

      // being lazy...
      if(alpha_min_){
	log_debug("alpha_min_: %f deg (=%f), cos_alpha_min_: %f,"
		  " alpha: %f deg (=%f), cos_alpha: %f", 
		  alpha_min_.get(), alpha_min_.get()*I3Units::degree, cos_alpha_min_, 
		  acos(cos_alpha)/I3Units::degree, acos(cos_alpha), cos_alpha);
      }
      if(   ( deltad_ && (p_diff1 + p_diff2 - p_diff3 <= deltad_.get()) )   
	    ||   ( alpha_min_ && (cos_alpha <= cos_alpha_min_) )   )
        {
	  double inv_v1 = t_diff1 / p_diff1;
	  double inv_v2 = t_diff2 / p_diff2;
	  double inv_v3 = t_diff3 / p_diff3;
	  
	  log_debug("dc_algo_ is set to %d cos_alpha is %f ", dc_algo_.get() , cos_alpha);
	  
	  double inv_v_mean = (inv_v1 + inv_v2 + inv_v3)/3.0;
	  
	  log_debug("inv_v1: %f, inv_v2: %f, inv_v3: %f, inv_v_mean: %f", 
		    inv_v1, inv_v2, inv_v3, inv_v_mean);
	  
	  log_debug("        ->step3 - inv_v_mean %f: " , fabs(inv_v2 - inv_v1) / inv_v_mean);
	  if(fabs(inv_v2 - inv_v1) / inv_v_mean <= relv_)
            {
	      // Found Triple
	      log_debug("Found Triple: t1: %f, t2: %f, t3: %f, t_diff1: %f  t_diff2: %f", 
			hit1.time, hit2.time, hit3.time, t_diff1, t_diff2);
	      
	      double triple_start = hit1.time;
	      double triple_end = hit3.time;

	      //  prepare a lot of stuff which is going to be written with the trigger	      
	      if(trigger_container_vector.size() == 0)
                {
		  I3TriggerPtr new_trig = I3TriggerPtr(new I3Trigger);
		  new_trig->SetTriggerFired(true);
		  new_trig->SetTriggerTime(triple_start);
		  new_trig->SetTriggerLength(triple_end-triple_start);
		  new_trig->GetTriggerKey() = triggerKey_;
		  
		  TriggerContainer new_cont(new_trig);
		  trigger_container_vector.push_back(new_cont);
		  
		  log_debug("** Adding first trigger to list **");
                }
	      else
                {
		  double trigger_start_temp = 
		    trigger_container_vector.back().GetTrigger()->GetTriggerTime();

		  double trigger_end_temp = 
		    trigger_start_temp 
		    + trigger_container_vector.back().GetTrigger()->GetTriggerLength();
		  
		  log_debug("Trigger TEMP: %f %f", trigger_start_temp, trigger_end_temp);
		  
		  if((triple_start >= trigger_start_temp) 
		     && (triple_start <= trigger_end_temp) 
		     && (triple_end > trigger_end_temp)) //overlap
                    {
		      trigger_container_vector.back().GetTrigger()->
			SetTriggerLength(triple_end-trigger_start_temp);

		      trigger_container_vector.back().IncreaseNTuples();
		      log_debug("** Found overlap with existing trigger **");
                    }
		  else if((triple_start >= trigger_start_temp) 
			  && (triple_end <= trigger_end_temp)) // contained tuple
                    {
		      trigger_container_vector.back().IncreaseNTuples();
		      log_debug("** Found contained tuple **");

                    }
		  else if(triple_start > trigger_end_temp)
                    {
		      
		      I3TriggerPtr new_trig = I3TriggerPtr(new I3Trigger);
		      new_trig->SetTriggerFired(true);
		      new_trig->SetTriggerTime(triple_start);
		      new_trig->SetTriggerLength(triple_end-triple_start);
		      new_trig->GetTriggerKey() = triggerKey_;
		      
		      
		      TriggerContainer new_cont(new_trig);
		      
		      trigger_container_vector.push_back(new_cont);
		      
		      log_debug("** Found SECOND TRIGGER **");
                    }
                }
            }
        }
    }
}

double SlowMonopoleTrigger::getDistance(SlowMPHit hit1, 
					SlowMPHit hit2, 
					const I3GeometryConstPtr &geo)
{
  I3OMGeoMap::const_iterator geo_iterator_1 = geo->omgeo.find(OMKey(hit1.string, hit1.pos));
  I3OMGeoMap::const_iterator geo_iterator_2 = geo->omgeo.find(OMKey(hit2.string, hit2.pos));
  
  double x1 = geo_iterator_1->second.position.GetX();
  double y1 = geo_iterator_1->second.position.GetY();
  double z1 = geo_iterator_1->second.position.GetZ();
  double x2 = geo_iterator_2->second.position.GetX();
  double y2 = geo_iterator_2->second.position.GetY();
  double z2 = geo_iterator_2->second.position.GetZ();
  
  double diff = sqrt( pow(x2 - x1, 2) + pow(y2 - y1, 2) + pow(z2 - z1, 2) );
  
  return diff;
}

vector<I3ParticlePtr> additional_info(SlowMPHit hit1, SlowMPHit hit2, SlowMPHit hit3)
{
  vector<I3ParticlePtr> tuples;
  
  double t1 = hit1.time;
  double t2 = hit2.time;
  double t3 = hit3.time;
  
  I3Position pos1(hit1.x, hit1.y, hit1.z);
  I3Position pos2(hit2.x, hit2.y, hit2.z);
  I3Position pos3(hit3.x, hit3.y, hit3.z);
  
  double dist1 = (pos1-pos2).Magnitude();
  double dist2 = (pos2-pos3).Magnitude();
  double dist3 = 0. ;
  
  I3Direction dir1((pos2.GetX()-pos1.GetX()),
		   (pos2.GetY()-pos1.GetY()),
		   (pos2.GetZ()-pos1.GetZ()));

  I3Direction dir2((pos3.GetX()-pos2.GetX()),
		   (pos3.GetY()-pos2.GetY()),
		   (pos3.GetZ()-pos2.GetZ()));
  I3Direction dir3;
  
  I3ParticlePtr tupel_info1 = I3ParticlePtr(new I3Particle);
  tupel_info1->SetType(I3Particle::Monopole);
  tupel_info1->SetLocationType(I3Particle::InIce);
  tupel_info1->SetPos(pos1);
  tupel_info1->SetDir(dir1);
  tupel_info1->SetEnergy(10000000);
  tupel_info1->SetSpeed((dist1*I3Units::m/(t2-t1)*I3Units::ns)/I3Constants::c);
  tupel_info1->SetTime(t1);
  tupel_info1->SetLength(dist1);
  tuples.push_back(tupel_info1);
  
  I3ParticlePtr tupel_info2 = I3ParticlePtr(new I3Particle);
  tupel_info2->SetType(I3Particle::Monopole);
  tupel_info2->SetLocationType(I3Particle::InIce);
  tupel_info2->SetPos(pos2);
  tupel_info2->SetDir(dir2);
  tupel_info2->SetEnergy(10000000);
  tupel_info2->SetSpeed((dist2*I3Units::m/(t3-t2)*I3Units::ns)/I3Constants::c);
  tupel_info2->SetTime(t2);
  tupel_info2->SetLength(dist2);
  tuples.push_back(tupel_info2);
  
  
  I3ParticlePtr tupel_info3 = I3ParticlePtr(new I3Particle);
  tupel_info3->SetType(I3Particle::Monopole);
  tupel_info3->SetLocationType(I3Particle::InIce);
  tupel_info3->SetPos(pos3);
  tupel_info3->SetDir(dir3);
  tupel_info3->SetEnergy(10000000);
  tupel_info3->SetSpeed(0);
  tupel_info3->SetTime(t3);
  tupel_info3->SetLength(dist3);
  tuples.push_back(tupel_info3);
  
  return tuples;
}

void SlowMonopoleTrigger::DetectorStatus(I3FramePtr frame){
  // Get DetectorStatus from the frame
  I3DetectorStatusConstPtr detStatus = frame->Get<I3DetectorStatusConstPtr>();
  if(!detStatus) log_fatal("This DetectorStatus frame has no I3DetectorStatus object.");

  boost::optional<tk_ts_pair_t> tkts = GetTriggerStatus
    (detStatus, _sourceID = SOURCEID, _typeID = TYPEID, _configID = configID_ );

  if(!tkts) log_fatal("Failed to configure this module from the DetectorStatus.");

  // this needs to be passed on to the I3Trigger
  triggerKey_ = tkts.get().first;
  
  // now set the parameters
  tkts.get().second.GetTriggerConfigValue("t_proximity", t_proximity_);
  tkts.get().second.GetTriggerConfigValue("t_min", t_min_);
  tkts.get().second.GetTriggerConfigValue("t_max", t_max_);
  tkts.get().second.GetTriggerConfigValue("rel_v", relv_);
  tkts.get().second.GetTriggerConfigValue("min_n_tuples", min_tuples_);
  tkts.get().second.GetTriggerConfigValue("max_event_length", max_event_length_);
  tkts.get().second.GetTriggerConfigValue("alpha_min", alpha_min_);//these are optional
  tkts.get().second.GetTriggerConfigValue("delta_d", deltad_);//these are optional
  tkts.get().second.GetTriggerConfigValue("domSet", domset_);//these are optional
  tkts.get().second.GetTriggerConfigValue("dc_algo", dc_algo_);//these are optional

  if(alpha_min_)
    cos_alpha_min_ = cos(alpha_min_.get()*I3Units::degree);

  // just a cross check
  if(dc_algo_ && dc_algo_.get() == true && !deltad_){
    log_error("Misconfigured I3TriggerStatus.");
    log_fatal("If dc_algo is true then delta_d needs to be defined.");
  }

  if(dc_algo_ && dc_algo_.get() == false && !alpha_min_){
    log_error("Misconfigured I3TriggerStatus.");
    log_fatal("If dc_algo is false then alpha_min needs to be defined.");
  }     

  std::stringstream sstr;
  sstr<<"t_proximity: "<<t_proximity_<<", t_min: "<<t_min_<<", t_max: "<<t_max_
      <<", rel_v: "<<relv_<<", min_tuples: "<<min_tuples_
      <<", max_event_length: "<<max_event_length_;

  sstr<<", delta_d: ";
  if(deltad_) sstr<<deltad_.get();
  else sstr<<"(unset)"; 

  sstr<<", alpha_min ";
  if(alpha_min_) sstr<<alpha_min_.get();
  else sstr<<"(unset)"; 

  sstr<<", dc_algo: ";
  if(dc_algo_) sstr<<dc_algo_.get();
  else sstr<<"(unset)"; 

  sstr<<", domset: ";
  if(domset_) sstr<<domset_.get();
  else sstr<<"(unset)"; 

  log_debug("%s",sstr.str().c_str());
	    
  PushFrame( frame );
}








