 /**
 * class: I3GlobalTriggerSim
 *
 * Version $Id: $
 *
 *
 * (c) 2009 IceCube Collaboration
 * @file I3GlobalTriggerSim.cxx
 * @date $Date: $
 * @author olivas
 **/

#include "trigger-sim/algorithms/GlobalTriggerSim.h"
#include "trigger-sim/utilities/GTSUtils.h"
#include "dataclasses/physics/I3Trigger.h"
#include "dataclasses/status/I3DetectorStatus.h"
#include <dataclasses/I3Time.h>
#include "trigger-sim/utilities/ReadoutWindowUtil.h"
#include <boost/foreach.hpp>
using namespace std;

typedef std::map<I3TriggerStatus::Subdetector, I3TriggerReadoutConfig> roconfigmap_t;


void GlobalTriggerSim::InsertThroughputTriggers(std::vector<I3Trigger>& triggerStream,
						std::vector< std::pair< I3Trigger, I3Trigger> >& triggerPairs){

  sort(triggerStream.begin(), triggerStream.end(), GTSUtils::LessThan);

  BOOST_FOREACH(const I3Trigger& t, triggerStream){

    if(t.GetTriggerKey().GetSource() != TriggerKey::IN_ICE &&
       t.GetTriggerKey().GetSource() != TriggerKey::ICE_TOP ) {
      log_error("Found a trigger from %s",t.GetTriggerKey().GetSourceString());
      log_error("This module only considers IN_ICE and ICE_TOP triggers and skips everything else.");
      continue;
    }

    //first get the readout window for this trigger key
    double etime = roUtil_->GetEarliestReadoutTime(t);
    if(std::isnan(etime)){
      log_debug("Got NAN for earliest readout time.  Going to use provided defaults.");
      if(std::isnan(defaultReadoutConfig_.readoutTimeMinus)){
	stringstream ss;
	ss<<t.GetTriggerKey();
	log_fatal("Can't determine non-NAN readout window for %s.  Data will be garbage.", ss.str().c_str() );
      }
      etime = defaultReadoutConfig_.readoutTimeMinus;
    }
    double ltime = roUtil_->GetLatestReadoutTime(t);
    if(std::isnan(ltime)){
      log_debug("Got NAN for latest readout time.  Going to use provided defaults.");
      if(std::isnan(defaultReadoutConfig_.readoutTimePlus)){
	stringstream ss;
	ss<<t.GetTriggerKey();
	log_fatal("Can't determine non-NAN readout window for %s.  Data will be garbage.", ss.str().c_str() );
      }
      ltime = defaultReadoutConfig_.readoutTimePlus;
    }

    double tlength(ltime - etime);
    double ttime(etime);

    TriggerKey tpKey(TriggerKey::GLOBAL, TriggerKey::THROUGHPUT);
    I3Trigger tpTrigger;
    tpTrigger.GetTriggerKey() = tpKey;
    tpTrigger.SetTriggerFired(true);
    tpTrigger.SetTriggerTime(ttime);
    tpTrigger.SetTriggerLength(tlength);
 
    triggerPairs.push_back( I3TriggerPair( tpTrigger, t ) );
  }
}


