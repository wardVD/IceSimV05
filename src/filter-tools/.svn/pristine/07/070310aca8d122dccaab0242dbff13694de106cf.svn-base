/**
 * @author jdumm
 *
 * @brief This module takes an existing I3FilterResultMap and writes out each
 *    filter result individually to the frame as an I3Bool given by 
 *    conditionPassed AND (IgnorePrescale OR prescalePassed), where
 *    <em>IgnorePrescale</em> is a parameter of FilterMask2Bools.
 *    Original intent was to pass bools to the flat-ntuple module.
 */


#ifndef FILTERMASK2BOOLS_H_INCLUDED
#define FILTERMASK2BOOLS_H_INCLUDED


#include <string>

#include <icetray/I3Frame.h>
#include <icetray/I3Module.h>
#include <icetray/I3Logging.h>

class FilterMask2Bools : public I3Module 
{
 public:

  /** Constructor.
   * 
   * @param ctx The context with which this module is built.
   */
  FilterMask2Bools(const I3Context& ctx);
  /** Destructor.
   */
  virtual ~FilterMask2Bools();

  void Configure();
  void Physics(I3FramePtr frame);

 private:
  SET_LOGGER("FilterMask2Bools");

  // Name of FilterResultMap object in frame
  std::string filter_result_name_;
  
  // Whether or not to ignore filter result prescale booleans
  bool ignore_prescale_;
};

#endif


