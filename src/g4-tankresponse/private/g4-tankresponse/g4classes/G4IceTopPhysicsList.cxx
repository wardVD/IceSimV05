/**
 * Copyright (C) 2009
 * The IceCube collaboration
 * ID: $Id: G4IceTopPhysicsList.cxx 86420 2012-03-20 16:00:37Z kislat $
 *
 * @file G4IceTopPhysicsList.cxx
 * @version $Rev: 86420 $
 * @date $Date: 2012-03-20 11:00:37 -0500 (Tue, 20 Mar 2012) $
 * @author Tilo Waldenmaier, Thomas Melzig
 */


//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include <globals.hh>
#include <g4-tankresponse/g4classes/G4IceTopPhysicsList.h>
#include "G4IceTopGeneralPhysics.h"
#include "G4IceTopEMPhysics.h"
#include "G4IceTopMuonPhysics.h"
#include "G4IceTopHadronPhysics.h"
#include "G4IceTopIonPhysics.h"

#include <G4ProcessManager.hh>
#include <G4ParticleTypes.hh>
#include <G4UserSpecialCuts.hh>


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4IceTopPhysicsList::G4IceTopPhysicsList()
  : G4VUserPhysicsList()
{
  defaultCutValue = 1.0 * cm;
  SetVerboseLevel(1);

  RegisterPhysics(new G4IceTopGeneralPhysics);
  RegisterPhysics(new G4IceTopEMPhysics);
  RegisterPhysics(new G4IceTopMuonPhysics);
  RegisterPhysics(new G4IceTopHadronPhysics);
  RegisterPhysics(new G4IceTopIonPhysics);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4IceTopPhysicsList::~G4IceTopPhysicsList()
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void G4IceTopPhysicsList::SetCuts()
{
  //G4VUserPhysicsList::SetCutsWithDefault method sets
  //the default cut value for all particle types
  //
  SetCutsWithDefault();
  
  if (verboseLevel>0) DumpCutValuesTable();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
