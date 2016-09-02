/**
 * Copyright (C) 2009
 * The IceCube collaboration
 * ID: $Id: G4IceTopDetectorConstruction.cxx 73871 2011-04-11 15:08:54Z thomas.melzig $
 *
 * @file G4IceTopDetectorConstruction.cxx
 * @version $Rev: 73871 $
 * @date $Date: 2011-04-11 10:08:54 -0500 (Mon, 11 Apr 2011) $
 * @author Tilo Waldenmaier, Thomas Melzig
 */


#include <g4-tankresponse/g4classes/G4IceTopDetectorConstruction.h>
#include <g4-tankresponse/g4classes/G4IceTopTank.h>

#include <G4NistManager.hh>
#include <G4Material.hh>

#include <G4LogicalVolume.hh>
#include <G4PVPlacement.hh>

#include <G4Box.hh>

#include <g4-tankresponse/g4classes/G4Delaunay.h>
#include <G4TriangularFacet.hh>
#include <G4QuadrangularFacet.hh>
#include <G4TessellatedSolid.hh>

#include <G4VisAttributes.hh>
#include <G4UserLimits.hh>

#include <boost/foreach.hpp>


G4IceTopDetectorConstruction::G4IceTopDetectorConstruction():
  origin_(NAN, NAN, NAN), verboseLevel_(0), tankList_(0)
{
  
}


G4IceTopDetectorConstruction::~G4IceTopDetectorConstruction()
{
  
}


G4VPhysicalVolume* G4IceTopDetectorConstruction::Construct()
{
  if(tankList_.empty()) return NULL;
  
  CreateMaterials();
  
  // Determine bottom z position of snow volume
  G4double zSnowBottom(tankList_.at(0)->GetPos().z());
  BOOST_FOREACH(G4IceTopTank* tank, tankList_)
  {
    // z position of bottom of tank
    G4double z = tank->GetPos().z() - 0.5*tank->GetTankHeight_G4();
    zSnowBottom = std::min(z, zSnowBottom);
  }
  
  // Subtract safety margin
  zSnowBottom -= 1.0*CLHEP::m;
  
  // Triangulate snow surface
  G4Delaunay delaunay;
  BOOST_FOREACH(G4IceTopTank* tank, tankList_)
  {
    // z position of snow surface
    G4double z = tank->GetPos().z() + 0.5 * tank->GetTankHeight_G4() + tank->GetSnowHeight_G4();
    
    delaunay.AddPoint(tank->GetDelaunayPoint1().x(),
                      tank->GetDelaunayPoint1().y(),
                      z - zSnowBottom);
    delaunay.AddPoint(tank->GetDelaunayPoint2().x(),
                      tank->GetDelaunayPoint2().y(),
                      z - zSnowBottom);
    delaunay.AddPoint(tank->GetDelaunayPoint3().x(),
                      tank->GetDelaunayPoint3().y(),
                      z - zSnowBottom);
  }
  
  // Create tesselated snow volume
  G4TessellatedSolid* solidSnow = new G4TessellatedSolid("solid_snow");
  delaunay.BuildSolid(solidSnow, 50.0*CLHEP::m, 100.0*CLHEP::m);
  
  // Determine World dimensions
  G4double xHalfLength = 0.5 * (delaunay.GetXmax() - delaunay.GetXmin());
  G4double yHalfLength = 0.5 * (delaunay.GetYmax() - delaunay.GetYmin());
  G4double zHalfLength = 0.5 * (delaunay.GetZmax() + 20.0 * CLHEP::m); // 20 m of atmosphere
  
  // World origin in IceCube coordinates
  origin_.set(delaunay.GetOrigin().x(), delaunay.GetOrigin().y(), zSnowBottom + zHalfLength);
  
  // Create world volume
  G4Box* world_box = new G4Box("solid_world", xHalfLength, yHalfLength, zHalfLength);
  G4LogicalVolume* worldLog =
      new G4LogicalVolume(world_box, G4Material::GetMaterial("Air"), "log_world", 0, 0, 0);
  G4VPhysicalVolume* worldPhys =
      new G4PVPlacement(0, G4ThreeVector(), worldLog, "world", 0, false, 0);
  
  // Snow layer
  G4LogicalVolume* snowLog =
      new G4LogicalVolume(solidSnow, G4Material::GetMaterial("Snow"), "log_snow", 0, 0, 0);
  G4VPhysicalVolume* snowPhys =
      new G4PVPlacement(0, G4ThreeVector(0, 0, -zHalfLength), snowLog, "snow", worldLog, false, 0);
  
  // Instantiation of a set of visualization attributes with cyan colour
  G4VisAttributes * snowVisAtt = new G4VisAttributes(G4Colour(0., 1., 1.));
  // Assignment of the visualization attributes to the logical volume
  snowLog->SetVisAttributes(snowVisAtt);
  
  // Install tanks
  BOOST_FOREACH(G4IceTopTank* tank, tankList_)
  {
    tank->InstallTank(snowPhys, origin_);
  }
  
  // User limits (energy cutoffs)
  // Do not create photons or electrons below cherenkov threshold
  // See also corresponding UserSpecialCuts in Physicslist !!!!
  G4UserLimits* energyLimit = new G4UserLimits();
  energyLimit->SetUserMinEkine(280.0 * CLHEP::keV);  // Cherenkov threshold of electrons in ice
  worldLog->SetUserLimits(energyLimit);
  snowLog->SetUserLimits(energyLimit);
  
  return worldPhys;
}

