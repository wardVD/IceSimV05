/**
 * Copyright (C) 2009
 * The IceCube collaboration
 * ID: $Id: G4IceTopRunManager.h 67781 2010-10-22 10:07:26Z tilo $
 *
 * @file G4IceTopRunManager.h
 * @version $Rev: 67781 $
 * @date $Date: 2010-10-22 05:07:26 -0500 (Fri, 22 Oct 2010) $
 * @author Tilo Waldenmaier
 */


#ifndef TOPSIMULATOR_G4ICETOPRUNMANAGER_H
#define TOPSIMULATOR_G4ICETOPRUNMANAGER_H

#include <G4RunManager.hh>

class G4ParticleGun;

class G4IceTopRunManager: public G4RunManager
{
 public:
  G4IceTopRunManager();
  
  static G4IceTopRunManager* GetIceTopRunManager() {return (G4IceTopRunManager*)GetRunManager();};
  
  // Disable BeamOn
  void BeamOn(G4int n_event,const char* macroFile=0,G4int n_select=-1);
  
  void InitializeRun();
  void InjectParticle(G4ParticleGun* particleGun);
  void TerminateRun();
  
 protected:
  G4Event* GenerateEvent(G4int i_event);
  
 private:
  // This method is an exact copy of UpdateScoring which is private in the G4RunManager
  void Update_Scoring();

};

#endif
