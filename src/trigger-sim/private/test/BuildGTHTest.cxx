#include <I3Test.h>

#include <icetray/I3Units.h>
#include <dataclasses/TriggerKey.h>
#include <dataclasses/status/I3TriggerStatus.h>
#include <trigger-sim/algorithms/GlobalTriggerSim.h>
#include <trigger-sim/utilities/GTSUtils.h>
#include "phys-services/I3GSLRandomService.h"

#include <boost/foreach.hpp>

TEST_GROUP(BuildGTHTest);

namespace BuildGTHTestUtils{
  std::vector<I3Trigger> 
  MakeTriggers(int nOverlaps, TriggerKey tkey,  int clusterSize = 2){
    
    I3GSLRandomService rand(99999); 
    
    const double TRIGGER_SPACING(100 * I3Units::microsecond);
    
    std::vector<I3Trigger> triggers;
    
    for(int i(0); i < nOverlaps; i++){
      
      double ttime(i * TRIGGER_SPACING);
      double tlength(rand.Uniform(10*I3Units::nanosecond,1e-2 * TRIGGER_SPACING));
      
      for(int j(0); j < clusterSize; j++){    
	
	double dt = rand.Uniform(0,0.1 * tlength);
	
	I3Trigger t;
	t.GetTriggerKey() = tkey;
	t.SetTriggerFired(true);
	t.SetTriggerTime(ttime + dt);
	t.SetTriggerLength(tlength);
	
	triggers.push_back(t);
	
      }
      
    }
    
    return triggers;
    
  }

  I3DetectorStatus
  MakeDetectorStatus(void){ 
    TriggerKey iiSMTKey(TriggerKey::IN_ICE, TriggerKey::SIMPLE_MULTIPLICITY);
    TriggerKey itSMTKey(TriggerKey::ICE_TOP, TriggerKey::SIMPLE_MULTIPLICITY);

    //construct a trigger status
    //the only thing we care about is the readout config map

    I3TriggerReadoutConfig iiSMT_ROConfig_for_IceTop;
    iiSMT_ROConfig_for_IceTop.readoutTimeMinus = 1*I3Units::microsecond;
    iiSMT_ROConfig_for_IceTop.readoutTimePlus = 1*I3Units::microsecond;
    iiSMT_ROConfig_for_IceTop.readoutTimeOffset = 0.;
    
    I3TriggerReadoutConfig iiSMT_ROConfig_for_InIce;
    iiSMT_ROConfig_for_InIce.readoutTimeMinus = 1*I3Units::microsecond;
    iiSMT_ROConfig_for_InIce.readoutTimePlus = 1*I3Units::microsecond;
    iiSMT_ROConfig_for_InIce.readoutTimeOffset = 0.;
    
    I3TriggerReadoutConfig itSMT_ROConfig_for_IceTop;
    itSMT_ROConfig_for_IceTop.readoutTimeMinus = 1*I3Units::microsecond;
    itSMT_ROConfig_for_IceTop.readoutTimePlus = 1*I3Units::microsecond;
    itSMT_ROConfig_for_IceTop.readoutTimeOffset = 0.;
    
    I3TriggerReadoutConfig itSMT_ROConfig_for_InIce;
    itSMT_ROConfig_for_InIce.readoutTimeMinus = 1*I3Units::microsecond;
    itSMT_ROConfig_for_InIce.readoutTimePlus = 1*I3Units::microsecond;
    itSMT_ROConfig_for_InIce.readoutTimeOffset = 0.;
    
    std::map<I3TriggerStatus::Subdetector, I3TriggerReadoutConfig> iiSMTROConfigMap;
    iiSMTROConfigMap[I3TriggerStatus::INICE] = iiSMT_ROConfig_for_InIce;
    iiSMTROConfigMap[I3TriggerStatus::ICETOP] = iiSMT_ROConfig_for_IceTop;
    
    std::map<I3TriggerStatus::Subdetector, I3TriggerReadoutConfig> itSMTROConfigMap;
    itSMTROConfigMap[I3TriggerStatus::INICE] = itSMT_ROConfig_for_InIce;
    itSMTROConfigMap[I3TriggerStatus::ICETOP] = itSMT_ROConfig_for_IceTop;
    
    //need to construct a test trigger status map
    std::map<TriggerKey, I3TriggerStatus> tStatusMap;
    
    I3TriggerStatus iiStatus;
    iiStatus.GetReadoutSettings() = iiSMTROConfigMap;
    tStatusMap[iiSMTKey] = iiStatus;
    
    I3TriggerStatus itStatus;
    itStatus.GetReadoutSettings() = itSMTROConfigMap;
    tStatusMap[itSMTKey] = itStatus;

    I3DetectorStatus d;
    d.triggerStatus = tStatusMap;
    
    return d;
  }

}

