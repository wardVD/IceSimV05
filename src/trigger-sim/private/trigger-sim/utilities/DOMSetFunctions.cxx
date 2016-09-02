/**
 * copyright  (C) 2004
 * the icecube collaboration
 *  $Id: DOMSetFunctions.cxx 2009/08/04 21: olivas Exp $
 *
 * @file DOMSetFunctions.cxx
 * @version $Revision: 1.1.1.1 $
 * @date $Date: 2004/11/06 21:00:46 $
 * @author olivas
 */

#include "trigger-sim/utilities/DOMSetFunctions.h"
#include <vector>
#include <algorithm>
#include <boost/foreach.hpp>
#include <boost/assign/std/vector.hpp>
#include <boost/assign/list_of.hpp>
#include <boost/foreach.hpp>

using boost::assign::list_of;
using std::vector;

// forward declaration of the original InDOMSet function
namespace DOMSetFunctions {
  bool InDOMSet_orig(const OMKey& dom, const unsigned& domSet);
}

bool DOMSetFunctions::InDOMSet(const OMKey& dom, const unsigned& domSet,
                               const I3MapKeyVectorIntConstPtr &domSets)
{
  // use the original function in case no DOMSets could be found
  // in the frame
  if (!domSets) {
    log_debug("No domSet Map configured, using default values.");
    return DOMSetFunctions::InDOMSet_orig(dom,domSet);
  }
    
  I3MapKeyVectorInt::const_iterator it = domSets->find(dom);
  if (it == domSets->end()) {
    log_error("DOM(%i,%u) is not in DOMSet configured from frame!",
              dom.GetString(), dom.GetOM());
    return false; // this DOM will be in no DOMSet at all
  }

  // got a list of set IDs
  const std::vector<int> &sets = it->second;

  // each of these vectors will probably have a size of exactly 1,
  // so iterating shouldn't hurt too much
  BOOST_FOREACH(int setID, sets) {
    if (setID < 0) continue; // domSets are unsigned 
    if (static_cast<unsigned>(setID) == domSet) return true;
  }
    
  return false;
}


// function that returns a I3MapKeyVectorInt with the default
// DOMSets (as configured in InDOMSet_orig below)
I3MapKeyVectorIntPtr DOMSetFunctions::GetDefaultDOMSets()
{
  // stupidly iterate over all known DOMSet IDs and OMKeys
  // to fill the map. It's not efficient, but it needs to
  // be done only once. 
  // If someone wants to improve this: you are very welcome
  // to do it! :) -ck

  // allocate the output map
  I3MapKeyVectorIntPtr output(new I3MapKeyVectorInt());
    
  // all possible domSet ids
  for (int stringId=1;stringId<=86;++stringId) {
    for (unsigned domId=1;domId<=60;++domId) {
      const OMKey dom(stringId, domId);

      // create an empty map entry (there are DOMs
      // that are in no DOMSet at all [DeepCore DOMs 1-11])
      I3MapKeyVectorInt::iterator it = 
        output->insert(std::make_pair(dom, std::vector<int>())).first;
      std::vector<int> &sets = it->second;

      // now check all possible DOMSets
      BOOST_FOREACH(unsigned domSetId, DOMSETS)
      {
        // skip if not in DOMSet
        if (!DOMSetFunctions::InDOMSet_orig(dom, domSetId))
          continue;
          
        // found a DOM, add it to the output map!
        // (this either inserts a new, empty entry or
        // retrieves an iterator to an existing entry)

        // finally, add the domSet ID
        sets.push_back(domSetId);
      }
    }        
  }
    
  return output;
}



////////////////////////////////////////
///// this is the old InDOMSet code, retain it as a default setting:

// DeepCore will be strings 79-86
const int LOWEST_DEEPCORE_STRING(79);

const int DOMSET_2_HIGH_STRING(78); // InIce will be strings 1-78
const int DOMSET_2_LOW_STRING(1);
const unsigned DOMSET_2_HIGH_DOM(60);
const unsigned DOMSET_2_LOW_DOM(1);

const unsigned DOMSET_4_DC_HIGH_DOM(60);
const unsigned DOMSET_4_DC_LOW_DOM(11);
const unsigned DOMSET_4_II_HIGH_DOM(60);
const unsigned DOMSET_4_II_LOW_DOM(41);
const std::vector<int> DOMSET_4_II_STRINGS = list_of(26)(27)(35)(36)(37)(45)(46);

