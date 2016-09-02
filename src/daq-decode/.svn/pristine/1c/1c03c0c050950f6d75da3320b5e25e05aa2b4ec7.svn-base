/**
 * @brief Implementation of I3DAQDecoderUtil class.
 * 
 * (c) 2005
 * the IceCube Collaboration
 * $Id$
 *
 * @file I3DAQDecoderUtil.cxx
 * @version $Revision$
 * @date $Date$
 * @author tschmidt
 */

// class header files

#include <daq-decode/I3DAQDecoderUtil.h>

#include <dataclasses/TriggerKey.h>

// definitions


// namespace declarations

using namespace std;

// implementation

const unsigned int I3DAQDecoderUtil::MAX_ATWD_CHANNEL = 4u;
const unsigned int I3DAQDecoderUtil::MAX_IN_ICE_OM_NUMBER = 60u;
const double I3DAQDecoderUtil::RAW_TIME_COUNTS_TO_NS_CONVERSION_FACTOR = 0.1;
const int32_t I3DAQDecoderUtil::SIMPLE_MULTIPLICITY_TRIG_TYPE = 0;
const int32_t I3DAQDecoderUtil::CALIBRATION_TRIG_TYPE = 1;
const int32_t I3DAQDecoderUtil::MIN_BIAS_TRIG_TYPE = 2;
const int32_t I3DAQDecoderUtil::THROUGHPUT_TRIG_TYPE = 3;
const int32_t I3DAQDecoderUtil::TWO_COINCIDENCE_TRIG_TYPE = 4;
const int32_t I3DAQDecoderUtil::THREE_COINCIDENCE_TRIG_TYPE = 5;
const int32_t I3DAQDecoderUtil::AMANDA_MULTIPLICITY_TRIG_TYPE = 6;
const int32_t I3DAQDecoderUtil::TWR_M20_FRAG_BIT_TRIG_TYPE = 7;
const int32_t I3DAQDecoderUtil::TWR_VOLUMEN_TRIG_BIT_TRIG_TYPE = 8;
const int32_t I3DAQDecoderUtil::TWR_M18_TRIG_BIT_TRIG_TYPE = 9;
const int32_t I3DAQDecoderUtil::TWR_M24_TRIG_BIT_TRIG_TYPE = 10;
const int32_t I3DAQDecoderUtil::TWR_STRING_TRIG_BIT_TRIG_TYPE = 11;
const int32_t I3DAQDecoderUtil::TWR_RANDOM_TRIG_BIT_TRIG_TYPE = 12;
const int32_t I3DAQDecoderUtil::PHYSICS_MIN_BIAS_TRIG_TYPE = 13;
const int32_t I3DAQDecoderUtil::CLUSTER_STRING_TRIG_TYPE = 14;
const int32_t I3DAQDecoderUtil::TWR_T0_CAL_TRIG_BIT_TRIG_TYPE = 15;
const int32_t I3DAQDecoderUtil::TWR_LASER_CAL_TRIG_BIT_TRIG_TYPE = 16;
const int32_t I3DAQDecoderUtil::MULTIPLICITY_STRING_TRIG_TYPE = 17;
const int32_t I3DAQDecoderUtil::CYLINDER_VOLUME_TRIG_TYPE = 21;
const int32_t I3DAQDecoderUtil::SLOW_PARTICLE_TRIG_TYPE = 22;
const int32_t I3DAQDecoderUtil::SLOW_PARTICLE2_TRIG_TYPE = 24;
const int32_t I3DAQDecoderUtil::FIXED_RATE_TRIG_TYPE = 23;
const int32_t I3DAQDecoderUtil::MERGED_TRIG_TYPE = -1;
  
const int32_t I3DAQDecoderUtil::IN_ICE_TRIG_ID = 4000;
const int32_t I3DAQDecoderUtil::ICE_TOP_TRIG_ID = 5000;
const int32_t I3DAQDecoderUtil::AMANDA_TRIG_ID = 10000;
const int32_t I3DAQDecoderUtil::GLOBAL_TRIG_ID = 6000;

const int32_t I3DAQDecoderUtil::NO_CONFIGURATION = -1;

const int32_t I3DAQDecoderUtil::DOM_TRIGGER_TYPE_MASK = 0x03;
const uint32_t I3DAQDecoderUtil::DOM_TRIGGER_TYPE_FSB = 0x00;
const int32_t I3DAQDecoderUtil::DOM_TRIGGER_MODE_MASK = 0x1f;
const uint32_t I3DAQDecoderUtil::DOM_TRIGGER_MODE_FSB = 0x03;

void
I3DAQDecoderUtil::FillTrigger(I3Trigger& trig,
                              double trigTime, double trigLength,
                              int32_t trigType, int32_t trigConfigId, int32_t trigSrcId)
{
  trig.SetTriggerTime(trigTime);        // trigger time
  trig.SetTriggerLength(trigLength);    // trigger window
  trig.SetTriggerFired(true);           // trigger flag
  trig.GetTriggerKey() =                // trigger key
    CreateTriggerKey(trigType, trigConfigId, trigSrcId);
}


