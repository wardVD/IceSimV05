/**
 * copyright  (C) 2004
 * the icecube collaboration
 * $Id: I3FilterResult.cxx 94949 2012-11-04 16:40:30Z nwhitehorn $
 *
 * @file I3FilterResult.cxx
 * @version $Revision: 94949 $
 * @date $Date: 2012-11-04 10:40:30 -0600 (Sun, 04 Nov 2012) $
 */

// class header file

#include <icetray/serialization.h>
#include <dataclasses/physics/I3FilterResult.h>

// definitions


// namespace declarations


// implementation

I3FilterResult::~I3FilterResult()
{
}


template <class Archive>
void I3FilterResult::serialize(Archive& ar, unsigned version)
{
  ar & make_nvp("I3FrameObject",base_object<I3FrameObject>(*this));
  ar & make_nvp("conditionPassed",conditionPassed);
  ar & make_nvp("prescalePassed",prescalePassed);
}

I3_SERIALIZABLE(I3FilterResult);

I3_SERIALIZABLE(I3FilterResultMap);
