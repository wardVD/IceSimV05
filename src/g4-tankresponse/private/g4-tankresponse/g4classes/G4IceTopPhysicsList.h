/**
 * Copyright (C) 2009
 * The IceCube collaboration
 * ID: $Id: G4IceTopPhysicsList.h 86420 2012-03-20 16:00:37Z kislat $
 *
 * @file G4IceTopPhysicsList.h
 * @version $Rev: 86420 $
 * @date $Date: 2012-03-20 11:00:37 -0500 (Tue, 20 Mar 2012) $
 * @author Tilo Waldenmaier, Thomas Melzig, Fabian Kislat
 */

#ifndef _TOPSIMULATOR_G4ICETOPPHYSICSLIST_H
#define _TOPSIMULATOR_G4ICETOPPHYSICSLIST_H

#include <G4VModularPhysicsList.hh>


class G4IceTopPhysicsList: public G4VModularPhysicsList
{
public:
  G4IceTopPhysicsList();
  ~G4IceTopPhysicsList();
  
private:
  void SetCuts();
};

#endif
