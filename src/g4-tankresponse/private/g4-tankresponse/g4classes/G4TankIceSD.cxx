/**
 * Copyright (C) 2009
 * The IceCube collaboration
 * ID: $Id: G4TankIceSD.cxx 86084 2012-03-08 01:54:18Z claudio.kopper $
 *
 * @file G4TankIceSD.cxx
 * @version $Rev: 86084 $
 * @date $Date: 2012-03-07 19:54:18 -0600 (Wed, 07 Mar 2012) $
 * @author Tilo Waldenmaier, Thomas Melzig
 */


#include <g4-tankresponse/g4classes/G4TankIceSD.h>
#include <g4-tankresponse/g4classes/G4IceTopTank.h>

#include <G4Step.hh>
#include <G4HCofThisEvent.hh>
#include <G4TouchableHistory.hh>
#include <G4ios.hh>
#include <iostream>
#include <ios>
#include <fstream>

#include <G4VProcess.hh>
#include <G4OpticalPhoton.hh>
#include "G4Poisson.hh"

#include <icetray/I3Units.h>

#include "G4Version.hh"

#if G4VERSION_NUMBER >= 950
// The G4MaterialPropertyVector is gone since 4.9.5.
// It has been typedef'd to a G4UnorderedPhysicsVector
// with a different interface. Try to support both
// versions with an ifdef.
#define MATERIAL_PROPERTY_VECTOR_IS_PHYSICS_VECTOR
#endif


G4TankIceSD::G4TankIceSD(G4String name, const std::map<OMKey, G4ThreeVector>& domPositions)
  : G4VSensitiveDetector(name), domPositions_(domPositions)
{}


G4TankIceSD::~G4TankIceSD() {}


void G4TankIceSD::Initialize(G4HCofThisEvent* HCE)
{
  std::map<OMKey, G4ThreeVector>::const_iterator om_iter;
  for(om_iter=domPositions_.begin(); om_iter!=domPositions_.end(); ++om_iter)
  {
    sumEdep_[om_iter->first] = 0;
    cogTime_[om_iter->first] = 0;
    cherenkovCounter_[om_iter->first] = 0;
    cherenkovCounterWeight_[om_iter->first] = 0;
  }
}


G4bool G4TankIceSD::ProcessHits(G4Step* aStep, G4TouchableHistory*)
{
  // Get energy deposit/time --> CHECK WHAT THESE QUANTITIES ACTUALLY MEAN !!!!!!!!!!!!!!!!!!!!!!!!
  G4double edep = aStep->GetTotalEnergyDeposit();
  G4double time = aStep->GetPreStepPoint()->GetGlobalTime();
  G4double cherenkovNumber = GetCerenkovNumber(aStep);
  
  if(edep<=0 && cherenkovNumber<=0) return false;
  
  // Get Position relative to ice center
  G4ThreeVector preStepPoint = aStep->GetPreStepPoint()->GetPosition();
  G4ThreeVector postStepPoint = aStep->GetPostStepPoint()->GetPosition();
  G4TouchableHandle theTouchable = aStep->GetPreStepPoint()->GetTouchableHandle();
  G4ThreeVector worldPosition = (preStepPoint + postStepPoint) / 2.0;
  G4ThreeVector localPosition = theTouchable->GetHistory()->
                                GetTopTransform().TransformPoint(worldPosition);
  
  std::map<OMKey, G4ThreeVector>::const_iterator om_iter;
  for(om_iter=domPositions_.begin(); om_iter!=domPositions_.end(); ++om_iter)
  {
    G4double radius = sqrt(pow(om_iter->second.x() - localPosition.x(), 2) +
                           pow(om_iter->second.y() - localPosition.y(), 2));
    G4double height = (om_iter->second.z() - localPosition.z());
    
    sumEdep_[om_iter->first] += edep;
    cogTime_[om_iter->first] += edep*time;
    cherenkovCounterWeight_[om_iter->first] += GetProbability(radius, height) * cherenkovNumber;
    cherenkovCounter_[om_iter->first] += cherenkovNumber;
  }
  return true;
}


