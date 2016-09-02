/**
    copyright  (C) 2004
    the icecube collaboration
    @version $Id: I3MCList.h 135122 2015-07-14 18:54:17Z jtatar $
    @file dataclasses/public/dataclasses/physics/I3MCList.h
    @date $Date: 2015-07-14 13:54:17 -0500 (Tue, 14 Jul 2015) $
*/

#ifndef I3MCLLIST_H_INCLUDED
#define I3MCLLIST_H_INCLUDED

#include "dataclasses/physics/I3Particle.h"
#include "dataclasses/I3Vector.h"

/**
 * @brief Track is for vanilla track (i.e. not cascade)
  */
static const unsigned i3mclist_version_ = 0;

class I3MCList : public I3Vector<I3Particle>
{
 public:

  I3MCList()
    {};
  
  I3MCList(I3Vector<I3Particle>::iterator l,I3Vector<I3Particle>::iterator r) : I3Vector<I3Particle>(l, r) { }

  ~I3MCList();

 private:

  friend class boost::serialization::access;

  template <class Archive> void serialize(Archive & ar, unsigned version);
};

BOOST_CLASS_VERSION(I3MCList, i3mclist_version_);
I3_POINTER_TYPEDEFS(I3MCList);

#endif 


