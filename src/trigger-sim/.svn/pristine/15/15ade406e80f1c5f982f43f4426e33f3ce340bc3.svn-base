/**
 *  
 * copyright (c) 2012
 * the IceCube Collaboration
 * $Id: CoincidenceAfterProcessing $
 *
 * @date $Date: 2012-02-16
 * @author Gluesenkamp
 *
 */

#include <icetray/I3Module.h>
#include <icetray/I3Context.h>
#include <icetray/I3Frame.h>
#include <icetray/I3Logging.h>
#include <boost/make_shared.hpp>
#include <boost/assign.hpp>
#include <icetray/I3Units.h>
#include <dataclasses/I3Double.h>
#include <icetray/I3Int.h>
#include <dataclasses/I3Map.h>
#include <dataclasses/I3MapOMKeyMask.h>
#include <dataclasses/TriggerKey.h>
#include <dataclasses/physics/I3Trigger.h>
#include <dataclasses/physics/I3DOMLaunch.h>
#include <dataclasses/physics/I3Particle.h>
#include <simclasses/I3MMCTrack.h>
#include <simclasses/I3MCPulse.h>
#include <simclasses/I3ParticleIDMap.hpp>
#include <simclasses/I3MCPE.h>

#include "dataclasses/physics/I3MCTreeUtils.h"
#include "trigger-sim/modules/CoincidenceAfterProcessing.h"
#include "trigger-sim/utilities/ReadoutWindowUtil.h"

#include <algorithm>
#include <vector>
#include <string>
#include <inttypes.h>

/*

CORSIKA MODE (TO be used with PolyplopiaExp only at the moment)

This module is needed to make sure every frame contains exactly one top level>
trigger (type 30 or 70) in the TriggerHierarchy, just as in DAQ. By those, it splits
the event into subevents, each one containg one toplevel trigger, copying everything there that is needed (mctree,
domlaunches, mmctracklist etc.). It fixes
the Eventheader and Drivingtime. It also includes the nhit/nch of mcpulses of primaries that contributed.

Important is the way it is implemented: It first loops over the top level triggers
and creates maps for each one. After that, it loops over all MCHits. In this loop,
the hittime of each hit is compared to the current toplevel window. If it is
a physics hit, it is further compared if a DomLaunch happens closeby. It is
only counted, if a HLC launch has happened in the 6.4 microsecons before the mcpulse,
or any domlaunch 250 ns after the hit.
Since the DOMLaunchMap is alreay pruned, this in the end means, the
nhit/nch for a primary obtained corresponds to the "visible" mcpulses, which seems to be 
the most sensible definition.


SIGNAL/CORSIKA MODE (also applies to 5-component CORSIKA + unweighed BG)

For Signal/Corsika the situation is a little different. The preceeding module is not
PolyplopiaExp, but some signal frame that got corsika muons mixed in within a big
-/+ timeframe with respect to the signal muon.
Here, we only want the toplevel triggerhierarchy that actually contains signal hits
within a readout, because every event in this signal dataset should dactually contain
signal. It then deletes all the non participating particles from the frame and
write as additional inforamtion the nhit/nch in a similar fashion as in the CORSIKA MODE.

*/

I3_POINTER_TYPEDEFS(ParticlePulseIndexMap);

bool sort_pes (I3MCPE i,I3MCPE j) { return (i.time <j.time ); }
bool sort_launches (I3DOMLaunch i, I3DOMLaunch j) { return (i.GetStartTime()<j.GetStartTime()); }
bool sort_hierarchies (I3TriggerHierarchyPtr i, I3TriggerHierarchyPtr j) { 
  return ( i->begin_fixed(i->begin(),0)->GetTriggerTime() < j->begin_fixed(j->begin(), 0)->GetTriggerTime()); 
}
//void CoincidenceAfterProcessing::SConvert(I3FramePtr frame);

typedef std::pair<uint64_t,int> MajorMinorID;
typedef I3Map<MajorMinorID, I3ParticlePtr> I3MapParticleMap;
typedef I3Map<OMKey, std::map<I3ParticleID, std::vector<uint32_t> > > I3ParticleIDMap;

I3MapParticleMap get_primaries_map (I3MCTree tree) {
    I3MapParticleMap map = I3MapParticleMap();

    I3MCTreePtr t_ptr(new I3MCTree(tree));
    I3MCTree::iterator iter;
    I3ParticlePtr latest_primary;
    for (iter=tree.begin(); iter != tree.end(); ++iter) {
        if (t_ptr->depth(iter) == 0) {
            // primary
            latest_primary = I3ParticlePtr(new I3Particle(*iter));
        }
        // add to map under latest primary
        MajorMinorID pair = MajorMinorID(iter->GetMajorID(),iter->GetMinorID());
        map[pair] = latest_primary;
    }
    return map;
}

I3ParticlePtr get_primary (I3MapParticleMap& map, const MajorMinorID& pair) {
    I3MapParticleMap::iterator iter2 = map.find(pair);
    if (iter2 != map.end()) {
        return iter2->second;
    } else {
        //log_fatal("primary not found for particle");
        return I3ParticlePtr();
    }
}

I3ParticlePtr get_primary (I3MapParticleMap& map, const I3ParticleID& id) {
    I3MapParticleMap::iterator iter2 = map.find(MajorMinorID(id.majorID,id.minorID));
    if (iter2 != map.end()) {
        return iter2->second;
    } else {
        return I3ParticlePtr();
    }
}

I3ParticlePtr get_primary (I3MapParticleMap& map, const I3Particle& particle) {
    MajorMinorID pair = MajorMinorID(particle.GetMajorID(),particle.GetMinorID());
    return get_primary(map,pair);
}
I3ParticlePtr get_primary (I3MapParticleMap& map, const I3MCPulse& hit) {
//    MajorMinorID pair = MajorMinorID(hit.GetParticleMajorID(),hit.GetParticleMinorID());
    MajorMinorID pair;
   return get_primary(map,pair);
}

