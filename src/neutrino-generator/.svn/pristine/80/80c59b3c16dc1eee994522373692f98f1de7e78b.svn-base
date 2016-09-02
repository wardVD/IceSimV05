#ifndef NuGCalculator_H
#define NuGCalculator_H
/**
 *  copyright  (C) 2005
 *  the IceCube collaboration
 *  $Id: $
 *
 *  @version $Revision: $
 *  @date    $Date: $
 *  @author  Kotoyo Hoshina <hoshina@icecube.wisc.edu>
 *
 * @brief Calculator header file
 * utility functions
 * 
 */

#include "phys-services/I3RandomService.h"
#include "dataclasses/I3Position.h"
#include "dataclasses/I3Direction.h"
#include "earthmodel-service/EarthModelService.h"

namespace nugen {

I3_FORWARD_DECLARATION(Particle);

namespace Calculator {

  double CalcPowerLawEnergyFactor(double primaryE,
                                  double gamma,
                                  double eminLog,
                                  double emaxLog);

  double CalcSolidAngleFactor(double minzen,
                              double maxzen,
                              double minazi,
                              double maxazi);

  void CalcSimpleSlopeWeight(double alpha,
                             double min, double max, 
                             I3RandomServicePtr random,
                             double &yval, double &weight);

  /**
   * This is a wrapper function of 
   * EarthModelService::DistanceToNextBoundaryCrossing
   * The original function returns distance zero when
   * the position is exactly on the boundary, while
   * this wrapper function returns the next boundary.
   * Position and Direction must be in EarthCenter coordinate.
   */
  double DistanceToNextBoundary(earthmodel::EarthModelServiceConstPtr earth,
                             const I3Position &posCE,
                             const I3Direction &dirCE);



  /**
   * Rotate is copied from TVector3::Rotate
   * as we stopped to use RVector3
   * this is equivalent to pos.Rotate(angle, axis) 
   * in old TVector3 form.
   */
  I3Position Rotate(const I3Position &pos, 
                    double angle,
                    const I3Direction &axis);


  /**
   * RotateUz is copied from TVector3::RotateUz
   * as we stopped to use RVector3
   * this is equivalent to pos.RotateUz(dir) in old TVector3 form.
   */
  I3Position RotateUz(const I3Position &pos, 
                      const I3Direction &dir);

  /**
   * Angle is copied from TVector3::Angle
   * as we stopped to use RVector3
   * this is equivalent to pos.Angle(dir) in old TVector3 form.
   */
  double Angle(const I3Position &p, const I3Position &q);
  double Angle(const I3Position &p, const I3Direction &q);
  double Angle(const I3Direction &p, const I3Direction &q);



}
}

#endif 
