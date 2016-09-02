#include <I3Test.h>

#include <topsimulator/GeoFunctions.h>
#include <dataclasses/physics/I3Particle.h>
#include <dataclasses/I3Position.h>
#include <dataclasses/I3Direction.h>
#include <dataclasses/I3Constants.h>
#include <icetray/I3Units.h>
#include <phys-services/I3GSLRandomService.h>

#include <iostream>

using namespace std;

TEST_GROUP(INTERSECT_CYLINDER);

void RandDirIsotropic(I3Particle& particle)
{
    // This function generates an isotropic distribution
    
    static I3GSLRandomService rand(0);
    double theta = acos(rand.Uniform(-1.0,1.0));
    double phi   = I3Constants::pi*rand.Uniform(2.0);
    
    particle.SetThetaPhi(theta, phi);
}

void RandPos(I3Particle& particle, double tankHeight, double tankRadius)
{
    static I3GSLRandomService rand(0);
    
    double x = rand.Uniform(-1.5*tankRadius, 1.5*tankRadius);
    double y = rand.Uniform(-1.5*tankRadius, 1.5*tankRadius);
    double z = rand.Uniform(-tankHeight, tankHeight);
    
    particle.SetPos(x,y,z);
}

void TestPosition(const I3Position& pos, double tankHeight, double tankRadius, double tolerance)
{
    double x = pos.GetX();
    double y = pos.GetY();
    double z = pos.GetZ();
    
    if(fabs(z)<(0.5*tankHeight - tolerance))
    {
	ENSURE_DISTANCE(sqrt(x*x + y*y), tankRadius, tolerance, "x,y coordinates not on cylinder border.");
    }
    else
    {
	ENSURE_DISTANCE(fabs(z), 0.5*tankHeight, tolerance, "z coordinate not on tank top/bottom.");
	ENSURE_DISTANCE(sqrt(x*x + y*y), 0.5*tankRadius, 0.5*tankRadius + tolerance, "x,y coordinates outside of cylinder radius.");
    }
}

void TestTrack(int i , const I3Particle& particle, double tankHeight, double tankRadius, double tolerance)
{
    cout << "Track " << i << ":"
	 <<  " x=" << particle.GetX()/I3Units::m << " m"
	 << ", y=" << particle.GetY()/I3Units::m << " m"
	 << ", z=" << particle.GetZ()/I3Units::m << " m"
	 << ", theta=" << particle.GetDir().CalcTheta()/I3Units::deg << " deg"
	 << ", phi=" << particle.GetDir().CalcPhi()/I3Units::deg << " deg";
	
    I3Particle track_out;
    if(GeoFunctions::IntersectCylinder(0, 0, 0, tankHeight, tankRadius, particle, track_out))
    {
        TestPosition(track_out.GetStartPos(), tankHeight, tankRadius, tolerance);
	TestPosition(track_out.GetStopPos(),  tankHeight, tankRadius, tolerance);
	cout << " --> HIT TANK" << endl;
    }
    else
    {
	cout << " --> MISSED TANK" << endl;
    }
}


TEST(intersect_cylinder)
{
  const double tankRadius = 0.93*I3Units::m;
  const double tankHeight = 0.90*I3Units::m;
  
  const double tolerance = 0.001*I3Units::mm;
  
  // Random tracks
  int num_tracks = 100000;
  I3Particle track;
  for(int i=0; i<num_tracks; i++)
  {
    RandPos(track, tankHeight, tankRadius);
    RandDirIsotropic(track);
    TestTrack(i, track, tankHeight, tankRadius, tolerance);
  }
    
  // Vertical tracks
  num_tracks = 10000;
  double nz = 1.0;
  for(int i=0; i<num_tracks; i++)
  {
    if(nz>0) nz=-1.0;
    else     nz= 1.0;
    
    RandPos(track, tankHeight, tankRadius);
    track.SetDir(0,0,nz);
    
    TestTrack(i, track, tankHeight, tankRadius, tolerance);
  }

  // Horizontal tracks
  static I3GSLRandomService rand(0);    
  for(int i=0; i<num_tracks; i++)
  {
    RandPos(track, tankHeight, tankRadius);
    
    double phi = I3Constants::pi*rand.Uniform(2.0);
    track.SetDir(cos(phi),sin(phi),0);
    
    TestTrack(i, track, tankHeight, tankRadius, tolerance);
  }
}
