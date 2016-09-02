#include <I3Test.h>
#include "earthmodel-service/EarthModelService.h"
#include <iostream>

TEST_GROUP(EarthModelServiceTest);

using namespace earthmodel;
using namespace std;

TEST(earthmodel_test){

  EarthModelService service;

  int nbins = 100;
  double maxrad = service.GetIceAirBoundary();
  double minrad = 0;
  double dx = (maxrad - minrad) / nbins;

  service.SetEarthModel(vector<string>(1, "PREM_mmc"));
  service.SetIceCapSimpleAngle(30*I3Units::degree);

  for (int i = 0; i<nbins; ++i) {
    double r = minrad + i*dx;
    I3Position p(0, 0, r);    
    double density1 = service.GetEarthDensityInCGS(p);
    double density2 = service.GetPREM(r);
    ENSURE_DISTANCE(density1, density2, 1e-3);
  }

  // boundary check

  double rockice_boundary   = service.GetRockIceBoundary();
  double outercrust_density = service.GetEarthDensityInCGS(I3Position(0, 0, rockice_boundary - 1.0));
  double ice_density = service.GetEarthDensityInCGS(I3Position(0, 0, maxrad - 1.0));

  ENSURE_EQUAL(rockice_boundary, 6371324);
  ENSURE_EQUAL(outercrust_density, 2.650);
  ENSURE_EQUAL(maxrad, 6374134);
  ENSURE_EQUAL(ice_density, 0.762944);

  //spot check some points
  //the detector should be in the ice
  ENSURE_EQUAL(service.GetEarthParam(service.GetDetectorPosInEarthCoord()).fBoundaryName_,"clearice_boundary");
  //this point should also be in the ice
  I3Position p(0,0,6373900);
  ENSURE_EQUAL(service.GetEarthParam(p).fBoundaryName_,"clearice_boundary");
  //but this one, still at the same altitude but away from the pole should not
  p.RotateX(30*I3Units::degree);
  ENSURE_EQUAL(service.GetEarthParam(p).fBoundaryName_,"atmo_radius");
}

TEST(material_test){

  EarthModelService service;
  service.SetMaterialModel(vector<string>(1, "Standard"));

  std::cout << "*** material weights from file ***" << std::endl;
  EarthModelService::MatRatioMap matmap = service.GetMatRatioMap();
  EarthModelService::MatRatioMap::iterator iter1;
  for (iter1 = matmap.begin(); iter1 != matmap.end(); ++iter1) {
     EarthModelService::MediumType medtype = iter1->first;
     std::map<int, double> matw = iter1->second;
     std::map<int, double>::iterator iter2;
     std::cout << "MedType = " << service.GetMediumTypeString(medtype) << std::endl;
     for (iter2 = matw.begin(); iter2 != matw.end(); ++iter2) {
        std::cout << " +-  pdg = " << iter2->first
                  << " weight = " << iter2->second << std::endl;
     }
  }

  std::cout << "*** PNE ratio (calculated) ***" << std::endl;
  EarthModelService::MatRatioMap pnemap = service.GetPNERatioMap();
  for (iter1 = pnemap.begin(); iter1 != pnemap.end(); ++iter1) {
     EarthModelService::MediumType medtype = iter1->first;
     std::map<int, double> matw = iter1->second;
     std::map<int, double>::iterator iter2;
     std::cout << "MedType = " << service.GetMediumTypeString(medtype) << std::endl;
     for (iter2 = matw.begin(); iter2 != matw.end(); ++iter2) {
        std::cout << " +-  pdg = " << iter2->first
                  << " ratio = " << iter2->second << std::endl;
     }
  }
  
  ENSURE_EQUAL(matmap[EarthModelService::ICE][1000080160], 0.8881016);

  ENSURE_DISTANCE(pnemap[EarthModelService::ICE][11], 0.529412, 1e-6);

}

