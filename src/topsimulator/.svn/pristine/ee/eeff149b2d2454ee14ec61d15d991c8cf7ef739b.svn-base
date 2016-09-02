/**
 * Copyright (C) 2010
 * The IceCube collaboration
 * ID: $Id: $
 *
 * @file GeoFunctions.h
 * @version $Rev: $
 * @date $Date: $
 * @author Tilo Waldenmaier
 */

#ifndef _TOPSIMULATOR_GEOFUNCTIONS_H_
#define _TOPSIMULATOR_GEOFUNCTIONS_H_


#include <dataclasses/I3Position.h>
#include <dataclasses/physics/I3Particle.h>

/** \brief These functions determine whether the path of an I3Particle
  *        intersects with a cylinder.
  *
  * They return a boolean to indicate an intersection, and
  * some overloads fill a passed reference with the contained track.
  */
namespace GeoFunctions
{

  bool IntersectCylinder(const double& xc, const double& yc, const double& zc, const double& height, const double& radius, const I3Particle& particle, I3Particle& contTrack);
  
  bool IntersectCylinder(const double& xc, const double& yc, const double& zc, const double& height, const double& radius, const I3Particle& particle);
  
  bool IntersectCylinder(const I3Position& center, const double& height, const double& radius, const I3Particle& particle, I3Particle& contTrack);
  
  bool IntersectCylinder(const I3Position& center, const double& height, const double& radius, const I3Particle& particle);
  
}

#endif
