#include <I3Test.h>

#include <icetray/I3Units.h>
#include <dataclasses/TriggerKey.h>
#include <dataclasses/status/I3TriggerStatus.h>
#include <dataclasses/status/I3DetectorStatus.h>
#include <trigger-sim/algorithms/GlobalTriggerSim.h>
#include "phys-services/I3GSLRandomService.h"

#include <boost/foreach.hpp>

TEST_GROUP(MergeTest);

TEST(test_empty){
  
  I3GSLRandomService rand(99999); 

  //for these test only the source is important
  TriggerKey tkey(TriggerKey::IN_ICE, TriggerKey::UNKNOWN_TYPE );

  //need to setup the trigger status
  //the important thing is the readout map
  I3TriggerReadoutConfig roConfig;
  roConfig.readoutTimeMinus = rand.Uniform(0,10*I3Units::microsecond);
  roConfig.readoutTimePlus = rand.Uniform(0,10*I3Units::microsecond);
  roConfig.readoutTimeOffset = rand.Uniform(0,10*I3Units::microsecond);

  std::map<I3TriggerStatus::Subdetector, I3TriggerReadoutConfig> roConfigMap;    
  roConfigMap[I3TriggerStatus::ALL] = roConfig;

  I3TriggerStatus ts;
  ts.GetReadoutSettings() = roConfigMap;

  std::map<TriggerKey, I3TriggerStatus> tsMap;
  tsMap[tkey] = ts;

  I3DetectorStatus d;
  d.triggerStatus = tsMap;

  GlobalTriggerSim gts(d);

  I3TriggerPairVector empty_trigger;
  ENSURE(gts.Merge(empty_trigger)->size() == 0);

}

TEST(test_merge_trigger_length){
  
  //for these test only the source is important
  TriggerKey tkey(TriggerKey::IN_ICE, TriggerKey::UNKNOWN_TYPE );

  //need to setup the trigger status
  //the important thing is the readout map
  I3TriggerReadoutConfig roConfig;
  roConfig.readoutTimeMinus = 1*I3Units::microsecond;
  roConfig.readoutTimePlus = 1*I3Units::microsecond;
  roConfig.readoutTimeOffset = 0*I3Units::microsecond;

  std::map<I3TriggerStatus::Subdetector, I3TriggerReadoutConfig> roConfigMap;    
  roConfigMap[I3TriggerStatus::ALL] = roConfig;

  I3TriggerStatus ts;
  ts.GetReadoutSettings() = roConfigMap;

  std::map<TriggerKey, I3TriggerStatus> tsMap;
  tsMap[tkey] = ts;

  I3DetectorStatus d;
  d.triggerStatus = tsMap;

  GlobalTriggerSim gts(d);

  I3Trigger tt1;
  tt1.SetTriggerTime(0.*I3Units::nanosecond );
  tt1.SetTriggerLength(1000*I3Units::nanosecond );
  tt1.SetTriggerFired( true );

  TriggerKey ttk1( TriggerKey::IN_ICE, TriggerKey::THROUGHPUT );  
  tt1.GetTriggerKey() = ttk1;

  I3Trigger t1;
  t1.SetTriggerTime(50.*I3Units::nanosecond );
  t1.SetTriggerLength(10*I3Units::nanosecond );
  t1.SetTriggerFired( true );

  TriggerKey tk1( TriggerKey::IN_ICE, TriggerKey::SIMPLE_MULTIPLICITY );  
  t1.GetTriggerKey() = tk1;

  I3Trigger tt2;
  tt2.SetTriggerTime(500.*I3Units::nanosecond);
  tt2.SetTriggerLength(750*I3Units::nanosecond);
  tt2.SetTriggerFired( true );
  TriggerKey ttk2( TriggerKey::IN_ICE, TriggerKey::THROUGHPUT );  
  tt2.GetTriggerKey() = ttk2;

  I3Trigger t2;
  t2.SetTriggerTime(500.*I3Units::nanosecond);
  t2.SetTriggerLength(750*I3Units::nanosecond);
  t2.SetTriggerFired( true );
  TriggerKey tk2( TriggerKey::IN_ICE, TriggerKey::SIMPLE_MULTIPLICITY );  
  t2.GetTriggerKey() = tk2;

  I3TriggerPairVector trigger; 
  trigger.push_back( I3TriggerPair( tt1, t1 ) );
  trigger.push_back( I3TriggerPair( tt2, t2 ) );

  std::cerr<<std::endl;
  std::cerr<<" ****  TTriggers ****"<<std::endl;
  BOOST_FOREACH( I3TriggerPair& tp, trigger){
    std::cerr<<tp.first<<"\n  "<< tp.second<<std::endl;
  }

  I3TriggerHierarchyPtr mergedTriggers = gts.Merge( trigger );

  I3TriggerPtr merged;
  for( I3TriggerHierarchy::iterator i( mergedTriggers->begin() );
       i != mergedTriggers->end() ; ++i ){
    if( i->GetTriggerKey().GetType() == TriggerKey::MERGED )
      merged = I3TriggerPtr( new I3Trigger( *i ) );
  }

  std::cerr<<*mergedTriggers<<std::endl;

  ENSURE( (bool)merged );
  ENSURE_DISTANCE( merged->GetTriggerLength(), 1250 * I3Units::nanosecond, 1 * I3Units::picosecond );
  
}


