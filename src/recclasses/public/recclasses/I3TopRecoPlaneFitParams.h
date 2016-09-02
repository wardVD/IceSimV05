/**
 * Copyright (C) 2006
 * The IceCube collaboration
 * ID: $Id: I3TopRecoPlaneFitParams.h 143992 2016-03-30 19:46:40Z hdembinski $
 *
 * @version $Rev: 28258 $
 * @date $Date: 2016-03-30 14:46:40 -0500 (Wed, 30 Mar 2016) $
 * @author $Author: schlenst $
 */

#ifndef RECCLASSES_I3TOPRECOPLANEFITPARAMS_H_
#define RECCLASSES_I3TOPRECOPLANEFITPARAMS_H_

#include <icetray/I3FrameObject.h>
#include <boost/serialization/access.hpp>
#include <boost/serialization/version.hpp>
#include <ostream>

static const unsigned i3toprecoplanefitparams_version_ = 1;

/**
 * @brief Stores description of a plane shower front (see project toprec).
 *
 * Intention is to use this as a first guess for the fit of more realistic
 * model of the shower front. Adds a chi2 and a t0 to the fit.
 */
class I3TopRecoPlaneFitParams : public I3FrameObject
{

  public:

  /**
   * The event time in secs since year began
   */
  int64_t ET;

  /**
   * Time which passed since the last event
   */
  double DeltaT;

  /**
   * t0 of the track
   */
  float T0;
  
  /**
   * x0 of the track
   */
  float X0;
  
  /**
   * y0 of the track
   */
  float Y0;
  
  /**
   * chi2 of the plane fit
   */
  float Chi2;

  /**
   * Number of pulses used in this fit
   */
  unsigned int NPulses;


  I3TopRecoPlaneFitParams() :
    ET(-1),
    DeltaT(NAN),
    T0(NAN),
    X0(NAN),
    Y0(NAN),
    Chi2(NAN),
    NPulses(0)
    {};

  virtual ~I3TopRecoPlaneFitParams() {};

 private:

  friend class boost::serialization::access;
  template <class Archive>
    void serialize(Archive& ar, unsigned version);

};

std::ostream& operator<<(std::ostream&, const I3TopRecoPlaneFitParams&);
       
BOOST_CLASS_VERSION(I3TopRecoPlaneFitParams, i3toprecoplanefitparams_version_);       

I3_POINTER_TYPEDEFS(I3TopRecoPlaneFitParams);

#endif
