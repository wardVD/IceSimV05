#include <I3Test.h>
#include "neutrino-generator/NuGUtils.h"
#include "dataclasses/physics/I3MCTreeUtils.h"

TEST_GROUP(NuGUtils);

TEST(list_to_tree_conversion){

  I3NuGParticlePtrList l;

  I3NuGParticlePtr primary(new I3NuGParticle(I3Particle::Null,I3Particle::unknown));

  //daughters of the rpimary
  I3NuGParticlePtr d1(new I3NuGParticle(I3Particle::Null,I3Particle::unknown));
  I3NuGParticlePtr d2(new I3NuGParticle(I3Particle::Null,I3Particle::unknown));

  //grandaughters of the primary
  //daughters of daughter one
  I3NuGParticlePtr gd1_1(new I3NuGParticle(I3Particle::Null,I3Particle::unknown));
  I3NuGParticlePtr gd1_2(new I3NuGParticle(I3Particle::Null,I3Particle::unknown));

  d1->GetDaughters().push_back(gd1_1);
  d1->GetDaughters().push_back(gd1_2);

  //daughters of daughter two
  I3NuGParticlePtr gd2_1(new I3NuGParticle(I3Particle::Null,I3Particle::unknown));
  I3NuGParticlePtr gd2_2(new I3NuGParticle(I3Particle::Null,I3Particle::unknown));
  I3NuGParticlePtr gd2_3(new I3NuGParticle(I3Particle::Null,I3Particle::unknown));
  I3NuGParticlePtr gd2_4(new I3NuGParticle(I3Particle::Null,I3Particle::unknown));

  d2->GetDaughters().push_back(gd2_1);
  d2->GetDaughters().push_back(gd2_2);
  d2->GetDaughters().push_back(gd2_3);
  d2->GetDaughters().push_back(gd2_4);

  primary->GetDaughters().push_back(d1);
  primary->GetDaughters().push_back(d2);
    
  l.push_back(primary);

  I3MCTreePtr tree = NuGUtils::ListToTree(l);

  ENSURE(tree->size() == 9);

  I3MCTreePtr tree_2 = NuGUtils::CleanDuplicates(tree);;

  ENSURE(tree_2->size() == tree->size());
}

TEST(list_to_tree_with_dups){

  I3NuGParticlePtrList l;

  I3NuGParticlePtr primary(new I3NuGParticle(I3Particle::Null,I3Particle::unknown));

  //daughters of the rpimary
  I3NuGParticlePtr d1(new I3NuGParticle(I3Particle::Null,I3Particle::unknown));
  I3NuGParticlePtr d2(new I3NuGParticle(I3Particle::Null,I3Particle::unknown));

  //grandaughters of the primary
  //daughters of daughter one
  I3NuGParticlePtr gd1_1(new I3NuGParticle(I3Particle::Null,I3Particle::unknown));
  I3NuGParticlePtr gd1_2(new I3NuGParticle(I3Particle::Null,I3Particle::unknown));

  d1->GetDaughters().push_back(gd1_1);
  d1->GetDaughters().push_back(gd1_2);

  //daughters of daughter two
  I3NuGParticlePtr gd2_1(new I3NuGParticle(I3Particle::Null,I3Particle::unknown));
  I3NuGParticlePtr gd2_2(new I3NuGParticle(I3Particle::Null,I3Particle::unknown));
  I3NuGParticlePtr gd2_3(new I3NuGParticle(I3Particle::Null,I3Particle::unknown));
  I3NuGParticlePtr gd2_4(new I3NuGParticle(I3Particle::Null,I3Particle::unknown));

  d2->GetDaughters().push_back(gd2_1);
  d2->GetDaughters().push_back(gd2_2);
  d2->GetDaughters().push_back(gd2_3);
  d2->GetDaughters().push_back(gd2_4);

  primary->GetDaughters().push_back(d1);
  primary->GetDaughters().push_back(d2);

  //add it twice    
  l.push_back(primary);
  l.push_back(primary);

  I3MCTreePtr tree = NuGUtils::ListToTree(l);

  ENSURE(tree->size() == 18);

  I3MCTreePtr tree_2 = NuGUtils::CleanDuplicates(tree);

  ENSURE(tree_2->size() == tree->size()/2);

}

TEST(clean_duplicates){

  I3NuGParticlePtrList l;

  I3NuGParticlePtr primary(new I3NuGParticle(I3Particle::Null,I3Particle::unknown));

  //daughters of the rpimary
  I3NuGParticlePtr d1(new I3NuGParticle(I3Particle::Null,I3Particle::unknown));
  I3NuGParticlePtr d2(new I3NuGParticle(I3Particle::Null,I3Particle::unknown));

  //grandaughters of the primary
  //daughters of daughter one
  I3NuGParticlePtr gd1_1(new I3NuGParticle(I3Particle::Null,I3Particle::unknown));
  I3NuGParticlePtr gd1_2(new I3NuGParticle(I3Particle::Null,I3Particle::unknown));

  d1->GetDaughters().push_back(gd1_1);
  d1->GetDaughters().push_back(gd1_2);

  //daughters of daughter two
  I3NuGParticlePtr gd2_1(new I3NuGParticle(I3Particle::Null,I3Particle::unknown));
  I3NuGParticlePtr gd2_2(new I3NuGParticle(I3Particle::Null,I3Particle::unknown));
  I3NuGParticlePtr gd2_3(new I3NuGParticle(I3Particle::Null,I3Particle::unknown));
  I3NuGParticlePtr gd2_4(new I3NuGParticle(I3Particle::Null,I3Particle::unknown));

  d2->GetDaughters().push_back(gd2_1);
  d2->GetDaughters().push_back(gd2_2);
  d2->GetDaughters().push_back(gd2_3);
  d2->GetDaughters().push_back(gd2_4);

  primary->GetDaughters().push_back(d1);
  primary->GetDaughters().push_back(d2);

  //pretend one of the daughters is a primary too.    
  l.push_back(primary);
  l.push_back(d2);//adds five more to the tree

  I3MCTreePtr tree = NuGUtils::ListToTree(l);

  ENSURE(tree->size() == 14);

  I3MCTreePtr tree_2 = NuGUtils::CleanDuplicates(tree);

  ENSURE(tree_2->size() == (tree->size() - 5));

  ENSURE(!NuGUtils::CheckForDuplicates(tree_2));

}

TEST(duplicate_primaries){
  I3NuGParticlePtrList l;

  I3NuGParticlePtr p1(new I3NuGParticle(I3Particle::Null,I3Particle::unknown));

  //add the same particle twice
  l.push_back(p1);
  l.push_back(p1);

  I3MCTreePtr tree = NuGUtils::ListToTree(l);

  ENSURE(tree->size() == 2);

  I3MCTreePtr tree_2 = NuGUtils::CleanDuplicates(tree);

  ENSURE(tree_2->size() == 1);

  ENSURE(!NuGUtils::CheckForDuplicates(tree_2));

}
