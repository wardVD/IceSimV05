/**
 * Copyright (C) 2009
 * The IceCube collaboration
 * ID: $Id: G4TankIceSD.h 67738 2010-10-21 13:59:07Z kislat $
 *
 * @file G4TankIceSD.h
 * @version $Rev: 67738 $
 * @date $Date: 2010-10-21 08:59:07 -0500 (Thu, 21 Oct 2010) $
 * @author Tilo Waldenmaier, Thomas Melzig
 */


#ifndef _G4TANKRESPONSE_G4TankIceSD_H
#define _G4TANKRESPONSE_G4TankIceSD_H

#include <icetray/OMKey.h>

#include <G4VSensitiveDetector.hh>
#include <G4ThreeVector.hh>

#include <map>

class G4Step;
class G4HCofThisEvent;
class G4TouchableHistory;


class G4TankIceSD : public G4VSensitiveDetector
{
 public:
   G4TankIceSD(G4String name, const std::map<OMKey, G4ThreeVector>& domPositions);
  ~G4TankIceSD();
  
  void Initialize(G4HCofThisEvent *HCE);
  G4bool ProcessHits(G4Step *aStep, G4TouchableHistory *ROhist);
  void EndOfEvent(G4HCofThisEvent *HCE);
  
  G4double GetEDep(const OMKey& omKey) {return sumEdep_[omKey];};
  G4double GetTime(const OMKey& omKey) {return cogTime_[omKey];};
  G4double GetNumCherenkov(const OMKey& omKey) {return cherenkovCounter_[omKey];};
  G4double GetNumCherenkovWeight(const OMKey& omKey) {return cherenkovCounterWeight_[omKey];};
  
 private:
  //ExN04TrackerHitsCollection *trackerCollection;
  const std::map<OMKey, G4ThreeVector> domPositions_;
  
  G4double GetCerenkovNumber(G4Step* aStep);
  G4double GetProbability(G4double radius, G4double height);
  
  std::map<OMKey, G4double> sumEdep_;
  std::map<OMKey, G4double> cogTime_;
  std::map<OMKey, G4double> cherenkovCounter_;
  std::map<OMKey, G4double> cherenkovCounterWeight_;
};

#endif
