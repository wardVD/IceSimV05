/**
 * copyright  (C) 2004
 * the icecube collaboration
 * @version $Id: I3RecoHit.h 135122 2015-07-14 18:54:17Z jtatar $
 * @file I3RecoHit.h
 * @date $Date: 2015-07-14 13:54:17 -0500 (Tue, 14 Jul 2015) $
 */

#ifndef I3RECOHIT_H_INCLUDED
#define I3RECOHIT_H_INCLUDED

#include "dataclasses/Utility.h"
#include "dataclasses/I3Vector.h"
#include "dataclasses/I3Map.h"
#include "icetray/OMKey.h"


/**
 * @brief A base class for reconstruted hits.  
 *
 * Carries no information beyond 'hit'
 * but can be sub-classed as part of a hit series reconstruction.
 *
 */
static const unsigned i3recohit_version_ = 1;

class I3RecoHit 
{
 public:
  I3RecoHit() : time_(NAN),hitID_(-1),sourceIndex_(-1){}

  ~I3RecoHit();

  double GetTime() const { return time_; }

  void SetTime(double time) { time_ = time; }

  int GetID() const { return hitID_; }

  void SetID(const int hitid) { hitID_ = hitid; }
/**
 *  GetSourceIndex - The Source Index can be used to keep track
 *    of which rawReadout index (from readout vector, like DOMLaunchSeries,
 *    that gave rise to this pariticular hit.  Index value defaults to -1.
 */
  int GetSourceIndex() const { return sourceIndex_; }
/**
 *  SetSourceIndex - The Source Index can be used to keep track
 *    of which rawReadout index (from readout vector, like DOMLaunchSeries,
 *    that gave rise to this pariticular hit.  Index value defaults to -1.
 */
  void SetSourceIndex(const int srcid) { sourceIndex_ = srcid; }

  bool operator==(const I3RecoHit& rhs) const ;

 private:

  double time_;

  int hitID_;

  int sourceIndex_;

  friend class boost::serialization::access;

  template <class Archive> void serialize(Archive & ar, unsigned version);
};


I3_POINTER_TYPEDEFS(I3RecoHit);
BOOST_CLASS_VERSION(I3RecoHit, i3recohit_version_);

typedef std::vector<I3RecoHit> I3RecoHitSeries;
typedef I3Map<OMKey, I3RecoHitSeries> I3RecoHitSeriesMap;

I3_POINTER_TYPEDEFS(I3RecoHitSeries);
I3_POINTER_TYPEDEFS(I3RecoHitSeriesMap);

#endif //I3RECOHIT_H_INCLUDED


