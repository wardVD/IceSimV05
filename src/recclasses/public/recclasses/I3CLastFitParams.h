/**
    $Id: I3CLastFitParams.h 15503 2006-02-09 16:42:08Z pretz $
    copyright  (C) 2004
    the icecube collaboration

    @version $Revision: 1.1 $
    @date $Date: 2006-02-09 11:42:08 -0500 (Thu, 09 Feb 2006) $
    @author toale (derived from TensorOfInteria code by grullon)
*/

#ifndef _I3_CLAST_FIT_PARAMS_H_
#define _I3_CLAST_FIT_PARAMS_H_

#include <icetray/I3FrameObject.h>
#include "dataclasses/Utility.h"


/**
 * @brief Store parameters of tensor of inertia from I3CLastModule (see project clast).
 */

class I3CLastFitParams : public I3FrameObject
{

 public:
    double mineval;
    double evalratio;
    double eval2;
    double eval3;

    I3CLastFitParams() :
      mineval(NAN),
      evalratio(NAN),
      eval2(NAN),
      eval3(NAN)
      {};

    virtual ~I3CLastFitParams() {};

    bool operator==(const I3CLastFitParams&) const;

 private:

  friend class boost::serialization::access;
  template <class Archive> void serialize(Archive& ar, unsigned version);

};

I3_POINTER_TYPEDEFS(I3CLastFitParams);

#endif

