/*
 * copyright  (C) 2010
 * The Icecube Collaboration
 *
 * $Id: G4IceTopIonPhysics.h 86420 2012-03-20 16:00:37Z kislat $
 *
 * @version $Revision: 86420 $
 * @date $LastChangedDate: 2012-03-20 11:00:37 -0500 (Tue, 20 Mar 2012) $
 * @author Fabian Kislat <fabian.kislat@desy.de>, Peter Nießen (tanktop)
 * Last changed by: $LastChangedBy: kislat $
 */

#ifndef G4TANKRESPONSE_G4ICETOPIONPHYSICS_H_INCLUDED
#define G4TANKRESPONSE_G4ICETOPIONPHYSICS_H_INCLUDED

#include <globals.hh>
#include <G4ios.hh>
#include <G4VPhysicsConstructor.hh>
#include <G4HadronElasticProcess.hh>
#include <G4LElastic.hh>
#include <G4DeuteronInelasticProcess.hh>
#include <G4LEDeuteronInelastic.hh>
#include <G4TritonInelasticProcess.hh>
#include <G4LETritonInelastic.hh>
#include <G4AlphaInelasticProcess.hh>
#include <G4LEAlphaInelastic.hh>
#include <G4hIonisation.hh>
#include <G4ionIonisation.hh>
#include <G4hMultipleScattering.hh>

/**
   @class G4IceTopIonPhysics
   @brief The ion physics
   @author GEANT4/Peter Niessen
   @date Sun Jul 25 00:24:42 EDT 2004

   The ion physics. Check the source for details
*/
class G4IceTopIonPhysics : public G4VPhysicsConstructor {

public: 

  /**
   * The constructor
   */
  G4IceTopIonPhysics();

  /**
   * The virtual destructor
   */
  virtual ~G4IceTopIonPhysics();

  /**
   * This method will be invoked in the Construct() method. 
   * each particle type will be instantiated
   */
  virtual void ConstructParticle();
 
  /**
   * This method will be invoked in the Construct() method.
   * each physics process will be instantiated and
   * registered to the process manager of each particle type 
   */
  virtual void ConstructProcess();

protected:
  // Elastic Process
  G4HadronElasticProcess      elasticProcess_;
  G4LElastic*                 elasticModel_;

  // Generic Ion physics
  G4hMultipleScattering        ionMultipleScattering_;
  G4ionIonisation             ionIonisation_;

  // Deuteron physics
  G4hMultipleScattering        deuteronMultipleScattering_;
  G4hIonisation               deuteronIonisation_;
  G4DeuteronInelasticProcess  deuteronProcess_;
  G4LEDeuteronInelastic*      deuteronModel_;

  // Triton physics
  G4hMultipleScattering        tritonMultipleScattering_;
  G4hIonisation               tritonIonisation_;
  G4TritonInelasticProcess    tritonProcess_;
  G4LETritonInelastic*        tritonModel_;
  
  // Alpha physics
  G4hMultipleScattering        alphaMultipleScattering_;
  G4hIonisation               alphaIonisation_;
  G4AlphaInelasticProcess     alphaProcess_;
  G4LEAlphaInelastic*         alphaModel_;

  // He3 physics
  G4hMultipleScattering        he3MultipleScattering_;
  G4hIonisation               he3Ionisation_;

};


#endif // G4TANKRESPONSE_G4ICETOPIONPHYSICS_H_INCLUDED
