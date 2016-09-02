/**
    copyright  (C) 2004
    the icecube collaboration
    $Id: I3TensorOfInertiaFitParams.h 15503 2006-02-09 16:42:08Z pretz $

    @version $Revision: 1.1 $
    @date $Date: 2006-02-09 11:42:08 -0500 (Thu, 09 Feb 2006) $
    @author grullon
*/

#ifndef RECCLASSES_I3TENSOROFINERTIAFITRESULT_H
#define RECCLASSES_I3TENSOROFINERTIAFITRESULT_H

#include <icetray/I3FrameObject.h>

#include <boost/serialization/access.hpp>
#include <boost/serialization/version.hpp>

#include <cmath>

/**
 * @brief Stores the eigenvalues of a tensor of inertia (see project tensor-of-inertia).
 *
 * Output of the I3TensorOfInertia module.
 */
class I3TensorOfInertiaFitParams : public I3FrameObject
{

 public:
    double mineval;   ///< smallest eigenvalue
    double evalratio; ///< ratio of smallest eigenvalue and sum of eigenvalues
    double eval2;     ///< intermediate eigenvalue
    double eval3;     ///< largest eigenvalue

    I3TensorOfInertiaFitParams() :
      mineval(NAN),
      evalratio(NAN),
      eval2(NAN),
      eval3(NAN)
      {};

    virtual ~I3TensorOfInertiaFitParams() {};

 private:

  friend class boost::serialization::access;
  template <class Archive>
    void serialize(Archive& ar, unsigned version);

};

I3_POINTER_TYPEDEFS(I3TensorOfInertiaFitParams);

#endif