I3TriggerHierarchyPtr
GlobalTriggerSim::Merge(const I3TriggerPairVector& tpTriggers){

  /**
   * First we'll start with a sanity check on the input
   */
  if( tpTriggers.size() == 0  ){
    log_debug("invalid input");
    log_debug("passed a null pointer, null iterator, empty hierarchy, or incomplete tree.");
    return I3TriggerHierarchyPtr(new I3TriggerHierarchy() );
  }
  
  /**
   * Each trigger has a throughput trigger as a parent, so if the size of
   * the tree is two, this means there is only one trigger in the event.
   */
  if(tpTriggers.size() == 1){
    // trivial case
    I3TriggerHierarchyPtr th(new I3TriggerHierarchy() );
    I3TriggerHierarchy::iterator tt_iter = th->insert( th->begin(), tpTriggers.front().first);
    th->append_child( tt_iter, tpTriggers.front().second );
    return th;
  }

  I3TriggerHierarchyPtr mergedTriggers(new I3TriggerHierarchy() );

  BOOST_FOREACH( const I3TriggerPair& tpair, tpTriggers ){
    
    I3TriggerHierarchy::iterator overlap_iter = FindOverlap( mergedTriggers, tpair.first );
 
    if( overlap_iter == mergedTriggers->end() ){
      // no overlap in the tree
      I3TriggerHierarchy::iterator tt_iter = mergedTriggers->insert( mergedTriggers->begin() , tpair.first);
      mergedTriggers->append_child( tt_iter, tpair.second );
    }else{
      // found an overlap
      // if it's already a MERGED we only need to add the child and expand the window
      if( overlap_iter->GetTriggerKey().GetType() == TriggerKey::MERGED ){
      	I3TriggerHierarchy::iterator tt_iter = mergedTriggers->append_child(overlap_iter, tpair.first);
	mergedTriggers->append_child( tt_iter, tpair.second );	

	double t1( overlap_iter->GetTriggerTime() + overlap_iter->GetTriggerLength() );
	double t2( tpair.first.GetTriggerTime() + tpair.first.GetTriggerLength() );	
	double latest_time( t1 < t2 ? t2 : t1 );
	
	double st1( overlap_iter->GetTriggerTime() );
	double st2( tpair.first.GetTriggerTime() );
	double earliest_time( st1 < st2 ? st1 : st2 );
	// set the times
	overlap_iter->SetTriggerTime( earliest_time );
	overlap_iter->SetTriggerLength( latest_time - earliest_time );
      }
      if( overlap_iter->GetTriggerKey().GetType() == TriggerKey::THROUGHPUT ){
	// need to create a merged trigger and reparent
	// there are only two TT to worry about
	
	double st1( overlap_iter->GetTriggerTime() );
	double st2( tpair.first.GetTriggerTime() );
	double earliest_time( st1 < st2 ? st1 : st2 );

	double lt1( overlap_iter->GetTriggerTime() + overlap_iter->GetTriggerLength() );
	double lt2( tpair.first.GetTriggerTime() + tpair.first.GetTriggerLength() );
	double latest_time( lt1 < lt2 ? lt2 : lt1 );

	//need to create a merged trigger
	TriggerKey mKey(TriggerKey::GLOBAL, TriggerKey::MERGED);
	I3Trigger mTrigger;
	mTrigger.GetTriggerKey() = mKey;
	mTrigger.SetTriggerFired(true);
	mTrigger.SetTriggerTime( earliest_time );
	mTrigger.SetTriggerLength( latest_time - earliest_time );
	 
	// insert the merged trigger into the tree
	I3TriggerHierarchy::iterator m_iter = 
	  mergedTriggers->insert( mergedTriggers->begin() , mTrigger);

	// append the TT as children ( don't forget the grand children )
	I3TriggerHierarchy::iterator tt_iter = 
	  mergedTriggers->append_child( m_iter, tpair.first );
	mergedTriggers->append_child( tt_iter, tpair.second );

	tt_iter = mergedTriggers->append_child( m_iter, *overlap_iter );
	mergedTriggers->append_child( tt_iter, *(mergedTriggers->child( overlap_iter, 0 ) ) );

	mergedTriggers->erase_children( overlap_iter );
	mergedTriggers->erase( overlap_iter );

      }
      //need a check that it's one or the other
    }    
  }
  return mergedTriggers;
}


I3TriggerHierarchy::iterator GlobalTriggerSim::FindOverlap(I3TriggerHierarchyConstPtr tree, const I3Trigger& t ){
  
  I3TriggerHierarchy::fixed_depth_iterator fd_iter;
  for(fd_iter = tree->begin_fixed(tree->begin(),0);
      tree->is_valid(fd_iter) ;  ++fd_iter ){
    if( t.GetTriggerTime() >= fd_iter->GetTriggerTime() && 
	t.GetTriggerTime() <= fd_iter->GetTriggerTime() + fd_iter->GetTriggerLength() ){
      // there's an overlap
      return I3TriggerHierarchy::iterator(fd_iter);
    } 
  }
  return tree->end();
}