void G4TankIceSD::EndOfEvent(G4HCofThisEvent*)
{
  std::map<OMKey, G4ThreeVector>::const_iterator om_iter;
  for(om_iter=domPositions_.begin(); om_iter!=domPositions_.end(); ++om_iter)
  {
    if(sumEdep_[om_iter->first]>0)
    {
      cogTime_[om_iter->first] /= sumEdep_[om_iter->first];
    }
  }
}


G4double G4TankIceSD::GetCerenkovNumber(G4Step* aStep)
{
  // same function as in "source/processes/electromagnetic/xrays/src/G4Cerenkov.cc" but without
  // cerenkov angle and only for an energy independent refraction index
  G4Track* aTrack = aStep->GetTrack();
  const G4DynamicParticle* aParticle = aTrack->GetDynamicParticle();
  const G4Material* aMaterial = aTrack->GetMaterial();
  
  G4StepPoint* pPreStepPoint  = aStep->GetPreStepPoint();
  G4StepPoint* pPostStepPoint = aStep->GetPostStepPoint();
  
  G4MaterialPropertiesTable* aMaterialPropertiesTable = aMaterial->GetMaterialPropertiesTable();
  if (!aMaterialPropertiesTable) return 0.0;
  
  G4MaterialPropertyVector* Rindex = aMaterialPropertiesTable->GetProperty("RINDEX");
  if (!Rindex) return 0.0;
  
  const G4double Rfact = 369.81 / (CLHEP::eV * CLHEP::cm);
  const G4double charge = aParticle->GetDefinition()->GetPDGCharge();
  const G4double beta = (pPreStepPoint->GetBeta() + pPostStepPoint->GetBeta()) / 2.;
  if (beta <= 0.0) return 0.0;
  G4double BetaInverse = 1. / beta;
  
  // Min and Max photon energies 
#ifdef MATERIAL_PROPERTY_VECTOR_IS_PHYSICS_VECTOR
  G4double Pmin = Rindex->GetMinLowEdgeEnergy();
  G4double Pmax = Rindex->GetMaxLowEdgeEnergy();
#else
  G4double Pmin = Rindex->GetMinPhotonEnergy();
  G4double Pmax = Rindex->GetMaxPhotonEnergy();
#endif
  
  // Min and Max Refraction Indices 
#ifdef MATERIAL_PROPERTY_VECTOR_IS_PHYSICS_VECTOR
  G4double nMin = Rindex->GetMinValue();
  G4double nMax = Rindex->GetMaxValue();
#else
  G4double nMin = Rindex->GetMinProperty();
  G4double nMax = Rindex->GetMaxProperty();
#endif
  if (nMin!=nMax)
  {
    log_error("Support for energy dependent refraction index not yet implemented!");
    return 0.0;
  }
  
  // If n(Pmax) < 1/Beta -- no photons generated
  if (nMax < BetaInverse) return 0.0;
  
  G4double MeanNumberOfPhotons = Rfact * charge/eplus * charge/eplus * (Pmax - Pmin) *
                                 (1. - BetaInverse * BetaInverse / nMin / nMin);
  
  G4double step_length = aStep->GetStepLength();
  
  return MeanNumberOfPhotons * step_length;
}


G4double G4TankIceSD::GetProbability(G4double radius, G4double height)
{
  height = 0.90 - height / CLHEP::m;
  radius = radius / CLHEP::m;
  G4double p0 = 0.0340 +  0.0120 * height;
  G4double p1 = 0.0400 + 0.000622 * exp(-1.13 + 8.03 * height);
  G4double p2 = 0.917 + 2.34 * exp(-1.82 + 3.12 * height);
  G4double p3 = -0.00325 - 0.00199 * height - 0.0121 * height*height;
  
  return (p0 + p3 * radius) + (p1 - p0) * exp(-p2*p2 * radius*radius);
}
