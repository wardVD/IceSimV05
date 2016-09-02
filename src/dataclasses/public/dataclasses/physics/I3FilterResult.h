/**
 * copyright  (C) 2004
 * the icecube collaboration
 * $Id: I3FilterResult.h 135122 2015-07-14 18:54:17Z jtatar $
 *
 * @file I3FilterResult.h
 * @version $Revision: 135122 $
 * @date $Date: 2015-07-14 13:54:17 -0500 (Tue, 14 Jul 2015) $
 */
#ifndef I3FILTERRESULT_H_INCLUDED
#define I3FILTERRESULT_H_INCLUDED

#include <string>

#include <icetray/I3FrameObject.h>
#include <icetray/I3PointerTypedefs.h>
#include <dataclasses/I3Map.h>


/** The filtering result information from JEB + PnF.
 * 
 * Filtering is a five step process:
 * <ol>
 * <li> Prepare data (decoding, calibration, feature extraction etc.),
 * <li> reconstruct event,
 * <li> make a filter decision (drop an I3Boolean with a name equal
 * to the corresponding filter stream into the frame that is true
 * for "good" events),
 * <li> prescale "good" events and compile the filter results,
 * <li> write all events into raw and good events into filtered data stream.
 * </ol>
 */
struct I3FilterResult : public I3FrameObject
{
  // @var conditionPassed True for "good" events.
  bool conditionPassed;

  //@var prescalePassed True for "good" events that pass prescaling.
  bool prescalePassed;
  
  /** Default constructor.
   */
  I3FilterResult() : conditionPassed(false), prescalePassed(false) {}
  /** Destructor.
   */
  ~I3FilterResult();
  
  template <class Archive>
  void serialize(Archive& ar,unsigned version);
};

// @typedef I3FilterResultMap Filter stream names vs. filter results.
typedef I3Map<std::string,I3FilterResult> I3FilterResultMap;

I3_POINTER_TYPEDEFS(I3FilterResult);
I3_POINTER_TYPEDEFS(I3FilterResultMap);

#endif
