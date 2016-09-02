#ifndef GTSUTILS_H
#define GTSUTILS_H
/**
 * class: GTSUtils
 *
 * Version $Id: $
 *
 * date: $Date: $
 *
 * @author olivas
 *
 * (c) 2006 IceCube Collaboration
 */

#include "dataclasses/status/I3TriggerStatus.h"
#include "dataclasses/TriggerKey.h"
#include "dataclasses/physics/I3TriggerHierarchy.h"
#include <vector>

/**
 * @brief Namespace that holds helper functions that the 
 *        I3GlobalTriggerSim module uses.
 */
namespace GTSUtils 
{
			    
  bool LessThan(I3Trigger i, I3Trigger j);

  I3TriggerStatus::Subdetector KeyToSubDetector(const TriggerKey& k);

  void TriggerPrettyPrint(const I3Trigger& , std::stringstream&);

  void DumpChildren(const I3TriggerHierarchy& , 
                    I3TriggerHierarchy::iterator,
                    std::stringstream&);

  std::string Stringize(const I3TriggerHierarchy& );

  std::string Stringize(I3TriggerHierarchyPtr );

};

#endif //GLOBALTRIGGERSIMUTILS_H
