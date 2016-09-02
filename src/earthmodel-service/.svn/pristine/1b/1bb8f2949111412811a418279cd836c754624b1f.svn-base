#include <I3Test.h>
#include "earthmodel-service/EarthModelCalculator.h"
#include "earthmodel-service/EarthModelService.h"
#include <map>

TEST_GROUP(EarthModelCalculatorTest);

using namespace earthmodel;

TEST(GetInteractionsWithSphere_test){

  I3Position pos(3, 0, 0);
  I3Direction dir(0, 1, 0);
  I3Position start, end;
  double r = 5;

  int result =  EarthModelCalculator::GetIntersectionsWithSphere(
                                                   pos, dir,
                                                   r,
                                                   start, end);
  fprintf(stderr,"start(%f,%f,%f), end(%f,%f,%f)\n", 
          start.GetX(), start.GetY(), start.GetZ(), end.GetX(), end.GetY(), end.GetZ());
  ENSURE_DISTANCE(start.GetX(),  3.0, 1e-12);
  ENSURE_DISTANCE(start.GetY(), -4.0, 1e-12);
  ENSURE_DISTANCE(start.GetZ(), 0, 1e-12);
  ENSURE_DISTANCE(end.GetX(),  3.0, 1e-12);
  ENSURE_DISTANCE(end.GetY(), 4.0, 1e-12);
  ENSURE_DISTANCE(end.GetZ(), 0, 1e-12);

  dir = I3Direction(1,1,0);
  result = EarthModelCalculator::GetIntersectionsWithSphere(
                                                   pos, dir,
                                                   r,
                                                   start, end);
  std::cerr << "result = " << result << std::endl;
  ENSURE(result == 2);

}

TEST(GetDistsToIntersectionsWithSphere_test){
  double enterd, exitd;
  const double r=100;
  int isect;
  
  isect=EarthModelCalculator::GetDistsToIntersectionsWithSphere(
    I3Position(0,0,0),
    I3Direction(0,0),
    r,enterd,exitd);
  ENSURE_EQUAL(isect,2);
  ENSURE_DISTANCE(enterd,-100,1e-6);
  ENSURE_DISTANCE(exitd,100,1e-6);
  
  //regardless of position and direction, if the point is inside
  //there should be two intersections, the entry distance should be negative,
  //and the exit distance should be positive
  for(double u=0; u<=10.0; u+=0.5){
    isect=EarthModelCalculator::GetDistsToIntersectionsWithSphere(
      I3Position(u,.5*u*u,.02*u*u*u),
      I3Direction(u/4,u/2),
      r,enterd,exitd);
    ENSURE_EQUAL(isect,2);
    ENSURE(enterd<0);
    ENSURE(exitd>0);
  }
  
  //test some points outside for a direction going toward the sphere;
  //both distances should be positive
  for(double u=0; u<=10.0; u+=0.5){
    isect=EarthModelCalculator::GetDistsToIntersectionsWithSphere(
      I3Position(150,4*u,8*u),
      I3Direction(M_PI/2,0),
      r,enterd,exitd);
    ENSURE_EQUAL(isect,2);
    ENSURE(enterd>0);
    ENSURE(exitd>0);
    ENSURE(exitd>enterd);
  }
  
  //test some points outside for a direction going away from the sphere;
  //both distances should be negative
  for(double u=0; u<=10.0; u+=0.5){
    isect=EarthModelCalculator::GetDistsToIntersectionsWithSphere(
      I3Position(-150,4*u,8*u),
      I3Direction(M_PI/2,0),
      r,enterd,exitd);
    ENSURE_EQUAL(isect,2);
    ENSURE(enterd<0);
    ENSURE(exitd<0);
    ENSURE(exitd>enterd);
  }
  
  //This would be a good thing to check, but roundoff destroys any certainty of
  //getting a single intersection point
  /*//test when there is only one intersection, and it is along the specified direction
  isect=EarthModelCalculator::GetDistsToIntersectionsWithSphere(
    I3Position(100,100,0),
    I3Direction(M_PI/2,0),
    r,enterd,exitd);
  std::cout << enterd << ' ' << exitd << ' ' << exitd-enterd << std::endl;
  ENSURE_EQUAL(isect,1);
  ENSURE_DISTANCE(enterd,100,1e-6);
  ENSURE_DISTANCE(exitd,100,1e-6);*/
}

TEST(MuonRange_test){

  double energy = 1e3; // 1TeV
  double range1 = EarthModelCalculator::GetLeptonRange(energy);
  //fprintf(stderr,"muon range for %f GeV muon is %f [m.w.e]\n", energy, range1);

  double range2 = EarthModelCalculator::GetLeptonRange(energy,
                                          false, EarthModelCalculator::DEFAULT, 1.0);
  ENSURE(range1 == range2);

}

struct mSin{
   mSin(){}
   double operator()(double x) const{ return(sin(x)); }
private:
   mSin(const mSin&); //disallow copying
};

TEST(Integration_test){
   //integrating should work both with function pointers and function objects
   //(and function objects should not be copied)
   ENSURE_DISTANCE(Integration::rombergIntegrate(&sin,0,M_PI,1e-6),2,1e-6);
   mSin mySin;
   ENSURE_DISTANCE(Integration::rombergIntegrate(mySin,0,M_PI,1e-6),2,1e-6);
}
