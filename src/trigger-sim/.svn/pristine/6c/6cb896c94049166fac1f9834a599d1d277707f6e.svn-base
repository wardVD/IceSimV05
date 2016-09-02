#ifndef TRIGGER_HIT_H
#define TRIGGER_HIT_H

#include <vector>
#include <list>
#include "dataclasses/I3Map.h"

/**
 * @brief A simple hit class used by the triggers.
 */

class TriggerHit
{
 public:
  TriggerHit() : time(0), pos(0), string(0) {}
  TriggerHit(double aTime, unsigned int aPos, int aString) : time(aTime), pos(aPos), string(aString) {}

  double time;
  unsigned int pos;
  int string;

  bool operator<(const TriggerHit& rhs) const { return time < rhs.time; }
  bool operator==(const TriggerHit& rhs) const { return ( (pos == rhs.pos) && (time == rhs.time) && (string == rhs.string) ); }

};

typedef std::vector<TriggerHit> TriggerHitVector;
typedef std::vector<TriggerHitVector> TriggerHitVectorVector;
typedef I3Map<int, TriggerHitVector> IntTriggerHitVectorMap;
typedef I3Map<TriggerHit, int> TriggerHitIntMap;
typedef std::pair<TriggerHitVector::const_iterator, TriggerHitVector::const_iterator> TriggerHitIterPair;
typedef std::vector<TriggerHitIterPair> TriggerHitIterPairVector;
typedef std::list<TriggerHit> TriggerHitList;

I3_POINTER_TYPEDEFS(TriggerHit);
I3_POINTER_TYPEDEFS(TriggerHitVector);
I3_POINTER_TYPEDEFS(TriggerHitVectorVector);
I3_POINTER_TYPEDEFS(IntTriggerHitVectorMap);
I3_POINTER_TYPEDEFS(TriggerHitIntMap);
I3_POINTER_TYPEDEFS(TriggerHitIterPair);
I3_POINTER_TYPEDEFS(TriggerHitIterPairVector);
I3_POINTER_TYPEDEFS(TriggerHitList);

#endif // TRIGGER_HIT_H