/*****************************************************************/

void G4IceTopDetectorConstruction::CreateMaterials()
{
  CreateAir();
  CreateIce();
  CreateSnow();
  CreateWater();
  CreatePlastic();
  CreatePerlite();
  CreateGlassSphere();
  CreateEffectiveDOMMaterial();
  
  //if(verboseLevel_>0) G4cout << *G4Material::GetMaterialTable() << G4endl;
}

/*****************************************************************/

void G4IceTopDetectorConstruction::CreateAir()
{
  G4NistManager* nistManager = G4NistManager::Instance();
  nistManager->ConstructNewGasMaterial("Air","G4_AIR", (273.15 - 40.0)*CLHEP::kelvin, 670.0E-3*CLHEP::bar);
}

/*****************************************************************/

void G4IceTopDetectorConstruction::CreateSnow()
{
  G4NistManager* nistManager = G4NistManager::Instance();  
  
  //
  // The snow density at Southpole has been measured in January 2010.
  // The density of the compacted snow around the tanks is 0.4 g/cm3
  // and changes, with increasing depths, to 0.35 g/cm3 which is the
  // density of untouched natural snow. An "average" value of 0.38 g/cm3
  // is taken here.
  //
  G4Material* snow = new G4Material("Snow", 0.38 * CLHEP::g / CLHEP::cm3, 2, kStateSolid);
  snow->AddElement(nistManager->FindOrBuildElement("H"), 2);
  snow->AddElement(nistManager->FindOrBuildElement("O"), 1);
}

/*****************************************************************/

void G4IceTopDetectorConstruction::CreateIce()
{
  G4NistManager* nistManager = G4NistManager::Instance();
  G4Material* ice = new G4Material("Ice", 0.92 * CLHEP::g / CLHEP::cm3, 2, kStateSolid);
  ice->AddElement(nistManager->FindOrBuildElement("H"), 2);
  ice->AddElement(nistManager->FindOrBuildElement("O"), 1);
  
  // pmt spectral response 300-650 nm
  // https://docushare.icecube.wisc.edu/dsweb/Get/Document-6637/R7081-02%20data%20sheet.pdf
  const G4int ice_bins = 2;
  G4double ice_ephot[ice_bins] = {1.91 * eV, 4.13 * eV};
  G4double ice_refr[ice_bins] = {1.31, 1.31};
  G4MaterialPropertiesTable *mpt_ice = new G4MaterialPropertiesTable ();
  mpt_ice->AddProperty ("RINDEX", ice_ephot, ice_refr, ice_bins);
  ice->SetMaterialPropertiesTable (mpt_ice);
}

/*****************************************************************/

void G4IceTopDetectorConstruction::CreateWater()
{
  G4NistManager* nistManager = G4NistManager::Instance();  
  G4Material* ice = new G4Material("Water", 1.0 * CLHEP::g / CLHEP::cm3, 2, kStateLiquid);
  ice->AddElement(nistManager->FindOrBuildElement("H"), 2);
  ice->AddElement(nistManager->FindOrBuildElement("O"), 1);
}

/*****************************************************************/

void G4IceTopDetectorConstruction::CreatePlastic()
{
  G4NistManager* nistManager = G4NistManager::Instance();
  /*
  G4Material* plastic = new G4Material("Plastic", 1.5 * CLHEP::g / CLHEP::cm3, 3, kStateSolid);
  plastic->AddElement(nistManager->FindOrBuildElement("H"), 20);
  plastic->AddElement(nistManager->FindOrBuildElement("C"), 10);
  plastic->AddElement(nistManager->FindOrBuildElement("O"), 5);
  */
  
  // POM
  G4Material* plastic = new G4Material("Plastic", 1.425 * CLHEP::g / CLHEP::cm3, 3, kStateSolid);
  plastic->AddElement(nistManager->FindOrBuildElement("H"), 2);
  plastic->AddElement(nistManager->FindOrBuildElement("C"), 1);
  plastic->AddElement(nistManager->FindOrBuildElement("O"), 1);
  
  //nistManager->FindOrBuildMaterial("G4_POLYOXYMETHYLENE");
  
}

/*****************************************************************/

