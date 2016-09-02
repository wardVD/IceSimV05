/**
 * copyright  (C) 2004
 * the icecube collaboration
 *  $Id: DOMSetFunctions.h 2009/08/04 21: olivas Exp $
 *
 * @file DOMSetFunctions.h
 * @version $Revision: 1.1.1.1 $
 * @date $Date: 2004/11/06 21:00:46 $
 * @author olivas
 */

#ifndef DOMSETFUNCTIONS_H
#define DOMSETFUNCTIONS_H

#include <icetray/OMKey.h>
#include <dataclasses/I3Map.h>
#include <boost/assign/list_of.hpp>

/**
 * @brief Contains functions useful for handling DOMsets.
 */
namespace DOMSetFunctions{

  // We only have information on the following DOM sets.
  // Don't know what came of  1 and 3, but they're not used.
  const std::vector<unsigned> DOMSETS = boost::assign::list_of(2)(4)(5)(6);

  bool InDOMSet(const OMKey& dom, const unsigned& domSet,
                const I3MapKeyVectorIntConstPtr &domSets);
    
  I3MapKeyVectorIntPtr GetDefaultDOMSets();

}; 

#endif //DOMSETFUNCTIONS_H
