/**
 * copyright  (C) 2010
 * the icecube collaboration
 * $Id: 
 *
 * @file CylinderTriggerAlgorithm.h
 * @version
 * @date
 * @author danninger
 */

#ifndef CYLINDER_TRIGGER_ALGORITHM_H
#define CYLINDER_TRIGGER_ALGORITHM_H

#include "icetray/I3Logging.h"
#include "trigger-sim/algorithms/TriggerHit.h"
#include <dataclasses/geometry/I3Geometry.h>

class CylinderTriggerAlgorithm
{

 public:
  CylinderTriggerAlgorithm(double triggerWindow, unsigned int triggerThreshold, unsigned int simpleMultiplicity,
			  I3GeometryConstPtr Geometry, double Radius , double Zdistance);
  ~CylinderTriggerAlgorithm();

  void AddHits(TriggerHitVectorPtr hits);

  unsigned int GetNumberOfTriggers();
  TriggerHitVectorPtr GetNextTrigger();

 private:

  double triggerWindow_;
  unsigned int triggerThreshold_;
  unsigned int simpleMultiplicity_;
  double Radius_; 
  double Zdistance_; 
  I3GeometryConstPtr Geometry_;
  
  TriggerHitList hitQueue_;
  
  TriggerHitVectorVector triggers_;
  unsigned int triggerCount_;

  bool PosWindow();


  SET_LOGGER("CylinderTriggerAlgorithm");
};

#endif