TEST(test_single_trigger){

  I3GSLRandomService rand(99999); 

  I3DetectorStatus detStat = BuildGTHTestUtils::MakeDetectorStatus();

  GlobalTriggerSim gts(detStat);

  //with clusterSize = 1 there will no overlaps, so no merging
  TriggerKey iiKey(TriggerKey::IN_ICE, TriggerKey::SIMPLE_MULTIPLICITY);
  std::vector<I3Trigger> test_triggers = BuildGTHTestUtils::MakeTriggers(1,iiKey,1);

  ENSURE(test_triggers.size() == 1);

  I3TriggerPairVector tTriggers;
  gts.InsertThroughputTriggers(test_triggers, tTriggers);  

  ENSURE(tTriggers.size() == 1 );

  I3TriggerHierarchyPtr gTriggers = 
    gts.Merge(tTriggers);

  ENSURE(gTriggers->size() == 2 );

  I3TriggerHierarchy::iterator p_iter = gTriggers->begin();

  ENSURE(gTriggers->number_of_children(p_iter) == 1);

  //make sure the time window is right
  ENSURE(p_iter->GetTriggerKey() == TriggerKey(TriggerKey::GLOBAL, TriggerKey::THROUGHPUT) );
  ENSURE(p_iter->GetTriggerFired());
  
  I3Trigger t = test_triggers.front();
  ReadoutWindowUtil roUtil(detStat);

  std::pair<double,double> p (roUtil.GetInIceReadoutWindow(t) );

  ENSURE_DISTANCE(p_iter->GetTriggerTime(), p.first , 1e-4);
  ENSURE_DISTANCE(p_iter->GetTriggerLength(), p.second - p.first, 1e-4);

  //make sure the child is the original
  I3TriggerHierarchy::sibling_iterator c_iter( p_iter );
  c_iter = gTriggers->begin(p_iter);

  std::cerr<<std::endl;
  std::cerr<<"c_iter = "<<c_iter->GetTriggerKey().GetSourceString()<<" "<<c_iter->GetTriggerKey().GetTypeString()<<std::endl;
  std::cerr<<"c_iter->GetTriggerFired() = "<<c_iter->GetTriggerFired()<<std::endl;
  std::cerr<<"c_iter->GetTriggerTime() = "<<c_iter->GetTriggerTime()<<std::endl;
  std::cerr<<"c_iter->GetTriggerLength() = "<<c_iter->GetTriggerLength()<<std::endl;

  ENSURE(c_iter->GetTriggerKey() == iiKey);

 
}

TEST(test_multiple_triggers){

  I3GSLRandomService rand(99999); 

  I3DetectorStatus detStat = BuildGTHTestUtils::MakeDetectorStatus();    

  TriggerKey iiKey(TriggerKey::IN_ICE, TriggerKey::SIMPLE_MULTIPLICITY);

  const int NTRIGGERS(100);
  const int CLUSTERSIZE(1);
  std::vector<I3Trigger> test_triggers = BuildGTHTestUtils::MakeTriggers(NTRIGGERS,iiKey,CLUSTERSIZE);

  ENSURE(static_cast<int>(test_triggers.size()) == NTRIGGERS);

  GlobalTriggerSim gts(detStat);

  I3TriggerPairVector tTriggers;
  gts.InsertThroughputTriggers(test_triggers, tTriggers);  

  std::cerr<<"tTriggers.size() = "<<tTriggers.size()<<std::endl;

  ENSURE(static_cast<int>(tTriggers.size()) == NTRIGGERS);

  I3TriggerHierarchyPtr gTriggers = 
    gts.Merge(tTriggers);

  std::cerr<<"gTriggers->size() = "<<gTriggers->size()<<std::endl;

  ENSURE(gTriggers->size() == 2 * NTRIGGERS);

  //there should be only one global throughput with this
  //trigger as a daughter
  for( I3TriggerHierarchy::fixed_depth_iterator gt_iter = gTriggers->begin_fixed(gTriggers->begin(),0) ;
       gTriggers->is_valid(gt_iter); gt_iter++){
    ENSURE(gt_iter->GetTriggerKey().GetSource() == TriggerKey::GLOBAL);
    ENSURE(gt_iter->GetTriggerKey().GetType() == TriggerKey::THROUGHPUT);
    
    //get the daughter
    ENSURE(gTriggers->number_of_children(gt_iter) == 1);
    
    I3TriggerHierarchy::sibling_iterator c_iter(gt_iter);
    c_iter = gTriggers->begin(gt_iter);
    
    ENSURE(c_iter->GetTriggerKey() == iiKey);

  }
 
}

