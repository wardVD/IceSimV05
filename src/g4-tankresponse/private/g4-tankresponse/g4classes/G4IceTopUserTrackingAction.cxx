/**
 * Copyright (C) 2011
 * The IceCube collaboration
 * ID: $Id$
 *
 * @file G4IceTopUserTrackingAction.cxx
 * @version $Revision$
 * @date $Date$
 * @author Thomas Melzig
 *
 * $LastChangedBy$
 */


#include <g4-tankresponse/g4classes/G4IceTopUserTrackingAction.h>

#include "G4Track.hh"
#include "G4UserLimits.hh"
#include "G4TrackVector.hh"
#include "G4TrackingManager.hh"

G4IceTopUserTrackingAction::G4IceTopUserTrackingAction(){}

void G4IceTopUserTrackingAction::PreUserTrackingAction(const G4Track*){}

void G4IceTopUserTrackingAction::PostUserTrackingAction(const G4Track* track)
{
  const G4LogicalVolume *volume = track->GetLogicalVolumeAtVertex();
  G4UserLimits *limit = volume->GetUserLimits();
  if(!limit) G4cout << "----> G4LogicalVolume: " << volume->GetName() << " has no defined G4UserLimit" << G4endl;
  G4double threshold = limit->GetUserMinEkine(*track);
  G4TrackVector* secondaries = fpTrackingManager->GimmeSecondaries();
  if(secondaries)
  {
    size_t nSeco = secondaries->size();
    if(nSeco>0)
    {
      for(size_t i=0;i<nSeco;i++)
      {
        //check if secondary particle is a gamma
        if((*secondaries)[i]->GetDefinition()->GetParticleName() == "gamma")
        {
          //check if particle energy is below threshold; if true, kill the particle
          G4double energy = (*secondaries)[i]->GetTotalEnergy();
          if(energy < threshold) (*secondaries)[i]->SetTrackStatus(fStopAndKill);
        }
      }
    }
  }
}