TriggerKey
I3DAQDecoderUtil::CreateTriggerKey(int32_t trigType, int32_t trigConfigId, int32_t trigSrcId)  
{
  TriggerKey retVal(TriggerKey::UNKNOWN_SOURCE,
                    TriggerKey::UNKNOWN_TYPE,
                    TriggerKey::NO_SUBTYPE);
  
  switch(trigSrcId)                     // trigger source
  {
    case IN_ICE_TRIG_ID:
      retVal.SetSource(TriggerKey::IN_ICE);
      break;
    case ICE_TOP_TRIG_ID:
      retVal.SetSource(TriggerKey::ICE_TOP);
      break;
    case AMANDA_TRIG_ID:
      retVal.SetSource(TriggerKey::AMANDA_TWR_DAQ);
      break;
    case GLOBAL_TRIG_ID:
      retVal.SetSource(TriggerKey::GLOBAL);
      break;
  }
  switch(trigType)                      // trigger type and subtype
  {
    case SIMPLE_MULTIPLICITY_TRIG_TYPE:
      retVal.SetType(TriggerKey::SIMPLE_MULTIPLICITY);
      break;
    case CALIBRATION_TRIG_TYPE:
      retVal.SetType(TriggerKey::CALIBRATION);
      break;
    case MIN_BIAS_TRIG_TYPE:
      retVal.SetType(TriggerKey::MIN_BIAS);
      break;
    case THROUGHPUT_TRIG_TYPE:
      retVal.SetType(TriggerKey::THROUGHPUT);
      break;
    case TWO_COINCIDENCE_TRIG_TYPE:
      retVal.SetType(TriggerKey::TWO_COINCIDENCE);
      break;
    case THREE_COINCIDENCE_TRIG_TYPE:
      retVal.SetType(TriggerKey::THREE_COINCIDENCE);
      break;
    case AMANDA_MULTIPLICITY_TRIG_TYPE:
      // this doesn't cause an ambiguity since this trigType will only be generated
      // for trigSrcId AMANDA_TRIG_ID ... trigType SIMPLE_MULTIPLICITY_TRIG_TYPE will
      // never be generated for this trigSrcId
      retVal.SetType(TriggerKey::SIMPLE_MULTIPLICITY);
      break;
    case TWR_M20_FRAG_BIT_TRIG_TYPE:
      retVal.SetType(TriggerKey::FRAGMENT_MULTIPLICITY);
      break;
    case TWR_VOLUMEN_TRIG_BIT_TRIG_TYPE:
      retVal.SetType(TriggerKey::VOLUME);
      break;
    case TWR_M18_TRIG_BIT_TRIG_TYPE:
      retVal.SetType(TriggerKey::SIMPLE_MULTIPLICITY);
      retVal.SetSubtype(TriggerKey::M18);
      break;
    case TWR_M24_TRIG_BIT_TRIG_TYPE:
      retVal.SetType(TriggerKey::SIMPLE_MULTIPLICITY);
      retVal.SetSubtype(TriggerKey::M24);
      break;
    case TWR_STRING_TRIG_BIT_TRIG_TYPE:
      retVal.SetType(TriggerKey::STRING);
      break;
    case TWR_RANDOM_TRIG_BIT_TRIG_TYPE:
      retVal.SetType(TriggerKey::MIN_BIAS);
      break;
    case PHYSICS_MIN_BIAS_TRIG_TYPE:
      retVal.SetType(TriggerKey::MIN_BIAS);
      break;
    case MULTIPLICITY_STRING_TRIG_TYPE:
      retVal.SetType(TriggerKey::STRING);
      break;
    case CLUSTER_STRING_TRIG_TYPE:
      retVal.SetType(TriggerKey::STRING);
      break;
    case TWR_T0_CAL_TRIG_BIT_TRIG_TYPE:
      retVal.SetType(TriggerKey::CALIBRATION);
      retVal.SetSubtype(TriggerKey::T0);
      break;
    case TWR_LASER_CAL_TRIG_BIT_TRIG_TYPE:
      retVal.SetType(TriggerKey::CALIBRATION);
      retVal.SetSubtype(TriggerKey::LASER);
      break;
    case MERGED_TRIG_TYPE:
      retVal.SetType(TriggerKey::MERGED);
      break;
    case CYLINDER_VOLUME_TRIG_TYPE:
      retVal.SetType(TriggerKey::VOLUME);
      break;
    case SLOW_PARTICLE_TRIG_TYPE:
      retVal.SetType(TriggerKey::SLOW_PARTICLE);
      break;
    case SLOW_PARTICLE2_TRIG_TYPE:
      retVal.SetType(TriggerKey::SLOW_PARTICLE);
      break;
    case FIXED_RATE_TRIG_TYPE:
      retVal.SetType(TriggerKey::UNBIASED);
      break;
  }
  if(trigConfigId != NO_CONFIGURATION)  // trigger configuration
    retVal.SetConfigID(trigConfigId);
  
  return(retVal);
}


I3TriggerHierarchy::iterator
I3DAQDecoderUtil::AddTrigger(I3TriggerHierarchy& triggers,
                             I3TriggerHierarchy::iterator iter,
                             const I3Trigger& trig)
{
  if(triggers.is_valid(iter))
    return triggers.append_child(iter, trig);
  else
    return triggers.insert(triggers.begin(), trig);
}


I3DAQDecoderUtil::I3DAQDecoderUtil()
{
}


I3DAQDecoderUtil::~I3DAQDecoderUtil()
{
}
