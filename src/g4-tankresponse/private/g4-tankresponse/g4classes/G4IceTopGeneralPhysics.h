/*
 * copyright  (C) 2010
 * The Icecube Collaboration
 *
 * $Id: G4IceTopGeneralPhysics.h 86420 2012-03-20 16:00:37Z kislat $
 *
 * @version $Revision: 86420 $
 * @date $LastChangedDate: 2012-03-20 11:00:37 -0500 (Tue, 20 Mar 2012) $
 * @author Fabian Kislat <fabian.kislat@desy.de> Last changed by: $LastChangedBy: kislat $
 */

#ifndef G4TANKRESPONSE_G4ICETOPGENERALPHYSICS_H_INCLUDED
#define G4TANKRESPONSE_G4ICETOPGENERALPHYSICS_H_INCLUDED

#include <globals.hh>
#include <G4VPhysicsConstructor.hh>
#include <G4Decay.hh>


class G4IceTopGeneralPhysics : public G4VPhysicsConstructor
{
public: 
  G4IceTopGeneralPhysics();
  virtual ~G4IceTopGeneralPhysics();

  void ConstructParticle();
  void ConstructProcess();

private:
  G4Decay decay;
};

#endif // G4TANKRESPONSE_G4ICETOPGENERALPHYSICS_H_INCLUDED
