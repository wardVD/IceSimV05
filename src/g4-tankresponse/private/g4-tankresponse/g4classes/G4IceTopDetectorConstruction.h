/**
 * Copyright (C) 2009
 * The IceCube collaboration
 * ID: $Id: G4IceTopDetectorConstruction.h 69401 2010-12-13 12:18:09Z tilo $
 *
 * @file G4IceTopDetectorConstruction.h
 * @version $Rev: 69401 $
 * @date $Date: 2010-12-13 06:18:09 -0600 (Mon, 13 Dec 2010) $
 * @author Tilo Waldenmaier, Thomas Melzig
 */


#ifndef _TOPSIMULATOR_G4ICETOPDETECTORCONSTRUCTION_H_
#define _TOPSIMULATOR_G4ICETOPDETECTORCONSTRUCTION_H_

#include <G4VUserDetectorConstruction.hh>
#include <G4ThreeVector.hh>

class G4IceTopTank;

class G4IceTopDetectorConstruction: public G4VUserDetectorConstruction
{
 public:
  G4IceTopDetectorConstruction();
  
  ~G4IceTopDetectorConstruction();
  
  G4VPhysicalVolume* Construct();
  
  void SetVerboseLevel(G4int level) {verboseLevel_=level;};

  void InstallTank(G4IceTopTank* tank) {tankList_.push_back(tank);};
  
  const G4ThreeVector& GetWorldOrigin() const {return origin_;};
  
 private:
  
  void CreateMaterials();
  
  void CreateAir();
  void CreateIce();
  void CreateSnow();
  void CreateWater();
  void CreatePlastic();
  void CreateTyvek();
  void CreatePerlite();
  void CreateGlassSphere();
  void CreateEffectiveDOMMaterial();
  
  G4ThreeVector origin_;
  
  G4int verboseLevel_;
  
  std::vector<G4IceTopTank*> tankList_;
};

#endif
