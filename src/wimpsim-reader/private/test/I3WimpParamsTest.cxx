/**
 * copyright  (C) 2012
 * the WimpSim::IceCube Collaboration
 * $Id: $
 *
 * @file I3WimpSimReaderTest.cxx
 * @author mzoll <marcel.zoll@fysik.su.se>
 * @date $Date: 2012-12-20$
 * @brief Tests the I3WimpParams for Setting/Getting
 */

#include <I3Test.h>
#include <dataclasses/I3Constants.h>
#include <dataclasses/external/CompareFloatingPoint.h>
#include <dataclasses/I3Time.h>
#include <wimpsim-reader/I3WimpParams.h>
#include <wimpsim-reader/WimpSimTools.h>

TEST_GROUP(I3WimpParams);

TEST(1_SETGET) {
  // set everything once and retrieve everything again
  I3WimpParams test_instance = I3WimpParams();
  test_instance.SetSource(I3WimpParams::SUN);
  test_instance.SetMass((unsigned int)1000);
  test_instance.SetChannel(WimpSim::unknown);
  test_instance.SetNuWeight(1.E-47);
  test_instance.SetLepWeight(1.E-47);
  test_instance.SetHadWeight(1.E-47);
  test_instance.SetVgen(1.);
  test_instance.SetTime(I3Time((int32_t) 2011, (int64_t) 0));
  ENSURE(test_instance.GetSource() == I3WimpParams::SUN);
  ENSURE(test_instance.GetMass() == (unsigned int)1000);
  ENSURE(test_instance.GetChannel() == WimpSim::unknown);
  ENSURE(test_instance.GetNuWeight() == 1.E-47);
  ENSURE(test_instance.GetLepWeight() == 1.E-47);
  ENSURE(test_instance.GetHadWeight() == 1.E-47);
  ENSURE(test_instance.GetVgen() == 1.);
  ENSURE(test_instance.GetTime() == I3Time((int32_t) 2011, (int64_t) 0));
  // additional setters/getters
  test_instance.SetLepWeightE47(1.);
  ENSURE(CompareFloatingPoint::Compare(test_instance.GetLepWeightE47(),1));
  test_instance.SetHadWeightE47(1.);
  ENSURE(CompareFloatingPoint::Compare(test_instance.GetHadWeightE47(),1));
  /*
  test_instance.SetSource("Sun");
  ENSURE(test_instance.GetSource() == "Sun");
  test_instance.SetSource("Earth");
  ENSURE(test_instance.GetSource() == "Earth");
  test_instance.SetSource(1);
  ENSURE(test_instance.GetSource() == "Sun");
  test_instance.SetSource(2);
  ENSURE(test_instance.GetSource() == "Earth");
  */
}

