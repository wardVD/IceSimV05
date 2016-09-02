/**
 * copyright  (C) 2012
 * the WimpSim::IceCube Collaboration
 * $Id: $
 *
 * @file WimpSimToolsTest.cxx
 * @author mzoll <marcel.zoll@fysik.su.se>
 * @date $Date: 2012-12-20$
 * @brief Tests the WimpSimTools
 */

#include <I3Test.h>
#include <vector>
#include <string>
#include <boost/foreach.hpp>
#include "dataclasses/I3Constants.h"
#include "wimpsim-reader/WimpSimTools.h"

using namespace WimpSim;
using namespace WimpSimTools;

TEST_GROUP(WimpSimTools);

TEST(1_ParticleEnum){
  std::vector<std::string> string_particles_to_test;
  string_particles_to_test.push_back("e-");
  string_particles_to_test.push_back("e+");
  string_particles_to_test.push_back("mu-");
  string_particles_to_test.push_back("mu-");
  string_particles_to_test.push_back("tau+");
  string_particles_to_test.push_back("tau-");
  string_particles_to_test.push_back("nu_e");
  string_particles_to_test.push_back("nu_e~");
  string_particles_to_test.push_back("nu_mu");
  string_particles_to_test.push_back("nu_mu~");
  string_particles_to_test.push_back("nu_tau");
  string_particles_to_test.push_back("nu_tau~");
  string_particles_to_test.push_back("had");

  for (unsigned int part_iter=0; part_iter<string_particles_to_test.size(); part_iter++) {
    ENSURE(I3ParticleTypeToParticleType(ParticleTypeToI3ParticleType(StringToParticleType(string_particles_to_test[part_iter])))
      == I3ParticleTypeToParticleType(WimpSimToI3ParticleType(string_particles_to_test[part_iter])));
  }
}

TEST(2_DecayChannelEnum){
  //0...14 + 100

  for (unsigned int dc=0; dc<=14; dc++)
    ENSURE(DecayChannelToInt(ConvertToDecayChannel(dc)) == dc);

  ENSURE(DecayChannelToInt(ConvertToDecayChannel(100)) == 100); 
}

TEST(3_Trigonometry){

  ENSURE(IceCubeAziRadFromWimpSimAziRad(0) == 0.5*I3Constants::pi); 
  ENSURE(IceCubeAziRadFromWimpSimAziRad(I3Constants::pi) == 1.5*I3Constants::pi); //icecube_azi = 2.5*I3Constants::pi - wimpsim_azi;
  ENSURE(IceCubeAziRadFromWimpSimAziRad(2*I3Constants::pi) == 0.5*I3Constants::pi);
  
  ENSURE(IceCubeZenRadFromWimpSimElRad(-0.5*I3Constants::pi) == I3Constants::pi); //0.5*I3Constants::pi - wimpsim_el
  ENSURE(IceCubeZenRadFromWimpSimElRad(+0.5*I3Constants::pi) == 0.);
  
  ENSURE(IceCubeZenRadFromWimpSimNadirRad(I3Constants::pi) == 0.);//I3Constants::pi - wimpsim_nadir
  ENSURE(IceCubeZenRadFromWimpSimNadirRad(0.) == I3Constants::pi);
}
