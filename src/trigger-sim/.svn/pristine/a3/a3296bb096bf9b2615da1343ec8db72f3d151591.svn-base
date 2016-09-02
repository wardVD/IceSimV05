#ifndef GLOBALTRIGGERSIM_H
#define GLOBALTRIGGERSIM_H
/**
 * class: GlobalTriggerSim
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
#include "dataclasses/status/I3DetectorStatus.h"
#include "dataclasses/TriggerKey.h"
#include "dataclasses/physics/I3TriggerHierarchy.h"
#include <vector>
#include "trigger-sim/utilities/ReadoutWindowUtil.h"

typedef std::vector< std::pair< I3Trigger, I3Trigger> > I3TriggerPairVector;
typedef std::pair< I3Trigger, I3Trigger> I3TriggerPair;

class GlobalTriggerSim 
{

 private:
    boost::shared_ptr< std::map<TriggerKey, I3TriggerStatus> > triggerStatusMap_;

    boost::shared_ptr<ReadoutWindowUtil> roUtil_;

    I3TriggerReadoutConfig defaultReadoutConfig_;

    GlobalTriggerSim();

  SET_LOGGER("GlobalTriggerSim");

 public:    

    GlobalTriggerSim(const I3DetectorStatus& d){
      triggerStatusMap_ = boost::shared_ptr< std::map<TriggerKey, I3TriggerStatus> >
	( new std::map<TriggerKey, I3TriggerStatus>(d.triggerStatus) );

      roUtil_ = boost::shared_ptr< ReadoutWindowUtil >
	( new ReadoutWindowUtil(d) );
    };

    GlobalTriggerSim(I3DetectorStatusConstPtr d){
      triggerStatusMap_ = boost::shared_ptr< std::map<TriggerKey, I3TriggerStatus> >
	( new std::map<TriggerKey, I3TriggerStatus>(d->triggerStatus) );

      roUtil_ = boost::shared_ptr< ReadoutWindowUtil >
	( new ReadoutWindowUtil(*d) );
    };

    void SetDefaultReadoutConfig(I3TriggerReadoutConfig r){ 
      defaultReadoutConfig_ = r;
    };

    // need non-const so we can sort
    void InsertThroughputTriggers(std::vector<I3Trigger>& src,
				  I3TriggerPairVector& dest);
    
    I3TriggerHierarchyPtr Merge( const std::vector< std::pair<I3Trigger, I3Trigger > >& );

    I3TriggerHierarchy::iterator FindOverlap(I3TriggerHierarchyConstPtr, const I3Trigger&);

};

#endif //GLOBALTRIGGERSIM_H