TEST(test_multiple_triggers_with_clusters){

  I3GSLRandomService rand(99999); 

  I3DetectorStatus detStat = BuildGTHTestUtils::MakeDetectorStatus();    

  TriggerKey iiKey(TriggerKey::IN_ICE, TriggerKey::SIMPLE_MULTIPLICITY);

  GlobalTriggerSim gts(detStat);

  const int NCLUSTERS(50);
  const int CLUSTERSIZE(10);

  for(int nclusters(1); nclusters <= NCLUSTERS; nclusters++){ 
    for(int clustersize(2); clustersize <= CLUSTERSIZE; clustersize++){ 

      std::vector<I3Trigger> test_triggers = BuildGTHTestUtils::MakeTriggers(nclusters,iiKey,clustersize);
      
      ENSURE((int)test_triggers.size() == nclusters * clustersize);
      
      I3TriggerPairVector tTriggers;
      gts.InsertThroughputTriggers(test_triggers, tTriggers);  

      I3TriggerHierarchyPtr gTriggers = 
	gts.Merge(tTriggers);

      int nExpected(nclusters * ( 2 * clustersize + 1 ));

      std::cerr<<"gTriggers->size() = "<<gTriggers->size()<<std::endl;
      std::cerr<< "nExpected = "<<nExpected<<std::endl;

      std::cerr<<GTSUtils::Stringize(*gTriggers)<<std::endl;

      ENSURE(gTriggers->size() == nExpected );
      
      //there should be only one global throughput with this
      //trigger as a daughter
      for( I3TriggerHierarchy::fixed_depth_iterator gt_iter = gTriggers->begin_fixed(gTriggers->begin(),0) ;
	   gTriggers->is_valid(gt_iter); gt_iter++){
	
	ENSURE(gt_iter->GetTriggerKey().GetSource() == TriggerKey::GLOBAL);
	ENSURE( ( gt_iter->GetTriggerKey().GetType() == TriggerKey::MERGED && 
		  gTriggers->number_of_children(gt_iter) > 1 ) );
	
	//get the daughter
	ENSURE((int)gTriggers->number_of_children(gt_iter) == clustersize);

	std::vector<double> begin_times;
	std::vector<double> end_times;
	I3TriggerHierarchy::sibling_iterator c_iter(gt_iter);
	for( c_iter = gTriggers->begin(gt_iter) ; 
	     c_iter != gTriggers->end(gt_iter) ; c_iter++){
	  ENSURE(c_iter->GetTriggerKey().GetSource() == TriggerKey::GLOBAL);
	  ENSURE( ( c_iter->GetTriggerKey().GetType() == TriggerKey::THROUGHPUT && 
		    gTriggers->number_of_children(c_iter) == 1 ) );
	  
	  I3TriggerHierarchy::sibling_iterator gd_iter(c_iter);
	  for( gd_iter = gTriggers->begin(c_iter) ; 
	       gd_iter != gTriggers->end(c_iter) ; gd_iter++){
	    ENSURE(gd_iter->GetTriggerKey() == iiKey );
	  }
	  
	  begin_times.push_back(c_iter->GetTriggerTime());
	  end_times.push_back(c_iter->GetTriggerTime() + c_iter->GetTriggerLength());
	}

	double earliest(*min_element(begin_times.begin(),begin_times.end()));
	double latest(*max_element(end_times.begin(),end_times.end()));
	
	ENSURE_DISTANCE(gt_iter->GetTriggerLength(), latest - earliest,1*I3Units::nanosecond);
	ENSURE_DISTANCE(gt_iter->GetTriggerTime(), earliest,1*I3Units::nanosecond);
      }
    }
  }
}

