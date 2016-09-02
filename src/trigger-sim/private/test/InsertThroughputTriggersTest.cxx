#include <I3Test.h>

#include <icetray/I3Units.h>
#include <dataclasses/TriggerKey.h>
#include <dataclasses/status/I3TriggerStatus.h>
#include <dataclasses/status/I3DetectorStatus.h>
#include <trigger-sim/algorithms/GlobalTriggerSim.h>
#include <trigger-sim/utilities/GTSUtils.h>
#include "phys-services/I3GSLRandomService.h"

#include <boost/foreach.hpp>

TEST_GROUP(InsertThroughputTriggerTest);

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
  
  std::vector<I3Trigger> empty_trigger;
  I3TriggerPairVector ttl;
  gts.InsertThroughputTriggers(empty_trigger, ttl);
  ENSURE(ttl.size() == 0 );

}

TEST(test_default){

  I3GSLRandomService rand(99999); 

   //for these test only the source is important
  TriggerKey tkey(TriggerKey::IN_ICE, TriggerKey::UNKNOWN_TYPE );

  //need to setup the trigger status
  //the important thing is the readout map
  I3TriggerReadoutConfig roConfig;
  roConfig.readoutTimeMinus = rand.Uniform(0,10*I3Units::microsecond);
  roConfig.readoutTimePlus = rand.Uniform(0,10*I3Units::microsecond);
  roConfig.readoutTimeOffset = rand.Uniform(0,roConfig.readoutTimeMinus);

  std::map<I3TriggerStatus::Subdetector, I3TriggerReadoutConfig> roConfigMap;    
  roConfigMap[I3TriggerStatus::ALL] = roConfig;

  I3TriggerStatus ts;
  ts.GetReadoutSettings() = roConfigMap;

  std::map<TriggerKey, I3TriggerStatus> tsMap;
  tsMap[tkey] = ts;

  I3DetectorStatus d;
  d.triggerStatus = tsMap;

  GlobalTriggerSim gts(d);

 
 I3Trigger t;
  t.GetTriggerKey() = tkey;
  t.SetTriggerFired(true);
  t.SetTriggerTime(0.);
  t.SetTriggerLength(3.14);

  std::vector<I3Trigger> triggerStream;
  triggerStream.push_back(t);
  
  I3TriggerPairVector tTriggers;
  gts.InsertThroughputTriggers(triggerStream, tTriggers);

  ENSURE(tTriggers.size() == 1);

  //make sure the time window is right
  I3Trigger ttrigger( tTriggers.front().first );
  ENSURE(ttrigger.GetTriggerKey() == TriggerKey(TriggerKey::GLOBAL, TriggerKey::THROUGHPUT) );
  ENSURE(ttrigger.GetTriggerFired());
  ENSURE_DISTANCE(ttrigger.GetTriggerTime(), 
		  t.GetTriggerTime() - roConfig.readoutTimeMinus, 
		  1e-4);

  std::cerr<<"t.GetTriggerLength() = "<<t.GetTriggerLength()<<std::endl;
  std::cerr<<"ttrigger.GetTriggerLength() = "<<ttrigger.GetTriggerLength()<<std::endl;
  std::cerr<<"roConfig.readoutTimeMinus = "<<roConfig.readoutTimeMinus<<std::endl;
  std::cerr<<"roConfig.readoutTimePlus = "<<roConfig.readoutTimePlus<<std::endl;
  std::cerr<<"roConfig.readoutTimeOffset = "<<roConfig.readoutTimeOffset<<std::endl;
  ENSURE_DISTANCE(ttrigger.GetTriggerLength(), 
		  roConfig.readoutTimeMinus + roConfig.readoutTimePlus + roConfig.readoutTimeOffset, 1e-4);

  //make sure the child is the original
  I3Trigger trigger( tTriggers.front().second );
  ENSURE(trigger.GetTriggerKey() == tkey);


}

TEST(test_only_inice_and_icetop){

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

  I3Trigger t;
  t.SetTriggerFired(true);
  t.SetTriggerTime(0.);
  t.SetTriggerLength(3.14);

  std::vector<I3Trigger> triggerStream;

  t.GetTriggerKey() = TriggerKey(TriggerKey::UNKNOWN_SOURCE, TriggerKey::UNKNOWN_TYPE);
  triggerStream.push_back(t);

 I3TriggerPairVector tTriggers;
 gts.InsertThroughputTriggers(triggerStream, tTriggers);

 ENSURE(tTriggers.size() == 0);

}
