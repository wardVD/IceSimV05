/**
    $Id$
    copyright  (C) 2004
    the icecube collaboration
    $Id: I3DipoleFitParams.h 12151 2005-11-04 05:53:22Z hagar $

    @version $Revision: 1.2 $
    @date $Date: 2005-11-04 00:53:22 -0500 (Fri, 04 Nov 2005) $
    @author deyoung
*/

#ifndef RECCLASSES_I3DIPOLEFITPARAMS_H
#define RECCLASSES_I3DIPOLEFITPARAMS_H

#include <icetray/I3FrameObject.h>
#include <boost/serialization/access.hpp>
#include <cmath>
#include <climits>

/**
 *  @brief Store results of I3DipoleFit module (see project dipolefit).
 */
class I3DipoleFitParams : public I3FrameObject
{

 public:
  double magnet;
  double magnetX;
  double magnetY;
  double magnetZ;
  double ampSum;
  int    nHits;
  int    nPairs;
  double maxAmp;

  I3DipoleFitParams() :
    magnet(NAN),
    magnetX(NAN),
    magnetY(NAN),
    magnetZ(NAN),
    ampSum(NAN),
    nHits(INT_MIN),
    nPairs(INT_MIN),
    maxAmp(NAN)
    {};

  virtual ~I3DipoleFitParams() {};

  bool operator==(const I3DipoleFitParams&) const;

 private:
  friend class boost::serialization::access;
  template <class Archive>
    void serialize(Archive& ar, unsigned version);

};

std::ostream& operator<<(std::ostream&, const I3DipoleFitParams&);

I3_POINTER_TYPEDEFS(I3DipoleFitParams);

#endif