CoincidenceAfterProcessing::CoincidenceAfterProcessing(const I3Context& context) 
    : I3Module(context),
    input_mcpulses_("I3MCPulseSeriesMap"),
    input_mcpes_("I3MCPESeriesMap"),
    particle_id_map_name_("I3MCPulseSeriesMapParticleIDMap"),
	input_domlaunches_("InIceRawData"),
	input_triggerhierarchy_("I3TriggerHierarchy"),
	input_mctree_("I3MCTree"),
	input_mmctracklist_("MMCTrackList"),
	signal_nch_minimum_(1),
	enforce_primaries_(false),
	fluxsum0_(0),
	drivingtime_name_("DrivingTime"),
	event_id_increment_(0)
{	
	AddOutBox("OutBox");
	
	AddParameter("InputMCHits",
		"Name of the MCHitSeries", 
		input_mcpulses_);
	AddParameter("ParticleIDMap",
		"Name of the I3ParticleIDMap", 
		particle_id_map_name_);
	AddParameter("InputDomLaunches",
		"Name for the DomLaunches", 
		input_domlaunches_);
      	AddParameter("InputTriggerHierarchy",
		"Name for the TriggerHierarchy", 
		input_triggerhierarchy_);
	AddParameter("InputMCTree",
		"Name for the MCTree in the frame", 
		input_mctree_);
	AddParameter("InputMMCTrackList",
		"Name for the mmc tracklist", 
		input_mmctracklist_);	
	AddParameter("MinimumSignalNch",
		"Minimum nch required for signal \
		ONLY REQUIRED FOR nugen/corsika", 
		signal_nch_minimum_);
		
	AddParameter("DrivingTimeName","The name of the driving time I3Time object", drivingtime_name_);	
					  
	AddParameter("FluxSum0", "FluxSum in unweighted case.. is this being used?", fluxsum0_);	
		
}
          
void CoincidenceAfterProcessing::Configure()
{
	GetParameter("InputMCHits", input_mcpulses_);
	GetParameter("ParticleIDMap", particle_id_map_name_);
      	GetParameter("InputDomLaunches", input_domlaunches_);
	GetParameter("InputTriggerHierarchy", input_triggerhierarchy_);
      	GetParameter("InputMCTree", input_mctree_);
	GetParameter("InputMMCTrackList", input_mmctracklist_);
	GetParameter("MinimumSignalNch", signal_nch_minimum_);
	GetParameter("DrivingTimeName", drivingtime_name_);
	GetParameter("FluxSum0",fluxsum0_);

}



    
void CoincidenceAfterProcessing::DAQ(I3FramePtr frame)
{
  log_debug("Entering CoincidenceAfterProcessing::DAQ()");
  
  // Check if the frame that is given to this module has an object called 
  // "CorsikaWeightMap". If this is the case, it means Polyplopia.cxx was
  // used, so we are running a dedicated coincident production of a given 
  // "multiplicity" specified in Polyplopia.cxx by nprimaries.
  // 
  // If there is no weightmap with name "CorsikaWeightMap", the frame is 
  // coming from PolyplopiaExp.cxx and we are running in a mode where every
  // Multiplicity frame is saved in one file in the end (standard unweighted 
  // hoerandel production).
  //
  // For Nugen-Corsika this is ignored
  if(frame->Has("CorsikaWeightMap"))
  {
    enforce_primaries_=true;
  }
  else
  {
    enforce_primaries_=false;
  }
  
  detector_status_ = frame->Get<I3DetectorStatusConstPtr>();
  
  ProcessCorsika(frame);  

}

// Only after this function the "EndTime" has a real meaning since only then 
// we know the exact length of the event
void CoincidenceAfterProcessing::
AddDTimeAndHeader(I3FramePtr frame_to_add,
                  I3EventHeaderConstPtr event_header, 
                  double first_trig_time,
                  double trig_length, 
                  double very_first_trig_start,
                  I3Time dtime)
{ 
  
  // Currently workarund.. start_time_old is taken from dtime not from the 
  // old event_header because of the GlobalTrigger
  int64_t daqTime_start = dtime.GetUTCDaqTime() + int64_t((first_trig_time)*10); 
  int64_t daqTime_end = daqTime_start + int64_t(trig_length*10);
  
  I3EventHeaderPtr new_header=I3EventHeaderPtr(new I3EventHeader(*event_header));
  int year = new_header->GetStartTime().GetUTCYear();
  
  new_header->SetStartTime(I3Time(year, daqTime_start));
  new_header->SetEndTime(I3Time(year, daqTime_end));
  new_header->SetEventID(new_header->GetEventID()+event_id_increment_);
  
  frame_to_add->Put(new_header);  
}


