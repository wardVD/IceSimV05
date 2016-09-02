 /**
 * class: GTSUtils
 *
 * Version $Id: $
 *
 *
 * (c) 2009 IceCube Collaboration
 * @file GTSUtils.cxx
 * @date $Date: $
 * @author olivas
 **/

#include "trigger-sim/utilities/GTSUtils.h"
#include "dataclasses/physics/I3Trigger.h"
#include <boost/foreach.hpp>

using namespace std;

void GTSUtils::TriggerPrettyPrint(const I3Trigger& t, std::stringstream& s){
  s<<t.GetTriggerKey().GetSourceString()<<" "
   <<t.GetTriggerKey().GetTypeString()<<" "
   <<" t = "<<t.GetTriggerTime()<<" "
   <<" length = "<<t.GetTriggerLength()<<" "
   <<" fired = "<<t.GetTriggerFired()<<" "
   <<endl;
}

void GTSUtils::DumpChildren(const I3TriggerHierarchy& t,
					 I3TriggerHierarchy::iterator i,
					 std::stringstream& s){
  I3TriggerHierarchy::sibling_iterator si;
  for(si = t.begin(i); si != t.end(i); si++){
    for(int j=0; j<2*t.depth(si); j++) s<<" ";
    TriggerPrettyPrint(*si,s);
    DumpChildren(t,si,s);
  }
}

string GTSUtils::Stringize(const I3TriggerHierarchy& t){
  std::stringstream s;
  I3TriggerHierarchy::sibling_iterator i;
  s<<"*** HIERARCHY DUMP - BEGIN***"<<endl;
  s<<"*** "<<t.size()<<" elements"<<endl;
  for(i = t.begin(); i != t.end(); i++){
    for(int j=0; j<2*t.depth(i); j++) s<<" ";
    TriggerPrettyPrint(*i,s);
    DumpChildren(t,i,s);
  }
  s<<"*** HIERARCHY DUMP - END***"<<endl;
  return s.str();
}

string GTSUtils::Stringize(I3TriggerHierarchyPtr t){
  return GTSUtils::Stringize(*t);
}

bool GTSUtils::LessThan(I3Trigger i, I3Trigger j){
  return i.GetTriggerTime() < j.GetTriggerTime();
}


I3TriggerStatus::Subdetector 
GTSUtils::KeyToSubDetector(const TriggerKey& k){
  I3TriggerStatus::Subdetector subDet(I3TriggerStatus::NOT_SPECIFIED);
  if(k.GetSource() == TriggerKey::IN_ICE)
    subDet = I3TriggerStatus::INICE;
  else if(k.GetSource() == TriggerKey::ICE_TOP)
    subDet = I3TriggerStatus::ICETOP;  
  return subDet;
}
