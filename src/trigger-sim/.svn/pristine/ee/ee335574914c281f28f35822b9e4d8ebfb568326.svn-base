#ifndef SLOW_MP_HIT_H
#define SLOW_MP_HIT_H

#include <vector>
#include <list>
#include "dataclasses/I3Map.h"

/**
 * Slow_MP_Hit
 *
 * A simple hit class used by the trigger-sim.
 * derived from trigger-sim/TriggerHit.h
 *
 */

class SlowMPHit
{
 public:
  SlowMPHit() : time(0), pos(0), string(0) {}
  SlowMPHit(double aTime, unsigned int aPos, int aString) : time(aTime), pos(aPos), string(aString) {}

  double time;
  int pos;
  int string;
  double x;
  double y;
  double z;

  bool operator<(const SlowMPHit& rhs) const { return time < rhs.time; }
  bool operator==(const SlowMPHit& rhs) const { return ( (pos == rhs.pos) && (time == rhs.time) && (string == rhs.string) ); }

};

typedef std::vector<SlowMPHit> SlowMPHitVector;
typedef std::vector<SlowMPHitVector> SlowMPHitVectorVector;
typedef I3Map<int, SlowMPHitVector> IntSlowMPHitVectorMap;
typedef I3Map<SlowMPHit, int> SlowMPHitIntMap;
typedef std::pair<SlowMPHitVector::const_iterator, SlowMPHitVector::const_iterator> SlowMPHitIterPair;
typedef std::vector<SlowMPHitIterPair> SlowMPHitIterPairVector;
typedef std::list<SlowMPHit> SlowMPHitList;

I3_POINTER_TYPEDEFS(SlowMPHit);
I3_POINTER_TYPEDEFS(SlowMPHitVector);
I3_POINTER_TYPEDEFS(SlowMPHitVectorVector);
I3_POINTER_TYPEDEFS(IntSlowMPHitVectorMap);
I3_POINTER_TYPEDEFS(SlowMPHitIntMap);
I3_POINTER_TYPEDEFS(SlowMPHitIterPair);
I3_POINTER_TYPEDEFS(SlowMPHitIterPairVector);
I3_POINTER_TYPEDEFS(SlowMPHitList);

typedef std::vector<SlowMPHitPtr> SlowMPHitPtrVector;

#endif // SLOW_MP_HIT_H
