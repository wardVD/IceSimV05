#ifndef NuGTreeUtils_H
#define NuGTreeUtils_H
/**
 *  copyright  (C) 2005
 *  the IceCube collaboration
 *  $Id: $
 *
 *  @version $Revision: $
 *  @date    $Date: $
 *  @author  Alex Olivas <olivas@icecube.umd.edu>
 *
 * @brief TreeUtils header file
 * Utilities for converting Particle lists to I3MCTrees
 * 
 */

#include "neutrino-generator/Particle.h"
#include "dataclasses/physics/I3MCTree.h"
#include <string>

namespace TreeUtils{

  void ListToTree(I3MCTreePtr tree, nugen::ParticlePtrList list);

  void AddParticleToTree(I3MCTree &, boost::shared_ptr<nugen::Particle>,
                         std::string indent="");

  bool CheckForDuplicates(I3MCTree &);
  bool CheckForInIces(I3MCTree &);

}

#endif 
