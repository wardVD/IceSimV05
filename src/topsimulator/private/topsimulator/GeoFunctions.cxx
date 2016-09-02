/**
 * Copyright (C) 2010
 * The IceCube collaboration
 * ID: $Id: $
 *
 * @file GeoFunctions.cxx
 * @version $Rev: $
 * @date $Date: $
 * @author Tilo Waldenmaier
 */


#include <topsimulator/GeoFunctions.h>

namespace GeoFunctions
{
  bool IntersectCylinder(const I3Position& center, const double& height, const double& radius, const I3Particle& particle, I3Particle& contTrack)
  {
    return IntersectCylinder(center.GetX(), center.GetY(), center.GetZ(), height, radius, particle, contTrack);
  }
  
  bool IntersectCylinder(const I3Position& center, const double& height, const double& radius, const I3Particle& particle)
  {
    return IntersectCylinder(center.GetX(), center.GetY(), center.GetZ(), height, radius, particle);
  }

  bool IntersectCylinder(const double& xc, const double& yc, const double& zc, const double& height, const double& radius, const I3Particle& particle)
  {
    I3Particle contTrack;
    return IntersectCylinder(xc, yc, zc, height, radius, particle, contTrack);
  }
  
  bool IntersectCylinder(const double& xc, const double& yc, const double& zc, const double& height, const double& radius, const I3Particle& particle, I3Particle& contTrack)
  {
    double zTop    = 0.5*height;
    double zBottom = -0.5*height;
    
    // Relative coordinates with respect to center
    double rx = particle.GetX() - xc;
    double ry = particle.GetY() - yc;
    double rz = particle.GetZ() - zc;
    
    // Particle velocity
    double vx = particle.GetSpeed()*particle.GetDir().GetX();
    double vy = particle.GetSpeed()*particle.GetDir().GetY();
    double vz = particle.GetSpeed()*particle.GetDir().GetZ();
    
    
    double t1, z1;
    double t2, z2;  
    if((vx*vx + vy*vy)>0)
    {
      // **************
      // Inclined track
      // **************
    
      // Calculate the square-root argument
      double arg = (vx*vx + vy*vy)*radius*radius - (vx*ry - vy*rx)*(vx*ry - vy*rx);
      
      // If this condition is satisfied the tank won't be hit!
      if(arg <= 0) 
      {
        return false;
      }

      // Times of cylinder intersections (t1 < t2)
      t1 = (-(rx*vx + ry*vy) - sqrt(arg))/(vx*vx + vy*vy);
      t2 = (-(rx*vx + ry*vy) + sqrt(arg))/(vx*vx + vy*vy);
      
      // z-coordinates of cylinder intersections
      z1 = rz + vz*t1;
      z2 = rz + vz*t2;
      
      // If the following conditions are satisfied the tank won't be hit!
      // Means: If both intersections with the cylinder are above or below
      // the top or bottom of the tank, the tank won't be hit 
      if(z1 < zBottom && z2 < zBottom) 
      {
        return false;
      }
      if(z1 > zTop    && z2 > zTop)    
      {
        return false;
      }

      // Incoming track
      if(z1 > zTop)
      {
        z1 = zTop;
        t1 = (z1 - rz)/vz;
      }

      if(z1 < zBottom)
      {
        z1 = zBottom;
        t1 = (z1 - rz)/vz;
      }
        
      // Outgoing track
      if(z2 > zTop)
      {
        z2 = zTop;
        t2 = (z2 - rz)/vz;
      }
        
      if(z2 < zBottom)
      {
        z2 = zBottom;
        t2 = (z2 - rz)/vz;
      }
    }
    else
    {
      // **************
      // Vertical track
      // **************
    
      // If this condition is satisfied tank won't be hit!
      if(sqrt(rx*rx + ry*ry) >= radius) 
      {
        return false;
      }
      
      // Times of top and bottom intersections (t1 < t2)
      if(vz>0)
      {
        // Upward going
        t1 = (zBottom - rz)/vz;
        t2 = (zTop - rz)/vz;
      }
      else
      {
        // Downward going
        t1 = (zTop - rz)/vz;
        t2 = (zBottom - rz)/vz;
      }
        
      // Calculate the z-coordinates
      z1 = rz + vz*t1;
      z2 = rz + vz*t2;
    }
    
    // Calculate the missing intersection coordinates
    double x1 = rx + t1*vx;
    double y1 = ry + t1*vy;
    
    double x2 = rx + t2*vx;
    double y2 = ry + t2*vy;
    
    double trackLength = sqrt((x2-x1)*(x2-x1) + (y2-y1)*(y2-y1) + (z2-z1)*(z2-z1)); 
    
    log_trace("Particle enters cylinder at (x=%2f m, y=%.2f m, z=%.2f m, t=%.2f ns)",
              x1/I3Units::m, y1/I3Units::m, z1/I3Units::m, t1/I3Units::ns);
    log_trace("Particle leaves cylinder at (x=%2f m, y=%.2f m, z=%.2f m, t=%.2f ns)",
              x2/I3Units::m, y2/I3Units::m, z2/I3Units::m, t2/I3Units::ns);
    
    // Fill the contained track
    contTrack.SetType(particle.GetType());
    contTrack.SetShape(I3Particle::ContainedTrack); // This is needed to be able to use the GetStartPos(), GetStopPos() methods ..  
    contTrack.SetPos(x1,y1,z1);
    contTrack.SetDir(particle.GetDir().GetX(), particle.GetDir().GetY(), particle.GetDir().GetZ());
    contTrack.SetTime(particle.GetTime() + t1); // Time when particle enters the tank
    contTrack.SetLength(trackLength);
    contTrack.SetEnergy(particle.GetEnergy());
    contTrack.SetSpeed(particle.GetSpeed());
    
    return true;
  }
}
