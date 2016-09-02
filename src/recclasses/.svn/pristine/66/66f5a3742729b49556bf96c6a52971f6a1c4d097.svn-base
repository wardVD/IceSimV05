/**
 * @version $Revision: 54885 $
 * @date $Date: 2009-05-11 15:24:36 +0200 (Mon, 11 May 2009) $
 * @author Sebastian Euler <sebastian.euler@icecube.wisc.edu>  
 */

#ifndef RECCLASSES_I3STARTSTOPPARAMS_H_INCLUDED
#define RECCLASSES_I3STARTSTOPPARAMS_H_INCLUDED

#include "icetray/I3FrameObject.h"
#include "icetray/I3Logging.h"

#include <boost/serialization/access.hpp>
#include <boost/serialization/version.hpp>

#include <string>
#include <ostream>

/**
 * @brief Stores likelihood a starting, stopping, and 
 * infinite track hypothesis (see project finiteReco).
 *
 * Store the results of the I3StartStopLProb module.
 * For details look into project finiteReco.
 */
class I3StartStopParams : public I3FrameObject
{
 public:
  /**
     log likelihood for a starting track
  */
  double LLHStartingTrack;
  /**
     log likelihood for a stopping track
   */
  double LLHStoppingTrack;  
  /**
     log likelihood for an infinite track
   */
  double LLHInfTrack;

  I3StartStopParams() { Reset(); }

  virtual ~I3StartStopParams(){}
  
  virtual void Reset();

private:
  friend class boost::serialization::access;

  template <class Archive>
    void serialize(Archive& ar, unsigned version);
  
  SET_LOGGER( "I3StartStopParams" );
};

std::ostream& operator<<(std::ostream&, const I3StartStopParams&);

I3_POINTER_TYPEDEFS(I3StartStopParams);

#endif
