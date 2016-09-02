/*
 * copyright  (C) 2010
 * The Icecube Collaboration
 *
 * $Id: G4IceTopMuonPhysics.h 86420 2012-03-20 16:00:37Z kislat $
 *
 * @version $Revision: 86420 $
 * @date $LastChangedDate: 2012-03-20 11:00:37 -0500 (Tue, 20 Mar 2012) $
 * @author Fabian Kislat <fabian.kislat@desy.de>, Peter Nießen (tanktop)
 * Last changed by: $LastChangedBy: kislat $
 */

#ifndef G4TANKRESPONSE_G4ICETOPMUONPHYSICS_H_INCLUDED
#define G4TANKRESPONSE_G4ICETOPMUONPHYSICS_H_INCLUDED

#include <globals.hh>
#include <G4VPhysicsConstructor.hh>
#include <G4MuMultipleScattering.hh>
#include <G4MuBremsstrahlung.hh>
#include <G4MuPairProduction.hh>
#include <G4MuIonisation.hh>
#include <G4hIonisation.hh>
#include <G4MuonMinusCaptureAtRest.hh>

/**
   @class G4IceTopMuonPhysics
   @brief Physics for the muons/taus

   This class implements the physics processes for the muons. For
   muons, it contains
   - Ionisation
   - Multiple scattering
   - Bremsstrahlung
   - Pair production
   - Capture at rest (mu-)
   For taus, it does
   - Multiple scattering
   - Ionisation
*/
class G4IceTopMuonPhysics : public G4VPhysicsConstructor
{
public: 
  G4IceTopMuonPhysics();
  ~G4IceTopMuonPhysics();
  
  void ConstructParticle();
  void ConstructProcess();

private:
  // Muon physics
  G4MuIonisation         muPlusIonisation_;
  G4MuMultipleScattering   muPlusMultipleScattering_;
  G4MuBremsstrahlung     muPlusBremsstrahlung_;
  G4MuPairProduction     muPlusPairProduction_;

  G4MuIonisation         muMinusIonisation_;
  G4MuMultipleScattering   muMinusMultipleScattering_;
  G4MuBremsstrahlung     muMinusBremsstrahlung_;
  G4MuPairProduction     muMinusPairProduction_;

  G4MuonMinusCaptureAtRest muMinusCaptureAtRest_;

  // Tau physics
  G4MuMultipleScattering   tauPlusMultipleScattering_;
  G4hIonisation          tauPlusIonisation_;

  G4MuMultipleScattering   tauMinusMultipleScattering_;
  G4hIonisation          tauMinusIonisation_;
};

#endif // G4TANKRESPONSE_G4ICETOPMUONPHYSICS_H_INCLUDED