void CoincidenceAfterProcessing::ProcessCorsika(I3FramePtr frame)
{
  // first check the Triggerhierarchy - we only need to split if 
  // there is more than one toplevel trigger'  if so, we need to 
  // copy the subtrees of the toplevel triggers into their own 
  // trigger-hierarchies and create multiple frames according to those
 
  // bool sim_frame_pushed = false;
  I3EventHeaderConstPtr old_header = frame->Get<I3EventHeaderConstPtr>();
  if(old_header==NULL)
  {
    log_fatal("No I3EventHeader found...");
  }
  
  I3DOMLaunchSeriesMapConstPtr old_launchmap = 
    frame->Get<I3DOMLaunchSeriesMapConstPtr>(input_domlaunches_);
  if(old_launchmap==NULL)
  {
    log_fatal("No DomLaunches found....");
  }
  
  I3MCPulseSeriesMapConstPtr old_mcpulses=frame->Get<I3MCPulseSeriesMapConstPtr>(input_mcpulses_);
  if(old_mcpulses==NULL)
  {
    log_fatal("No MCPulses found..");
  }

  I3MCPESeriesMapConstPtr old_mcpes =frame->Get<I3MCPESeriesMapConstPtr>(input_mcpes_);
  if(old_mcpes==NULL)
  {
    log_fatal("No MCPEs found..");
  }

  if(!frame->Has(particle_id_map_name_))
  {
    log_fatal("No I3ParticleIDMap %s found..",particle_id_map_name_.c_str());
  }
  I3ParticleIDMap particle_id_map = frame->Get<I3ParticleIDMap>(particle_id_map_name_);
  
  I3MCTreeConstPtr old_tree = frame->Get<I3MCTreeConstPtr>(input_mctree_);
  if(old_tree==NULL)
  {
    log_fatal("No MCTree found.. even if it is empty we need it...");
  }

  // Workaround for GlobalTrigger timeshifting.. as long as GlobalTrigger 
  // shifts the EventHeader time, need to get Timing via a timeobject
  I3TimeConstPtr dtime_const = frame->Get<I3TimeConstPtr>("dtime");  
  // just some high number
  I3Time dtime(2013, 17000000000000LL);
  if(dtime_const!=NULL)
  {
    dtime = *dtime_const;
  }

  // If we retrigger nugen datasets there might be no dtime in it...
  // so no logfatal  
  bool no_mmclist=false;
  I3MMCTrackListConstPtr old_mmclist;
  if(frame->Has(input_mmctracklist_))
  {
    old_mmclist=frame->Get<I3MMCTrackListConstPtr>(input_mmctracklist_);
  }
  else
  {
    no_mmclist=true; // are we dealing with neutral current/ cascades?
  }
  
  // Here we look if have a signal dataset or corsika dataset  
  unsigned long signal_major_particle_id=0;

  bool signal_plus_background = false;
  bool signal_is_nugen   = false;
  I3MapParticleMap primary_map = get_primaries_map(*old_tree); // used to look up primaries for particles and hits
  std::vector<I3Particle> primaries = I3MCTreeUtils::GetPrimaries(*old_tree); // will be used in the very last loop over hierarchies as well
  /*
   * PoissonMerger module should have added a bare I3Particle to identify the
   * primary (signal). No need to scan I3MCTree to guess what the signal is.
   */
  if ( frame->Has("PolyplopiaPrimary") ) {
    signal_plus_background=true;
    I3Particle primary = frame->Get<I3Particle>("PolyplopiaPrimary");
    signal_major_particle_id=primary.GetMajorID();
    if(primary.GetType() == I3Particle::NuMuBar || primary.GetType() == I3Particle::NuMu
      || primary.GetType() == I3Particle::NuE || primary.GetType() == I3Particle::NuEBar
      || primary.GetType() == I3Particle::NuTau || primary.GetType() == I3Particle::NuTauBar)
    {
      signal_is_nugen   = true;  // This is NuGen with CORSIKA BG
    }
  }
  
  // the "global" vectors hold information for each toplevel trigger hierarchy
  std::vector<I3TriggerHierarchyPtr> global_hierarchies;
  std::vector<double> global_starttimes;
  std::vector<double> global_endtimes;
  std::vector<double> global_triggerlengths;
  
  std::vector< I3MCPulseSeriesMapPtr> global_mcpulses;
  std::vector< I3ParticleIDMapPtr> global_particle_id_maps;
  std::vector< I3MCPESeriesMapPtr> global_mcpes;
  
  std::vector<I3MCTreePtr> global_mctrees;
  std::vector<I3MMCTrackListPtr> global_mmc_tracklists;
  std::vector<I3DOMLaunchSeriesMapPtr> global_domlaunches;
  
  // these maps contain the corsika nch/nhit information for each top level hierarchy.. might be switching from int keys to particle keys at some point
  
  std::map<int, std::map<int, int> > global_primaries_nch;
  std::map<int, std::map<int, int> > global_primaries_nhit;
  std::map<int, std::map<int, int> > global_primaries_id;
  std::map<int, std::map<int, bool> > global_primaries_nch_count_boolean;
  
  // these maps do the same for signal... signal is identified via the major id though, already defined at the very beginning
  
  std::map<int, int > global_signal_nch;
  std::map<int, int > global_signal_nhit;
  std::map<int, bool > global_signal_nch_count_boolean;

  //prepare the map of trigger hierarchies and the connected time information
  // first sort the hierarchies (it could be they are unsorted within the big parent hierarchy)
  int hierarchy_index=0;
  I3TriggerHierarchyConstPtr t_hierarchy = frame->Get<I3TriggerHierarchyConstPtr>(input_triggerhierarchy_);
  I3TriggerHierarchy::fixed_depth_iterator fd_iter=t_hierarchy->begin_fixed(t_hierarchy->begin(),0);
  for( ; t_hierarchy->is_valid(fd_iter) ;  ++fd_iter , ++hierarchy_index)
  {
    
    I3TriggerHierarchyPtr subtrig_ptr = I3TriggerHierarchyPtr( new I3TriggerHierarchy());
    subtrig_ptr->insert_subtree(subtrig_ptr->end(), fd_iter);
    global_hierarchies.push_back(subtrig_ptr);
  }

  std::sort( global_hierarchies.begin(), global_hierarchies.end(), sort_hierarchies);
  
  // now start going through the sorted hierarchy vector and start filling it with important items
  for( hierarchy_index=0; hierarchy_index<(int)global_hierarchies.size(); ++hierarchy_index)
  {
    I3TriggerHierarchy::fixed_depth_iterator fd_new = global_hierarchies[hierarchy_index]->
      begin_fixed( global_hierarchies[hierarchy_index]->begin(), 0);
    
    global_starttimes.push_back(fd_new->GetTriggerTime());
    global_endtimes.push_back(fd_new->GetTriggerTime()+fd_new->GetTriggerLength());
    global_triggerlengths.push_back(fd_new->GetTriggerLength());

    global_mcpulses.push_back(I3MCPulseSeriesMapPtr(new I3MCPulseSeriesMap()));
    global_mctrees.push_back(I3MCTreePtr(new I3MCTree()));
    global_mmc_tracklists.push_back(I3MMCTrackListPtr(new I3MMCTrackList()));
    global_domlaunches.push_back(I3DOMLaunchSeriesMapPtr(new I3DOMLaunchSeriesMap()));
    global_mcpes.push_back(I3MCPESeriesMapPtr(new I3MCPESeriesMap()));
    global_particle_id_maps.push_back(I3ParticleIDMapPtr(new I3ParticleIDMap()));
    
    std::map<int, int> init_int;
    std::map<int, bool> init_bool;
    
    global_primaries_nch[hierarchy_index]=init_int;
    global_primaries_nhit[hierarchy_index]=init_int;
    global_primaries_id[hierarchy_index]=init_int;
    global_primaries_nch_count_boolean[hierarchy_index]=init_bool;
    
    global_signal_nch[hierarchy_index]=0;
    global_signal_nhit[hierarchy_index]=0;
    global_signal_nch_count_boolean[hierarchy_index]=false;
    
  }
    

  I3MCPulseSeriesMap::const_iterator om_iterator;
//  I3MCPulseSeries::iterator hit_iterator; 
  I3MCPulseSeries non_const_sorted_series;
    
  I3DOMLaunchSeries::iterator launch_iterator;
  I3DOMLaunchSeriesMapPtr newmap=I3DOMLaunchSeriesMapPtr(new I3DOMLaunchSeriesMap(*old_launchmap));
  I3DOMLaunchSeries non_const_sorted_launches; 
    
  I3MCPESeries::iterator pe_iterator;
  I3MCPESeriesMapPtr new_pe_map=I3MCPESeriesMapPtr(new I3MCPESeriesMap(*old_mcpes));
  I3MCPESeries non_const_sorted_pes; 

  ParticlePulseIndexMapPtr particleMap(new ParticlePulseIndexMap());

  I3MCPulseSeries series_this_subframe;
  I3DOMLaunchSeries launches_this_subframe;
  I3MCPESeries pes_this_subframe;
   
  int num_hierarchies = global_starttimes.size();
  
  double last_launchtime;
  for(om_iterator=old_mcpulses->begin(); om_iterator != old_mcpulses->end(); ++om_iterator)
  {
    // are they timeordered ?
    int curr_hier_index=0;
    series_this_subframe.clear();
    pes_this_subframe.clear();
    
    // pointers for faster access
    std::map<int, int> * current_primaries_nch_pointer = &global_primaries_nch[curr_hier_index];
    std::map<int, int> * current_primaries_nhit_pointer = &global_primaries_nhit[curr_hier_index];
    std::map<int, int> * current_primaries_id_pointer = &global_primaries_id[curr_hier_index];    
    std::map<int, bool> * current_primaries_nch_count_boolean_pointer = &global_primaries_nch_count_boolean[curr_hier_index];
    std::map<I3ParticleID,std::vector<uint32_t> > idMap = particle_id_map[om_iterator->first];
    //unused for pure corsika
    int* current_signal_nch_pointer = &global_signal_nch[curr_hier_index];
    int* current_signal_nhit_pointer = &global_signal_nhit[curr_hier_index];
    bool* current_signal_nch_count_boolean_pointer = &global_signal_nch_count_boolean[curr_hier_index];
        
    // define series for the sorting
    non_const_sorted_series=om_iterator->second;    
    non_const_sorted_launches=(*newmap)[om_iterator->first];
    non_const_sorted_pes=(*new_pe_map)[om_iterator->first];
    
    std::sort(non_const_sorted_pes.begin(), non_const_sorted_pes.end(), sort_pes);
    std::sort(non_const_sorted_launches.begin(), non_const_sorted_launches.end(), sort_launches);
    
    launch_iterator=non_const_sorted_launches.begin();
    
    double prev_to_last_launchtime=-1;
    bool prev_to_last_flag=false;
    
    int cur_launch=0;
    int cur_hit=0;
    int num_launches=non_const_sorted_launches.size();
    int num_mcpulses=non_const_sorted_series.size();
    // int num_mcpes=non_const_sorted_pes.size();
    
    last_launchtime = -100000.0;
    int cur_launch_hierarchy=0;
    if(num_launches>0)
    {
      launches_this_subframe.push_back(*launch_iterator); 
      // the launches are already pruned.. this
      //one starts the first subframe launchseries
      last_launchtime=launch_iterator->GetStartTime();
      
      while(last_launchtime>global_endtimes[cur_launch_hierarchy])
      {
        ++cur_launch_hierarchy;
      }
      
    }
        
    for(pe_iterator=non_const_sorted_pes.begin(); pe_iterator!=non_const_sorted_pes.end(); ++pe_iterator)
    {
          if(
                pe_iterator->time > global_starttimes[cur_launch_hierarchy] 
             && pe_iterator->time < global_endtimes[cur_launch_hierarchy]
          ) {
              pes_this_subframe.push_back(*pe_iterator); 
	  }
    }
    (*global_mcpes[cur_launch_hierarchy])[om_iterator->first]=pes_this_subframe;
    pes_this_subframe.clear(); 
    
    int pulse_index=0; 
    int sub_frame_pulse_index=0; 
    for(pulse_index=0; pulse_index < num_mcpulses; ++pulse_index)
    {
      double hittime=non_const_sorted_series.at(pulse_index).time;
      
      // The || (or) scenario only happens in the new DOMLauncher simulation. MCHits do not contain
      // pre/afterpulses anymore, thus a DOMlaunch can be present without any MCHit directly preceding it
      // An example observed in one event: MCHit time 29217 / SLC Launch 140 ns later
      // Another Launch at ~37000, but no MCHit before this 2nd one -> An afterpulse occured
      // which is only visible in the processed MCHits .. and this only gets missed when we
      // are at the last mcpulse (if such an afterpulse launch happens in the middle of the
      // event, other mcpulses come after it and it will be .. that is why we need the "|| cur_hit==(num_mcpulses-1)"
      //
      // In the old simulation the last domlaunch was always later than the last mcpulse.
      if(last_launchtime != -100000.0 && (hittime>last_launchtime || cur_hit==(num_mcpulses-1))) 
      {
        if(cur_launch<num_launches-1)
        {
          launch_iterator++;
          cur_launch++;
          last_launchtime=launch_iterator->GetStartTime();
          
          if(last_launchtime > global_endtimes[cur_launch_hierarchy])
          {
            (*global_domlaunches[cur_launch_hierarchy])[om_iterator->first]=launches_this_subframe;
            launches_this_subframe.clear();
            
            //updating the current launc h hierarchy
            while(last_launchtime>global_endtimes[cur_launch_hierarchy])
            {
              ++cur_launch_hierarchy;
            }              
            launches_this_subframe.push_back(*launch_iterator);              
          }
          else
          {
            // the new launch is within the current hierarchy... lets push it
            launches_this_subframe.push_back(*launch_iterator);
            
            // if this is already the last mcpulse, there might be more launches behind it that needs pushing
            if(cur_hit==(num_mcpulses-1))
            {
              while(cur_launch < (num_launches-1))
              {
                launch_iterator++;
                cur_launch++;
                last_launchtime=launch_iterator->GetStartTime();
                
                if(last_launchtime > global_endtimes[cur_launch_hierarchy])
                {
                  (*global_domlaunches[cur_launch_hierarchy])[om_iterator->first]=launches_this_subframe;
                  launches_this_subframe.clear();

                  //updating the current launc h hierarchy
                  while(last_launchtime>global_endtimes[cur_launch_hierarchy])
                  {
                    ++cur_launch_hierarchy;
                  } 

                }
                
                launches_this_subframe.push_back(*launch_iterator);
              }
              (*global_domlaunches[cur_launch_hierarchy])[om_iterator->first]=launches_this_subframe;
              launches_this_subframe.clear();
            }
          }            
        }
        else // the current launch is the last one, but still smaller in time than the last hittime
        {
          if(launches_this_subframe.size() > 0)
          {
            (*global_domlaunches[cur_launch_hierarchy])[om_iterator->first]=launches_this_subframe;

            launches_this_subframe.clear();
          }
          
        }
      }     
      if(curr_hier_index==num_hierarchies)
      {
        break;
      }
      // find the first index of the first hit
      if(hittime>global_endtimes[curr_hier_index])
      {
        if(series_this_subframe.size()>0)
        {
          (*global_mcpulses[curr_hier_index])[om_iterator->first]=series_this_subframe;
          series_this_subframe.clear();
        }
        // we can get out already when we are at the last
        if(curr_hier_index==num_hierarchies-1)
        {
          break;
        }
          
        while(hittime>global_endtimes[curr_hier_index])
        {
          ++curr_hier_index;
          // ok now we are already one over the last.. get out
          if(curr_hier_index==num_hierarchies)
          {
            break;
          }      
        }	

        // we go out to the next om
        if(curr_hier_index==num_hierarchies)
        {
          break;
        }
	
        current_primaries_nch_pointer = & global_primaries_nch[curr_hier_index];
        current_primaries_nhit_pointer = & global_primaries_nhit[curr_hier_index];
        current_primaries_id_pointer = & global_primaries_id[curr_hier_index];
	
        current_primaries_nch_count_boolean_pointer = & global_primaries_nch_count_boolean[curr_hier_index];
	
        if(signal_plus_background)
        {
          current_signal_nch_pointer = & global_signal_nch[curr_hier_index];
          current_signal_nhit_pointer = & global_signal_nhit[curr_hier_index];
          current_signal_nch_count_boolean_pointer = & global_signal_nch_count_boolean[curr_hier_index];
          
        }
      }
      if(hittime<global_starttimes[curr_hier_index])
      {
        log_trace("the hittime is smaller than the current start... go to next hit..");
        continue;
      }

	  /*
	   * we add hits to the current subframe, even if
       * not necessarily lie within readout windows.
       * Probably the hits are dropped anyway.
       * In single events, this is also the case this way
	   */
      log_trace("pushing a mcpulse");
      series_this_subframe.push_back(non_const_sorted_series.at(pulse_index));
      sub_frame_pulse_index++;
      if (non_const_sorted_series.at(pulse_index).source != I3MCPulse::PE)
      {
        log_trace("no physics hit..continue");
        continue;
      }

      if(non_const_sorted_launches.size()==0)
      {
      	continue;
      }
      
      bool last_launch_flag=(launch_iterator)->GetLCBit();
      // double launch_hittime_offset=last_launchtime-hittime;
      // we got one launch
      
      if(hittime>last_launchtime) // the current mcpulse time is bigger than the last launchtime (can happen if some mcpulses do not even launch) 
      {
        if(!last_launch_flag) // no lc bit - we can continue to the next mcpulse
        {
          continue;
        }
        
        if((hittime-last_launchtime) > 6400.0) // the mcpulse is not within a 6400 ns window after an hlc launch.. we can go to the next
        {
          continue;
        }
        
      }
      else // the launchtime comes later than the currenth mcpulse time 
      { 
        if(cur_launch>0) // do we have one more domlaunch as a previous one?
        {
          prev_to_last_flag=(launch_iterator-1)->GetLCBit();
          prev_to_last_launchtime=(launch_iterator-1)->GetStartTime();
          // If we are not within the 250ns before a domlaunch and also not in the 6400 ns after the previous domlauch (if it is hlc), skip
          if( (last_launchtime-hittime > 250) && ( (((hittime-prev_to_last_launchtime) > 6400) && prev_to_last_flag) ||
          prev_to_last_flag==false) )
          {
            continue;
          }
        }
        else
        {
          if(last_launchtime-hittime > 250)
          {
            continue;
          }
        }
      }
  
      // ok add the hit to the nhit-nch counters
      std::vector<I3ParticleID> parent_particles = findParents(pulse_index, idMap);
      I3ParticlePtr primary; 

      bool contains_primary = false;
      for (std::vector<I3ParticleID>::iterator pid=parent_particles.begin(); pid != parent_particles.end(); ++pid)
      {

              if(signal_major_particle_id==pid->majorID) {
                  contains_primary=true;
                  primary = get_primary(primary_map, MajorMinorID(pid->majorID,pid->minorID));
                  break;
              }
      }
      if (!primary) {
          log_debug("No primary found");
      }

      // Rebuild particle ID map
      for (std::vector<I3ParticleID>::iterator pid=parent_particles.begin(); pid != parent_particles.end(); ++pid)
      {
              if(pid->majorID > 0) { // Has a parent particle
		(*particleMap)[*pid].push_back(sub_frame_pulse_index);
              }
      }
     

      // Ok we are in signal/background dataset and the major id fits
      if(signal_plus_background && contains_primary)
      {
        if(current_signal_nch_pointer==0)
        {
          (*current_signal_nch_count_boolean_pointer)=true;
          (*current_signal_nch_pointer)=1;
          (*current_signal_nhit_pointer)=1;

        }
        else
        {
          (*current_signal_nhit_pointer)+=1;
          if((*current_signal_nch_count_boolean_pointer)==false)
          {

           (*current_signal_nch_pointer)+=1;
           (*current_signal_nch_count_boolean_pointer)=true;
          }

        }

      } 	
      else // we are in pure corsika.. or its a background corsika particle in signal/background
      {

        for (std::vector<I3ParticleID>::iterator pid=parent_particles.begin(); pid != parent_particles.end(); ++pid)
        {
           I3ParticlePtr pp = get_primary(primary_map, MajorMinorID(pid->majorID,pid->minorID));
           if (!pp) {
              log_debug("No primary found for %" PRIu64 ".%u", pid->majorID,pid->minorID);
              continue;
           }

           if(current_primaries_nch_pointer->count(pp->GetMinorID())==0) {
                 (*current_primaries_nch_pointer)[pp->GetMinorID()]=1,
                 (*current_primaries_id_pointer)[pp->GetMinorID()]=pp->GetMinorID();
                 (*current_primaries_nch_count_boolean_pointer)[pp->GetMinorID()]=true;
                 (*current_primaries_nhit_pointer)[pp->GetMinorID()]=1;
           } else {
                 (*current_primaries_nhit_pointer)[pp->GetMinorID()]+=1;
                 if((*current_primaries_nch_count_boolean_pointer)[pp->GetMinorID()]==false) {

                    (*current_primaries_nch_pointer)[pp->GetMinorID()]+=1;
                    (*current_primaries_nch_count_boolean_pointer)[pp->GetMinorID()]=true;
                 }
           }
	    }
     }
   }
    
   if(series_this_subframe.size()>0)
   {
      (*global_mcpulses[curr_hier_index])[om_iterator->first]=series_this_subframe;
      (*global_particle_id_maps[curr_hier_index])[om_iterator->first]=*particleMap;
      series_this_subframe.clear();
      particleMap->clear();
    }
    if(launches_this_subframe.size()>0)
    {
      	(*global_domlaunches[cur_launch_hierarchy])[om_iterator->first]=launches_this_subframe;
      	launches_this_subframe.clear();
    }
    // reset the nch boolean flat
    
    for(int count=0; count < num_hierarchies; ++count)
    {
      current_primaries_nch_count_boolean_pointer = &global_primaries_nch_count_boolean[count];
   
      std::map<int, bool>::iterator p_iterator;
      for(p_iterator=(*current_primaries_nch_count_boolean_pointer).begin(); p_iterator!=
      (*current_primaries_nch_count_boolean_pointer).end(); ++p_iterator)
      {
      	p_iterator->second=false;
      }
      
      if(signal_plus_background)
      {
      	global_signal_nch_count_boolean[count]=false;
      }
     
    }
  }
  

  I3MCTreePtr non_const_old_tree=I3MCTreePtr(new I3MCTree(*old_tree));
  I3MMCTrackList non_const_mmc_list;
  
  if(!no_mmclist)
  {
    non_const_mmc_list=*old_mmclist;
  }

  for(int i = 0; i < num_hierarchies; ++i)
    {
      if(signal_plus_background) {
          
          if(global_signal_nch[i] < signal_nch_minimum_) {
              continue;
          }
      }
      I3FramePtr subframe(new I3Frame());
      subframe->SetStop(I3Frame::DAQ);
      
      if(i>0) {
          event_id_increment_+=1;
      }
      
      AddDTimeAndHeader(subframe, old_header, global_starttimes[i], global_triggerlengths[i],
                        global_starttimes[0], dtime);
      
      subframe->Put(input_triggerhierarchy_, global_hierarchies[i]);  
      subframe->Put(input_mcpulses_, global_mcpulses[i]);
      subframe->Put(particle_id_map_name_, global_particle_id_maps[i]);
      if ( frame->Has("PolyplopiaPrimary") ) {
          I3Particle polyplopia_primary = frame->Get<I3Particle>("PolyplopiaPrimary");
          subframe->Put("PolyplopiaPrimary", I3ParticlePtr(new I3Particle(polyplopia_primary)));
      }
      
      // now look which primaries are within this one.. and save them
      std::map<int, int>::iterator prim_nch_iter;
      int prim_counter=0;
      char prim_name[256];
      std::string prim_name_str;
      std::vector<I3Particle>::iterator primary_iterator;
      
      for(prim_nch_iter=global_primaries_nch[i].begin(); 
          prim_nch_iter!=global_primaries_nch[i].end(); 
          ++prim_nch_iter, ++prim_counter)
        {
          sprintf(prim_name, "primary_%d_nch", prim_counter);
          prim_name_str.clear();
          prim_name_str.append(prim_name);
          
          subframe->Put(prim_name_str, I3IntPtr(new I3Int(prim_nch_iter->second)));
          
          sprintf(prim_name, "primary_%d_nhit", prim_counter);
          prim_name_str.clear();
          prim_name_str.append(prim_name);
          
          subframe->Put(prim_name_str, I3IntPtr(new I3Int(global_primaries_nhit[i][prim_nch_iter->first])));
          
          sprintf(prim_name, "primary_%d_id", prim_counter);
          prim_name_str.clear();
          prim_name_str.append(prim_name);
          
          subframe->Put(prim_name_str, I3IntPtr(new I3Int(global_primaries_id[i][prim_nch_iter->first])));
          

          // now search the mmc tracklist
          if(!no_mmclist){
            
            I3MMCTrackList::iterator mmciter= non_const_mmc_list.begin();
            I3MMCTrackList newmmclist = I3MMCTrackList();
            
            for( ; mmciter != non_const_mmc_list.end(); )
              {
                I3ParticlePtr primary = get_primary(primary_map, mmciter->GetI3Particle());
                if(prim_nch_iter->first==primary->GetMinorID() 
                   && primary->GetMajorID() != signal_major_particle_id)
                  {
                    // yeah we found the MMCTrack corresponding to a primary that was taken.. lets put it into the new MMCTrackList                    
                    global_mmc_tracklists[i]->push_back(*mmciter);
                  }
                else
                  {
                    newmmclist.push_back(*mmciter);
                  }
                ++mmciter;
              }
            non_const_mmc_list.clear();
            non_const_mmc_list.swap(newmmclist);
          }
      
          // now lets look through the mctree (the nonconst one) and delete elements not needed anymore
          for(primary_iterator=primaries.begin(); primary_iterator!=primaries.end(); ++primary_iterator)
            {
              if( primary_iterator->GetMinorID()==prim_nch_iter->first && primary_iterator->GetMajorID()!=signal_major_particle_id )
                {
                  I3MCTree::iterator tree_iterator(*non_const_old_tree, *primary_iterator);
                  global_mctrees[i]->insert_subtree(global_mctrees[i]->end(), tree_iterator);
                  primaries.erase(primary_iterator);
                  break;
                }
            }
        }
      
      // add signal stats if necessary
      if(signal_plus_background)
        {
          sprintf(prim_name, "signal_nch");
          prim_name_str.clear();
          prim_name_str.append(prim_name);
          
          subframe->Put(prim_name_str, I3IntPtr(new I3Int(global_signal_nch[i])));
          
          sprintf(prim_name, "signal_nhit");
          prim_name_str.clear();
          prim_name_str.append(prim_name);
          
          subframe->Put(prim_name_str, I3IntPtr(new I3Int(global_signal_nhit[i])));
          
          if(!no_mmclist)
            {
              I3MMCTrackList::iterator mmciter= non_const_mmc_list.begin();
              I3MMCTrackList newmmclist = I3MMCTrackList();
              
              // look for the signal muons in the mmc tracks
              
              for( ; mmciter != non_const_mmc_list.end(); )
                {
                  I3ParticlePtr primary = get_primary(primary_map, mmciter->GetI3Particle());
                  if(primary->GetMajorID() == signal_major_particle_id)
                    {
                      // yeah we found the MMCTrack corresponding to a primary that was taken.. lets put it into the new MMCTrackList
                      
                      global_mmc_tracklists[i]->push_back(*mmciter);
                      //mmciter=non_const_mmc_list.erase(mmciter);
                    }
                  else
                    {
                      newmmclist.push_back(*mmciter);
                    }
                  ++mmciter;
                }
              
              non_const_mmc_list.clear();
              non_const_mmc_list.swap(newmmclist);
            }
          
          // look for the signal primary in the tree
          
          for(primary_iterator=primaries.begin(); primary_iterator!=primaries.end(); ++primary_iterator)
            {
              if(primary_iterator->GetMajorID()==signal_major_particle_id)
                {
                  I3MCTree::iterator tree_iterator(*non_const_old_tree,  *primary_iterator);
                  global_mctrees[i]->insert_subtree(global_mctrees[i]->end(), tree_iterator);
                  primaries.erase(primary_iterator);
                  
                  break;
                }
            }
          
          
        }
      
      subframe->Put(input_mctree_, global_mctrees[i]);
      
      // Put the cleaned new MMCInfoList in the frame, if there has been one initially
      if(!no_mmclist)
        {
          subframe->Put(input_mmctracklist_, global_mmc_tracklists[i]);
        }
      
      subframe->Put(input_domlaunches_, global_domlaunches[i]);
      subframe->Put(input_mcpes_, global_mcpes[i]);
      
      // now look for the weights. For that, loop through all the primaries that are registered, multiply their
      // weights and Create one CorsikaWeight (basically moved the calculation from PolyplopiaExp to here)
      
      if(signal_plus_background)
        {
          
          // we do not have sophisticated multiplication of the weight... but the multiplicity will be updated to the correct value (based on primaries nch/nhit)
          I3MapStringDoubleConstPtr temp_weight=frame->Get<I3MapStringDoubleConstPtr>("CorsikaWeightMap");
          if (!temp_weight) {
            log_fatal("No CorsikaWeightMap found in frame");
          }
          
          I3MapStringDoublePtr final_weight=I3MapStringDoublePtr(new I3MapStringDouble(*temp_weight));
          (*final_weight)["Multiplicity"]   = prim_counter+1;
          (*final_weight)["DiplopiaWeight"] = 1.0;
          
          subframe->Put("CorsikaWeightMap", final_weight);
          
          /*
           * For 5-component CORSIKA we don't expect an I3MCWeightDict but for
           * NuGen we should throw a fatal if we don't find it.
           */
          temp_weight=frame->Get<I3MapStringDoubleConstPtr>("I3MCWeightDict");
          if (temp_weight) { 
            final_weight=I3MapStringDoublePtr(new I3MapStringDouble(*temp_weight));
            subframe->Put("I3MCWeightDict", final_weight);
          } else if (signal_is_nugen) {
            log_fatal("No I3MCWeightDict found in frame");
          }
          
        }
      else // normal corsika is different for the weighting!! 
        {
          // we are in Polyplopia-Mode (Not Exp). This means we simulate coincident events with a certain
          // Multiplicity only. We need to make sure that the number of primaries created with Polyplopia all made it
          // into the readout. Otherwhise Multiplicity in the weight is different than the number of primaries and 
          // we will not push this frame. If indeed number of Primaries is equal to Multiplicity, we Put the CorsikaWeight
          // into the subframe and push it.
          double polyplopia_timescale=0.0;
          if(enforce_primaries_==true) 
            {
              
              I3MapStringDoubleConstPtr temp_weight=frame->Get<I3MapStringDoubleConstPtr>("CorsikaWeightMap");
              if (!temp_weight) {
                log_fatal("No CorsikaWeightMap found in frame");
              }
              int multiplicity=temp_weight->at("Multiplicity");
              
              if(multiplicity!=-1)
                {
                  if(multiplicity!=prim_counter)
                    {
                      continue;
                    }
                }
              else
                {
                  
                  if (!frame->Has("minimum_nprimaries")) {
                    log_fatal("minimum_nprimaries not found in frame");
                  }
                  int minimum_multiplicity=frame->Get<I3Int>("minimum_nprimaries").value;
                  if(prim_counter<minimum_multiplicity)
                    {
                      continue;
                    }
                }
              
              polyplopia_timescale=temp_weight->at("TimeScale");
            }
          
          double weight_multiplicity=1.0; // this holds the multiplicity of the corsika weight
          int temp_id;
          
          if(prim_counter>0) // ok , we got multiplicity 1 or higher
            {
              sprintf(prim_name, "primary_%d_id", 0);
              prim_name_str.clear();
              prim_name_str.append(prim_name);
              temp_id=subframe->Get<I3Int>(prim_name_str).value;
              
              sprintf(prim_name, "%d", temp_id);
              prim_name_str.clear();
              prim_name_str.append(prim_name);
              
              I3MapStringDoubleConstPtr temp_weight=frame->Get<I3MapStringDoubleConstPtr>(prim_name_str);
              
              I3MapStringDoublePtr final_weight=I3MapStringDoublePtr(new I3MapStringDouble(*temp_weight));
              
              double weightnorm = 1.0;
              double fluxsum   =  temp_weight->at("FluxSum");
              if ( fluxsum0_ > 0 ) 
                {
                  weightnorm =  fluxsum/fluxsum0_;
                }
              
              (*final_weight)["FluxSum0"] = fluxsum0_;
              (*final_weight)["DiplopiaWeight"]=1.0; // not really used for PolyplopiaExp.. just for completeness
              
              for(int k=0; k<prim_counter; ++k)
                {
                  sprintf(prim_name, "primary_%d_id", k);
                  prim_name_str.clear();
                  prim_name_str.append(prim_name);
                  temp_id=subframe->Get<I3Int>(prim_name_str).value;
                  
                  sprintf(prim_name, "%d", temp_id);
                  prim_name_str.clear();
                  prim_name_str.append(prim_name);
                  
                  I3MapStringDoubleConstPtr temp_weight=frame->Get<I3MapStringDoubleConstPtr>(prim_name_str);
                  
                  weight_multiplicity*=temp_weight->at("Weight")*weightnorm; // is this correct? In PolyplopiaExp, the
                  // first weight is devided by weightnorm,
                  // then for all frames it is multiplied by
                  // weightnorm    ..... confusion!?
                }
              
              
              (*final_weight)["Weight"]=weight_multiplicity;
              (*final_weight)["Multiplicity"]=prim_counter;
              
              if(enforce_primaries_)
                {
                  (*final_weight)["TimeScale"]=polyplopia_timescale;
                }
              
              subframe->Put("CorsikaWeightMap", final_weight);
              
            }
          else // we got multiplicity 0 - a pure noise event
            {
              const I3MapStringDouble& temp_weight=frame->Get<I3MapStringDouble>("noise_weight");
              
              I3MapStringDoublePtr final_weight=I3MapStringDoublePtr(new I3MapStringDouble(temp_weight));
              
              // also need to multiply weightnorm to the weight?? not sure
              
              (*final_weight)["Multiplicity"]=0;
              (*final_weight)["DiplopiaWeight"]=1.0; // not really used for PolyplopiaExp.. just for completeness
              subframe->Put("CorsikaWeightMap", final_weight);
            }
          
        }      
      copy_additional_frame_objects(frame,subframe);
      PushFrame(subframe);
    }
}

void CoincidenceAfterProcessing::copy_additional_frame_objects(I3FramePtr frame, I3FramePtr subframe) {
	for (I3Frame::typename_iterator iter = frame->typename_begin();
		    iter != frame->typename_end(); iter++) 
	{
	    if (frame->GetStop(iter->first) == I3Frame::DAQ && !subframe->Has(iter->first))
	        subframe->Put(iter->first, frame->Get<I3FrameObjectConstPtr>(iter->first));
	}
}

I3_MODULE(CoincidenceAfterProcessing);