void G4IceTopDetectorConstruction::CreateTyvek()
{
  // Tyvek consists of high density polyethylene fibers
  G4NistManager* nistManager = G4NistManager::Instance();
  G4Material* tyvek = new G4Material("Tyvek", 0.304 * CLHEP::g / CLHEP::cm3, 2, kStateSolid);
  tyvek->AddElement(nistManager->FindOrBuildElement("H"), 4);
  tyvek->AddElement(nistManager->FindOrBuildElement("C"), 2);
}

/*****************************************************************/

void G4IceTopDetectorConstruction::CreatePerlite()
{
  G4Material* water = G4Material::GetMaterial("Water"); 
  if(!water) return;
  
  //
  // An average perlite density of 0.1598 g/cm3 has
  // been measured at Southpole in January 2010.
  // 
  G4NistManager* nistManager = G4NistManager::Instance();
  G4Material* perlite = new G4Material("Perlite", 0.1598 * CLHEP::g / CLHEP::cm3, 13, kStateSolid);
  
  /*
  // From the Ryolex Perlite Tech Data Sheet
  perlite->AddElement(nistManager->FindOrBuildElement("O"),  .475);
  perlite->AddElement(nistManager->FindOrBuildElement("Si"), .338);
  perlite->AddElement(nistManager->FindOrBuildElement("Al"), .072);
  perlite->AddElement(nistManager->FindOrBuildElement("K"),  .035);
  perlite->AddElement(nistManager->FindOrBuildElement("Na"), .034);
  perlite->AddElement(nistManager->FindOrBuildElement("Fe"), .006);
  perlite->AddElement(nistManager->FindOrBuildElement("Ca"), .006);
  perlite->AddElement(nistManager->FindOrBuildElement("Mg"), .002);
  // 0.2% are traces. Mn is the most abundant trace.
  perlite->AddElement(nistManager->FindOrBuildElement("Mn"), .002);
  perlite->AddMaterial(water,                                .030);
  */
  
  // in mail-contact to Paul Evenson (02/03/11) he comfirmed,
  // that the air between perlite particles should be taken into account
  // assuming perlite mainly consists of SiO2 with density of 2.254 g/cm3
  // only 7.09% is perlite, the rest is air
  perlite->AddElement(nistManager->FindOrBuildElement("Si"), .0240);
  perlite->AddElement(nistManager->FindOrBuildElement("Al"), .0051);
  perlite->AddElement(nistManager->FindOrBuildElement("K"),  .0025);
  perlite->AddElement(nistManager->FindOrBuildElement("Na"), .0024);
  perlite->AddElement(nistManager->FindOrBuildElement("Fe"), .0004);
  perlite->AddElement(nistManager->FindOrBuildElement("Ca"), .0004);
  perlite->AddElement(nistManager->FindOrBuildElement("Mg"), .0001);
  perlite->AddElement(nistManager->FindOrBuildElement("Mn"), .0001);
  perlite->AddMaterial(water,                                .0021);
  // 0.0337% Oxygen is taken into acoount below;
  // since the perlite sums up only to 7.08%, air is 92.92%
  // consisting of 75.53% Nitrogen, 23.18% Oxygen, 1.28% Argon
  // and 0.01% Carbon (the composition of G4_AIR)
  perlite->AddElement(nistManager->FindOrBuildElement("N"),  .7018);
  perlite->AddElement(nistManager->FindOrBuildElement("O"),  .2491);
  perlite->AddElement(nistManager->FindOrBuildElement("Ar"), .0119);
  perlite->AddElement(nistManager->FindOrBuildElement("C"),  .0001);
}

/*****************************************************************/

void G4IceTopDetectorConstruction::CreateGlassSphere()
{
  // Elemental composition not exact for detailed line-up look at
  // docushare collection 690 -> XRF and ICP Analysis of Pressure Sphere Glass
  
  // 20 lbs. weight = 9072 g
  // 6.5" outer radius & 0.5" thickness = 4024 cm3
  G4NistManager* nistManager = G4NistManager::Instance();
  G4Material* glass = new G4Material("Glass", 2.254 * CLHEP::g / CLHEP::cm3, 2, kStateSolid);
  glass->AddElement(nistManager->FindOrBuildElement("Si"), 1);
  glass->AddElement(nistManager->FindOrBuildElement("O"),  2);
}

/*****************************************************************/

void G4IceTopDetectorConstruction::CreateEffectiveDOMMaterial()
{
  // Mass of the a complete DOM: 12050 g
  // Mass without glass sphere: 2978 g
  // 6" inner glass radius: volume = 14830 cm3
  G4NistManager* nistManager = G4NistManager::Instance();
  G4Material* glass = new G4Material("effectiveDOM", 0.2 * CLHEP::g / CLHEP::cm3, 2, kStateSolid);
  glass->AddElement(nistManager->FindOrBuildElement("Si"), 1);
  glass->AddElement(nistManager->FindOrBuildElement("O"),  2);
}
