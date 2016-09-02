/**
    $Id: I3LineFitParams.h 15503 2006-02-09 16:42:08Z pretz $
    copyright  (C) 2004
    the icecube collaboration

    @version $Revision: 1.1 $
    @date $Date: 2006-02-09 11:42:08 -0500 (Thu, 09 Feb 2006) $
    @author deyoung
*/

#ifndef RECCLASSES_I3LINEFITRESULT_H
#define RECCLASSES_I3LINEFITRESULT_H

#include <dataclasses/Utility.h>
#include <icetray/I3FrameObject.h>
#include <climits>
#include <cmath>

static const unsigned i3linefitparams_version_ = 1;

/**
 * @brief A class to store the results of the linefit
 */
class I3LineFitParams : public I3FrameObject
{

 public:
    double LFVel;
    double LFVelX;
    double LFVelY;
    double LFVelZ;
    int nHits;

    I3LineFitParams() :
      LFVel(NAN),
      LFVelX(NAN),
      LFVelY(NAN),
      LFVelZ(NAN),
      nHits(INT_MIN)
    {}

    virtual ~I3LineFitParams() {}

    bool operator==(const I3LineFitParams&) const;

 private:

  friend class boost::serialization::access;
  template <class Archive>
    void serialize(Archive& ar, unsigned version);

};

std::ostream& operator<<(std::ostream&, const I3LineFitParams&);

I3_POINTER_TYPEDEFS(I3LineFitParams);
BOOST_CLASS_VERSION(I3LineFitParams, i3linefitparams_version_);

#endif
