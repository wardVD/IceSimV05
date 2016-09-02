/**
 * @brief Testing the I3DAQDecoderUtil class.
 * 
 * (c) 2005
 * the IceCube Collaboration
 * $Id$
 *
 * @file I3DAQDecoderUtilTest.cxx
 * @version $Revision$
 * @date $Date$
 * @author tschmidt
 */

// class header files

#include <daq-decode/I3DAQDecoderUtil.h>

#include <map>
#include <boost/assign/list_of.hpp>

#include <dataclasses/TriggerKey.h>
#include <dataclasses/physics/I3Trigger.h>

 #include <icetray/I3Units.h>


#include <I3Test.h>

// definitions


// namespace declarations

using namespace std;
using namespace boost::assign;

// implementation

TEST_GROUP(I3DAQDecoderUtilTest);

TEST(CreateTriggerKey)
{
  TriggerKey key =
    I3DAQDecoderUtil::CreateTriggerKey(I3DAQDecoderUtil::SIMPLE_MULTIPLICITY_TRIG_TYPE,
    	                               I3DAQDecoderUtil::NO_CONFIGURATION,
    	                               I3DAQDecoderUtil::IN_ICE_TRIG_ID);
  ENSURE(!key.CheckConfigID());

  key =
    I3DAQDecoderUtil::CreateTriggerKey(I3DAQDecoderUtil::SIMPLE_MULTIPLICITY_TRIG_TYPE,
    	                               1,
    	                               I3DAQDecoderUtil::IN_ICE_TRIG_ID);
  ENSURE(key.CheckConfigID());
  ENSURE_EQUAL(key.GetConfigID(), 1)

  map<int32_t, TriggerKey::SourceID> sourceIDs =
    map_list_of(I3DAQDecoderUtil::IN_ICE_TRIG_ID, TriggerKey::IN_ICE)
               (I3DAQDecoderUtil::ICE_TOP_TRIG_ID, TriggerKey::ICE_TOP)
               (I3DAQDecoderUtil::GLOBAL_TRIG_ID, TriggerKey::GLOBAL);
  map<int32_t, TriggerKey::TypeID> typeIDs =
    map_list_of(I3DAQDecoderUtil::SIMPLE_MULTIPLICITY_TRIG_TYPE, TriggerKey::SIMPLE_MULTIPLICITY)
               (I3DAQDecoderUtil::CALIBRATION_TRIG_TYPE, TriggerKey::CALIBRATION)
               (I3DAQDecoderUtil::MIN_BIAS_TRIG_TYPE, TriggerKey::MIN_BIAS)
               (I3DAQDecoderUtil::THROUGHPUT_TRIG_TYPE, TriggerKey::THROUGHPUT)
               (I3DAQDecoderUtil::TWO_COINCIDENCE_TRIG_TYPE, TriggerKey::TWO_COINCIDENCE)
               (I3DAQDecoderUtil::THREE_COINCIDENCE_TRIG_TYPE, TriggerKey::THREE_COINCIDENCE)
               (I3DAQDecoderUtil::PHYSICS_MIN_BIAS_TRIG_TYPE, TriggerKey::MIN_BIAS)
               (I3DAQDecoderUtil::CLUSTER_STRING_TRIG_TYPE, TriggerKey::STRING)
               (I3DAQDecoderUtil::MULTIPLICITY_STRING_TRIG_TYPE, TriggerKey::STRING)
               (I3DAQDecoderUtil::CYLINDER_VOLUME_TRIG_TYPE, TriggerKey::VOLUME)
               (I3DAQDecoderUtil::SLOW_PARTICLE_TRIG_TYPE, TriggerKey::SLOW_PARTICLE)
               (I3DAQDecoderUtil::SLOW_PARTICLE2_TRIG_TYPE, TriggerKey::SLOW_PARTICLE)
               (I3DAQDecoderUtil::FIXED_RATE_TRIG_TYPE, TriggerKey::UNBIASED)
               (I3DAQDecoderUtil::MERGED_TRIG_TYPE, TriggerKey::MERGED);
  for(map<int32_t, TriggerKey::SourceID>::const_iterator iter = sourceIDs.begin();
  	  iter != sourceIDs.end();
  	  ++iter)
  {
  	for(map<int32_t, TriggerKey::TypeID>::const_iterator jter = typeIDs.begin();
  		jter != typeIDs.end();
  		++jter)
  	{
  	  key =
        I3DAQDecoderUtil::CreateTriggerKey(jter->first,
    	                                   I3DAQDecoderUtil::NO_CONFIGURATION,
    	                                   iter->first);
      ENSURE_EQUAL(key, TriggerKey(iter->second, jter->second));
  	}
  }
}


TEST(FillTrigger)
{
  I3Trigger trig;
  I3DAQDecoderUtil::FillTrigger(trig,
  	                            100 * I3Units::nanosecond,
  	                            2000 * I3Units::nanosecond,
  	                            I3DAQDecoderUtil::SIMPLE_MULTIPLICITY_TRIG_TYPE,
    	                        I3DAQDecoderUtil::NO_CONFIGURATION,
    	                        I3DAQDecoderUtil::IN_ICE_TRIG_ID);
  ENSURE_EQUAL(trig.GetTriggerTime(), 100 * I3Units::nanosecond);
  ENSURE_EQUAL(trig.GetTriggerLength(), 2000 * I3Units::nanosecond);
  ENSURE(trig.GetTriggerFired());
  ENSURE_EQUAL(trig.GetTriggerKey(), TriggerKey(TriggerKey::IN_ICE, TriggerKey::SIMPLE_MULTIPLICITY));
}

