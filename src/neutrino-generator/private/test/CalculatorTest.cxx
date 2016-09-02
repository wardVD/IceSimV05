#include <I3Test.h>
#include "neutrino-generator/utils/Calculator.h"
#include "earthmodel-service/EarthModelService.h"
#include <boost/make_shared.hpp>
#include <iostream>
#include <icetray/I3Logging.h>

TEST_GROUP(NuGCalculator);

TEST(distance_to_next_boundary){

  GetIcetrayLogger()->SetLogLevel(I3LOG_WARN);

  std::vector<std::string> dummy;
  earthmodel::EarthModelServicePtr earth = 
     boost::make_shared<earthmodel::EarthModelService>();
  earth->SetIceCapTypeString("IceSheet");
  earth->Init();
  //earthmodel::EarthModelServicePtr earth = 
  //   boost::make_shared<earthmodel::EarthModelService>("earthmodel","",dummy,dummy,"IceSheet");

  // test if the function find a next boundary when starting point is exactly on a boundary
  // start from atmo_radius at NorthPole
  I3Position posCE = I3Position(0, 0, -6478000);
  //I3Position posCE = I3Position(0, 0, -6477000);
  I3Direction dirCE = earth->GetEarthCoordDirFromDetCoordDir(I3Direction(0,0,1));
  double startdensity = earth->GetEarthDensityInCGS(posCE);
  std::cerr << "start density " << startdensity << " at pos " << posCE << std::endl;
  double density = -1; // dummy

  std::map<double, double> densities;
  densities[posCE.GetZ()] = startdensity;
  int nlayers = 12;
  int ilayers = 0;
  while (density != startdensity) {
     // find next boundary (to iceair boundary)
     double crossing = nugen::Calculator::DistanceToNextBoundary(earth, posCE,dirCE);
     std::cerr << "dist to next " << crossing << std::endl;
     posCE += crossing * dirCE;
     density = earth->GetEarthDensityInCGS(posCE);
     std::cerr << std::setprecision(12) << "density " << density << " at pos " << posCE << std::endl;
     double z = posCE.GetZ();
     densities[z] = density; 
     if (ilayers > nlayers) {
        ENSURE(densities.count(-z));
        ENSURE_DISTANCE(density, densities[-z], 0.1, "density does not match with same radius.");
     }
     ilayers++;
  }
}