const unsigned DOMSET_5_DC_HIGH_DOM(60);
const unsigned DOMSET_5_DC_LOW_DOM(11);
const unsigned DOMSET_5_II_HIGH_DOM(60);
const unsigned DOMSET_5_II_LOW_DOM(39);
const std::vector<int> DOMSET_5_II_STRINGS = list_of(26)(27)(35)(36)(37)(45)(46);

const unsigned int DOMSET_6_DC_HIGH_DOM(60);
const unsigned int DOMSET_6_DC_LOW_DOM(11);
const unsigned int DOMSET_6_II_HIGH_DOM(60);
const unsigned int DOMSET_6_II_LOW_DOM(39);
const std::vector<int> DOMSET_6_II_STRINGS = list_of(25)(26)(27)(34)(35)(36)(37)(44)(45)(46)(47)(54);


bool DOMSetFunctions::InDOMSet_orig(const OMKey& dom, const unsigned& domSet){

  //DomSet 2: Inice
  // InIce (1,1)-(1,60):(80,1)-(80,60)
  // InIce (1,1)-(1,60):(78,1)-(78,60) - CHANGED 11/16/10 - gluesenkamp
  if(domSet == 2){
    return (dom.GetString() >= DOMSET_2_LOW_STRING &&
	    dom.GetString() <= DOMSET_2_HIGH_STRING &&
	    dom.GetOM() >= DOMSET_2_LOW_DOM &&
	    dom.GetOM() <= DOMSET_2_HIGH_DOM );

  }

  //DomSet 4: lower part of DeepCore strings plus lower part of 7 inice strings
  // Deepcore (81,11)-(81,60):(86,11)-(86,60)
  // Deepcore (79,11)-(79,60):(86,11)-(86,60)  - CHANGED 5/27/10 - olivas
  // InIce strings are 26,27,35,36,37,45,46 positions are 41-60
  if(domSet == 4){
    return ( ( dom.GetString() >= LOWEST_DEEPCORE_STRING &&
	       ( dom.GetOM() >= DOMSET_4_DC_LOW_DOM &&
		 dom.GetOM() <= DOMSET_4_DC_HIGH_DOM) )
	     ||
	     ( count(DOMSET_4_II_STRINGS.begin(),DOMSET_4_II_STRINGS.end(),dom.GetString()) &&
	       ( dom.GetOM() >= DOMSET_4_II_LOW_DOM &&
		 dom.GetOM() <= DOMSET_4_II_HIGH_DOM) ) );	     
  }
  
  //DomSet 5: lower part of DeepCore strings plus lower part of 7 inice strings
  // Deepcore (81,11)-(81,60):(86,11)-(86,60)
  // Deepcore (79,11)-(79,60):(86,11)-(86,60)  - CHANGED 5/27/10 - olivas
  // InIce strings are 26,27,35,36,37,45,46 positions are 39-60
  if(domSet == 5){
    return ( ( dom.GetString() >= LOWEST_DEEPCORE_STRING &&
	       ( dom.GetOM() >= DOMSET_5_DC_LOW_DOM &&
		 dom.GetOM() <= DOMSET_5_DC_HIGH_DOM) )
	     ||
	     ( count(DOMSET_5_II_STRINGS.begin(),DOMSET_5_II_STRINGS.end(),dom.GetString()) &&
	       ( dom.GetOM() >= DOMSET_5_II_LOW_DOM &&
		 dom.GetOM() <= DOMSET_5_II_HIGH_DOM) ) );	     
  }
  //DomSet 6: lower part of DeepCore strings plus lower part of 7 inice strings
  // Deepcore (81,11)-(81,60):(86,11)-(86,60)
  // Deepcore (79,11)-(79,60):(86,11)-(86,60)  - CHANGED 5/27/10 - olivas
  // InIce strings are 26,27,34,35,36,37,44,45,46,47,54 positions are 39-60
  
  if(domSet == 6){
    return ( ( dom.GetString() >= LOWEST_DEEPCORE_STRING &&
	       ( dom.GetOM() >= DOMSET_6_DC_LOW_DOM &&
		 dom.GetOM() <= DOMSET_6_DC_HIGH_DOM) )
	     ||
	     ( count(DOMSET_6_II_STRINGS.begin(),DOMSET_6_II_STRINGS.end(),dom.GetString()) &&
	       ( dom.GetOM() >= DOMSET_6_II_LOW_DOM &&
		 dom.GetOM() <= DOMSET_6_II_HIGH_DOM) ) );	     
  }
  return false;
}
