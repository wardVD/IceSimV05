/**
 * Definition of I3DAQDecoderUtil class
 *
 * (c) 2005
 * the IceCube Collaboration
 * $Id$
 *
 * @file I3DAQDecoderUtil.h
 * @version $Revision$
 * @date $Date$
 * @author tschmidt
 */

#ifndef I3_DAQ_DECODER_UTIL_H_INCLUDED
#define I3_DAQ_DECODER_UTIL_H_INCLUDED

// forward declarations

class I3Trigger;

// header files

#include <stdint.h>

#include <icetray/I3Logging.h>
#include <dataclasses/TriggerKey.h>
#include <dataclasses/physics/I3TriggerHierarchy.h>

// definitions


/**
 * @brief This is a utility class that offers commonly needed functions
 * to decode I3 DAQ data.
 */
class I3DAQDecoderUtil
{
 public:
  static const unsigned int MAX_ATWD_CHANNEL;
  
  static const unsigned int MAX_IN_ICE_OM_NUMBER;
  
  static const double RAW_TIME_COUNTS_TO_NS_CONVERSION_FACTOR;
  
  static const int32_t SIMPLE_MULTIPLICITY_TRIG_TYPE;
  static const int32_t CALIBRATION_TRIG_TYPE;
  static const int32_t MIN_BIAS_TRIG_TYPE;
  static const int32_t THROUGHPUT_TRIG_TYPE;
  static const int32_t TWO_COINCIDENCE_TRIG_TYPE;
  static const int32_t THREE_COINCIDENCE_TRIG_TYPE;
  static const int32_t AMANDA_MULTIPLICITY_TRIG_TYPE;
  static const int32_t TWR_M20_FRAG_BIT_TRIG_TYPE;
  static const int32_t TWR_VOLUMEN_TRIG_BIT_TRIG_TYPE;
  static const int32_t TWR_M18_TRIG_BIT_TRIG_TYPE;
  static const int32_t TWR_M24_TRIG_BIT_TRIG_TYPE;
  static const int32_t TWR_STRING_TRIG_BIT_TRIG_TYPE;
  static const int32_t TWR_RANDOM_TRIG_BIT_TRIG_TYPE;
  static const int32_t PHYSICS_MIN_BIAS_TRIG_TYPE;
  static const int32_t CLUSTER_STRING_TRIG_TYPE;
  static const int32_t TWR_T0_CAL_TRIG_BIT_TRIG_TYPE;
  static const int32_t TWR_LASER_CAL_TRIG_BIT_TRIG_TYPE;
  static const int32_t MULTIPLICITY_STRING_TRIG_TYPE;
  static const int32_t CYLINDER_VOLUME_TRIG_TYPE;
  static const int32_t SLOW_PARTICLE_TRIG_TYPE;
  static const int32_t SLOW_PARTICLE2_TRIG_TYPE;
  static const int32_t FIXED_RATE_TRIG_TYPE;
  static const int32_t MERGED_TRIG_TYPE;
  
  static const int32_t IN_ICE_TRIG_ID;
  static const int32_t ICE_TOP_TRIG_ID;
  static const int32_t AMANDA_TRIG_ID;
  static const int32_t GLOBAL_TRIG_ID;

  static const int32_t NO_CONFIGURATION;

  static const int32_t DOM_TRIGGER_TYPE_MASK;
  static const uint32_t DOM_TRIGGER_TYPE_FSB;
  static const int32_t DOM_TRIGGER_MODE_MASK;
  static const uint32_t DOM_TRIGGER_MODE_FSB;

  /**
   * @brief Decodes POD from an I3 DAQ event and fills a trigger.
   *
   * @param trig The trigger.
   * @param trigTime Time at which the trigger was issued
   * (in I3Units).
   * @param trigLength Duration of triggered readout window.
   * (in I3Units)
   * @param trigType Type of trigger (s. trigger payload).
   * @param trigConfId Config ID of trigger (s. trigger payload).
   * @param trigSrcId Source of the trigger (s. trigger payload).
   */
  static void
  FillTrigger(I3Trigger& trig,
              double trigTime, double trigLength,
              int32_t trigType, int32_t trigConfId, int32_t trigSrcId);
  /**
   * @brief Decodes POD from an I3 DAQ event and creates a trigger key.
   * 
   * @param trigType Type of trigger (s. trigger payload).
   * @param trigConfId Config ID of trigger (s. trigger payload).
   * @param trigSrcId Source of the trigger (s. trigger payload).
   * @return The trigger key.
   */
  static TriggerKey
  CreateTriggerKey(int32_t trigType, int32_t trigConfId, int32_t trigSrcId);
  /**
   * @brief Adds a trigger to a trigger hierarchy.
   *
   * @param triggers Trigger hierarchy.
   * @param iter Parent trigger that 'contains' the given trigger among others
   * (iter should be invalid (pass an I3TriggerHierarchy::iterator()), if there is
   * no parent trigger existing).
   * @param trig Trigger.
   * @return Position to add a sibling-like trigger to.
   */
  static I3TriggerHierarchy::iterator
  AddTrigger(I3TriggerHierarchy& triggers,
             I3TriggerHierarchy::iterator iter,
             const I3Trigger& trig);
  
  
  /**
   * @brief Default constructor.
   */
  I3DAQDecoderUtil();
  /**
   * @brief Destructor.
   */
  virtual ~I3DAQDecoderUtil();

 private:
  // logging
  SET_LOGGER("I3DAQDecoderUtil");
};

#endif /* I3_DAQ_DECODER_UTIL_H_INCLUDED */
