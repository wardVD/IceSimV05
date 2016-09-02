/**
 * Copyright (C) 2009
 * The IceCube collaboration
 * ID: $Id: G4IceTopTank.h 141217 2016-01-25 17:56:59Z kjmeagher $
 *
 * @file G4IceTopTank.h
 * @version $Rev: 141217 $
 * @date $Date: 2016-01-25 11:56:59 -0600 (Mon, 25 Jan 2016) $
 * @author Tilo Waldenmaier, Thomas Melzig
 */


#ifndef _TOPSIMULATOR_G4ICETOPTANK_H_
#define _TOPSIMULATOR_G4ICETOPTANK_H_

class OMKey;
struct TankKey;
class I3Geometry;
class I3Position;

class G4LogicalVolume;
class G4VPhysicalVolume;
class G4TankIceSD;

#include <G4Types.hh>
#include <G4ThreeVector.hh>
#include <map>

#include <icetray/I3Logging.h>


class G4IceTopTank
{
 public:
  G4IceTopTank(const TankKey& tankKey, const I3Geometry& geometry);
  
  ~G4IceTopTank();
  
  const G4ThreeVector& GetPos() const {return position_;};
  
  I3Position GetPos_I3();
  double GetX_I3();
  double GetY_I3();
  double GetZ_I3();
  
  // Delaunay points around tank
  const G4ThreeVector& GetDelaunayPoint1() const {return delaunayPoint1_;};
  const G4ThreeVector& GetDelaunayPoint2() const {return delaunayPoint2_;};
  const G4ThreeVector& GetDelaunayPoint3() const {return delaunayPoint3_;};

  
  G4double GetTankHeight_G4() {return tankHeight_;};
  G4double GetTankRadius_G4() {return outerRadius_;};
  G4double GetSnowHeight_G4() {return snowHeight_;};
  
  double   GetTankHeight_I3();
  double   GetTankRadius_I3();
  double   GetSnowHeight_I3();
  
  G4double GetEDep_G4(const OMKey& omKey);
  G4double GetTime_G4(const OMKey& omKey);
  
  double   GetEDep_I3(const OMKey& omKey);
  double   GetTime_I3(const OMKey& omKey);
  
  double GetNumCherenkov(const OMKey& omKey);
  double GetNumCherenkovWeight(const OMKey& omKey);
  
  const TankKey& GetTankKey() const {return tankKey_;};
  
  G4VPhysicalVolume* InstallTank(G4VPhysicalVolume* mother, const G4ThreeVector& origin);
  
 private:
  
  G4double tankThickness_;
  G4double tankHeight_;
  G4double fillHeight_;
  G4double innerRadius_;
  G4double outerRadius_;
  
  G4double snowHeight_;
  G4double perliteHeight_;
  
  G4double glassOuterRadius_;
  G4double glassThickness_;
  
  G4ThreeVector position_;
  
  G4ThreeVector delaunayPoint1_;
  G4ThreeVector delaunayPoint2_;
  G4ThreeVector delaunayPoint3_;
  
  std::map<OMKey, G4ThreeVector> relDomPositions_;
  
  G4LogicalVolume* tankLog_;
  G4TankIceSD* iceSD_;
  
  const TankKey& tankKey_;
  const I3Geometry& geometry_;
  
  SET_LOGGER("G4IceTopTank");
};

#endif