TEST(BoundaryDistance){
  EarthModelService prem;
  
  //compute the distance to the firn, goig straight up
  I3Position posCE=prem.GetEarthCoordPosFromDetCoordPos(I3Position(0,0,0));
  I3Direction dirCE=prem.GetEarthCoordDirFromDetCoordDir(I3Direction(0,0,1));
  double crossing = prem.DistanceToNextBoundaryCrossing(posCE,dirCE);
  //ensure that it is the same as what we expect for the PREM_mmc model
  ENSURE_DISTANCE(crossing,prem.GetDetectorDepth()-200,.01);
   
  //try the same thing, going sideways
  const EarthModelService::EarthParam& ice=prem.GetEarthParam(posCE);
  //how high is the detector above the center of the ice sphere?
  double h=(posCE-I3Position(0,0,ice.fZOffset_)).Magnitude();
  double r=ice.fUpperRadius_;
  //horizontal distance to the edge of the sphere is given by the pythagorean theorem:
  // r^2 = h^2 + d^2
  double d=sqrt(r*r - h*h);
  dirCE=prem.GetEarthCoordDirFromDetCoordDir(I3Direction(1,0,0));
  crossing = prem.DistanceToNextBoundaryCrossing(posCE,dirCE);
  ENSURE_DISTANCE(crossing,d,.01);

  //try going out of the atmosphere, and make sure that we're warned
  posCE=I3Position(0,0,6373900);
  posCE.RotateX(30*I3Units::degree);
  dirCE=prem.GetEarthCoordDirFromDetCoordDir(I3Direction(-1,0,0));
  bool willLeaveAtmosphere;
  prem.DistanceToNextBoundaryCrossing(posCE,dirCE,willLeaveAtmosphere);
  ENSURE(willLeaveAtmosphere);

  //test a sneaky case: the direction is out of the atmosphere, but the track
  //has skimmed the earth, so the point of entry to the rock is closer than
  //the exit point from the atmosphere, but must be ignored because it is behind
  //the current position
  posCE=I3Position(12294.4,-21132.6,6.37409e+06);
  dirCE=I3Direction(93.5977*I3Units::degree, 121.547*I3Units::degree);
  crossing=prem.DistanceToNextBoundaryCrossing(posCE,dirCE,willLeaveAtmosphere);
  ENSURE(willLeaveAtmosphere);
  ENSURE(crossing>806400);

  //another tricky case:
  //The simple IceCap model has the ice boundary intersect to bedrock;
  //it is important that traces through the bedrock should not stop at
  //the ficticious ice surface inside the rock
  posCE=I3Position(6371320*sin(21*I3Units::degree),0,6371320*cos(21*I3Units::degree));
  dirCE=I3Direction(21*I3Units::degree,0);
  crossing=prem.DistanceToNextBoundaryCrossing(posCE,dirCE);
  //the crossing should be at the distance to the inner crust
  ENSURE_DISTANCE(crossing,15320,.1);
}

TEST(TestColumnDepthCalculation){
  EarthModelService prem;
  //prem.SetIceCapTypeString("IceSheet");
  
  //Test that GetColumnDepthInCGS() and DistanceForColumnDepthToPoint()
  //actually work as inverse functions.
  //Test arbitrary directions distances (forming a logarithmic spiral from 1e7 m
  //through the Earth to 1000 m above the detector center)
  std::cout.precision(10);
  std::cout << std::endl;
  const double b=4*log(10.0)/M_PI;
  const double A=.1;
  const I3Position center(0,0,0);
  for(double zenith=M_PI; zenith>=0.0; zenith-=M_PI/100.0){
    std::cout << "zenith[deg]" << zenith / I3Units::degree << std::endl;
    I3Direction dir(zenith,0.0);
    double dist1=A*exp(b*(zenith+M_PI));
    std::cout << "Initial distance " << dist1 << std::endl;
    double depth=prem.GetColumnDepthInCGS(center,center-dist1*dir);
    std::cout << "Column depth " << depth << " g/cm^2" << std::endl;
    double dist2=prem.DistanceForColumnDepthToPoint(center,dir,depth);
    std::cout << "Final distance: " << dist2 << std::endl;
    
    std::cout << " difference: " << dist1-dist2 << std::endl;
    ENSURE_DISTANCE(dist2,dist1,1.0);
  }
  
  //Make sure that nothing goes wrong when we put one endpoint outside the atmosphere,
  //or request more column depth than exists
  {
    I3Position distant(0,0,prem.GetAtmoRadius());
    double depth=prem.GetColumnDepthInCGS(center,distant);
    double dist=prem.DistanceForColumnDepthFromPoint(center,I3Direction(distant-center),10*depth);
    ENSURE_DISTANCE(dist,prem.GetAtmoRadius()-prem.GetEarthCoordPosFromDetCoordPos(center).Magnitude(),1.0);
  }
}

TEST(LeptonRange_test){
  EarthModelService prem;
  
  { //test boundary clipping
    const I3Position center(0,0,0);
    const double en=100*I3Units::TeV;
    //try launching muons upward
    //this muon should have enough energy to exit the atmosphere
    double range=prem.GetLeptonRangeInMeterFrom(center,I3Direction(M_PI,0),en,false);
    ENSURE_DISTANCE(range,prem.GetAtmoRadius()-prem.GetIceAirBoundary()+prem.GetDetectorDepth(),0.1);
    
    //now test the same muons coming down
    range=prem.GetLeptonRangeInMeterTo(center,I3Direction(0,0),en,false);
    ENSURE_DISTANCE(range,prem.GetAtmoRadius()-prem.GetIceAirBoundary()+prem.GetDetectorDepth(),0.1);
  }
  
  //This would be the place to test the length clipping due to decay, but none of
  //the materials we use is low enough density for that code path to take effect
  //for even mildly relativistic particles 
}