// The problem identified is that one of the earlier throughput triggers has a later ending than one of the later ones.
// In this case, the type 70 merged trigger was 
// built using 24000 as the ending time (as of 2011 Nov. 1st - Gluesenkamp)

TEST(test_merged_length_long_enough){
  
  I3TriggerReadoutConfig roConfig;
  roConfig.readoutTimeMinus = 10*I3Units::microsecond;
  roConfig.readoutTimePlus = 10*I3Units::microsecond;
  roConfig.readoutTimeOffset = 0*I3Units::microsecond;

  std::map<I3TriggerStatus::Subdetector, I3TriggerReadoutConfig> roConfigMap;    
  roConfigMap[I3TriggerStatus::ALL] = roConfig;

  I3TriggerStatus ts;
  ts.GetReadoutSettings() = roConfigMap;

  TriggerKey inice_key=TriggerKey(TriggerKey::IN_ICE, TriggerKey::SIMPLE_MULTIPLICITY);
  std::map<TriggerKey, I3TriggerStatus> tsMap;
  tsMap[inice_key] = ts;

  I3DetectorStatus d;
  d.triggerStatus = tsMap;

  I3GSLRandomService rand(99999); 

  GlobalTriggerSim gts(d);

  // Put three fake throughput triggers into a hierarchy  
  TriggerKey global_key=TriggerKey(TriggerKey::GLOBAL, TriggerKey::THROUGHPUT);

  I3Trigger t1;
  t1.GetTriggerKey() = global_key;
  t1.SetTriggerFired(true);
  t1.SetTriggerTime(0);
  t1.SetTriggerLength(28000);
  
  I3Trigger t2;
  t2.GetTriggerKey() = global_key;
  t2.SetTriggerFired(true);
  t2.SetTriggerTime(0);
  t2.SetTriggerLength(26000);
  
  I3Trigger t3;
  t3.GetTriggerKey() = global_key;
  t3.SetTriggerFired(true);
  t3.SetTriggerTime(0);
  t3.SetTriggerLength(24000);
  
  I3TriggerPairVector tTriggers;
  tTriggers.push_back( I3TriggerPair( t1, I3Trigger() ) );
  tTriggers.push_back( I3TriggerPair( t2, I3Trigger() ) );
  tTriggers.push_back( I3TriggerPair( t3, I3Trigger() ) );
  
  // Merge the throughput triggers
  
  I3TriggerHierarchyPtr gTriggers = gts.Merge(tTriggers);
  
  // The Merged Trigger should be from 0 to 28000. With the bug, it will be from 0 to 24000
  // because the ending of the last trigger inserted is taken
   
  I3TriggerHierarchy::iterator merged_iterator=gTriggers->begin(); // This iterator points to the type 70 now
  
  ENSURE(merged_iterator->GetTriggerLength() == 28000);
  
}
