#include <I3Test.h>

#include "neutrino-generator/legacy/I3NeutrinoGenerator.h"
#include "icetray/test/ConstructorTest.h"
#include "icetray/I3Tray.h"
#include <iostream>
#include <icetray/I3Logging.h>

TEST_GROUP(I3NuG);

TEST(icetray_inspect){
  boost::python::import("icecube.icetray");
  boost::python::import("icecube.neutrino_generator");
  clean_constructor_test<I3NeutrinoGenerator>();
}

void test_module(std::string injtype, 
              std::string flavorString,
              std::vector<double> nunubarratio,
              std::string simmode)
{
   boost::python::import("icecube.icetray");
   boost::python::import("icecube.dataio");
   boost::python::import("icecube.neutrino_generator");

   I3Tray tray;
  
   tray.AddService("I3GSLRandomServiceFactory", "random");

   std::string I3_TESTDATA;
   if(getenv("I3_TESTDATA") != NULL) {
     I3_TESTDATA = std::string(getenv("I3_TESTDATA"));
   }else{
     ENSURE( false, "Neither I3_TESTDATA is defined!");
   }
   std::string prefix = I3_TESTDATA + "/sim/GeoCalibDetectorStatus_IC86.55380_corrected.i3.gz";
   tray.AddModule("I3InfiniteSource","source")
     ("prefix", prefix);
		 
   tray.AddModule("I3MCEventHeaderGenerator","headme")
     ("Year",2001)
     ("DAQTime",314159);
 
   
   GetIcetrayLogger()->SetLogLevel(I3LOG_WARN);
    
   tray.AddService("I3EarthModelServiceFactory", "EarthModelService")
     ("IceCapType", "IceSheet")
     ("DetectorDepth",1948*I3Units::m)
     ("PathToDataFileDir","");
  
   tray.AddService("I3NuGSteeringFactory", "steering")
                 ("EarthModelName","EarthModelService")
                 ("NEvents", 20)
                 ("SimMode", simmode)
                 ("VTXGenMode", "NuGen");

   tray.AddService("I3NuGInteractionInfoFactory", "interaction")
                 ("SteeringName", "steering")
                 ("CrossSectionModel", "cteq5");

   if (injtype=="OLD") {
      tray.AddService("I3NuGInjectorFactory", "injector")
                    ("SteeringName", "steering")
                    ("NuFlavor", flavorString)
                    ("GammaIndex", 1.0)
                    ("EnergyMinLog", 5)
                    ("EnergyMaxLog", 8)
                    ("ZenithMin", 0.*I3Units::degree)
                    ("ZenithMax", 180.*I3Units::degree);

   } else if (injtype=="POINT") {
      tray.AddModule("I3NuGPointSource", "injector")
                    ("SteeringName", "steering")
                    ("NuFlavor", flavorString)
                    ("NuNuBarRatio", nunubarratio)
                    ("GammaIndex", 1.0)
                    ("EnergyMinLog", 5)
                    ("EnergyMaxLog", 8)
                    ("Zenith", 45.*I3Units::degree)
                    ("Azimuth", 45.*I3Units::degree);
   } else {
      tray.AddModule("I3NuGDiffuseSource", "injector")
                    ("SteeringName", "steering")
                    ("NuFlavor", flavorString)
                    ("NuNuBarRatio", nunubarratio)
                    ("GammaIndex", 1.0)
                    ("EnergyMinLog", 5)
                    ("EnergyMaxLog", 8)
                    ("ZenithMin", 0.*I3Units::degree)
                    ("ZenithMax", 180.*I3Units::degree);
   }

   tray.AddModule("I3NeutrinoGenerator","generator")
                 ("SteeringName", "steering")
                 ("InjectorName", "injector")
                 ("InteractionInfoName", "interaction");

   tray.AddModule("I3NuGenTestModule", "test")
                 ("NuNuBarRatio", nunubarratio)
                 ("runmode", injtype + flavorString + simmode);

   tray.AddModule("TrashCan", "trash");

   tray.Execute();
   tray.Finish();
}

TEST(test_old_default_NuE)
{ 
        std::cerr << "start test_old_default_NuE" << std::endl;
        std::vector<double> nunubarratio;
        nunubarratio.push_back(1.0); // for Nu 
        nunubarratio.push_back(1.0); // for NuBar
	test_module("OLD", "NuE", nunubarratio, "FULL");
}

TEST(test_old_default_NuMu)
{ 
        std::cerr << "start test_old_default_NuMu" << std::endl;
        std::vector<double> nunubarratio;
        nunubarratio.push_back(1.0); // for Nu 
        nunubarratio.push_back(1.0); // for NuBar
	test_module("OLD", "NuMu", nunubarratio, "FULL");
}

TEST(test_old_default_NuTau)
{ 
        std::cerr << "start test_old_default_NuTau" << std::endl;
        std::vector<double> nunubarratio;
        nunubarratio.push_back(1.0); // for Nu 
        nunubarratio.push_back(1.0); // for NuBar
	test_module("OLD", "NuMu", nunubarratio, "FULL");
}

TEST(test_point_default_NuE)
{ 
        std::cerr << "start test_point_default_NuE" << std::endl;
        std::vector<double> nunubarratio;
        nunubarratio.push_back(1.0); // for Nu 
        nunubarratio.push_back(2.0); // for NuBar
	test_module("POINT", "NuE", nunubarratio, "FULL");
}

TEST(test_point_default_NuMu)
{ 
        std::cerr << "start test_point_default_NuMu" << std::endl;
        std::vector<double> nunubarratio;
        nunubarratio.push_back(1.0); // for Nu 
        nunubarratio.push_back(2.0); // for NuBar
	test_module("POINT", "NuMu", nunubarratio, "FULL");
}

TEST(test_point_default_NuTau)
{ 
        std::cerr << "start test_point_default_NuTau" << std::endl;
        std::vector<double> nunubarratio;
        nunubarratio.push_back(2.0); // for Nu 
        nunubarratio.push_back(1.0); // for NuBar
	test_module("POINT", "NuTau", nunubarratio, "FULL");
}

TEST(test_diffuse_NuE)
{ 
        std::cerr << "start test_diffuse_default_NuE" << std::endl;
        std::vector<double> nunubarratio;
        nunubarratio.push_back(0); // for Nu 
        nunubarratio.push_back(2.0); // for NuBar
	test_module("DIFFUSE", "NuE", nunubarratio, "FULL");
}

TEST(test_diffuse_NuMu)
{ 
        std::cerr << "start test_diffuse_default_NuMu" << std::endl;
        std::vector<double> nunubarratio;
        nunubarratio.push_back(2.0); // for Nu 
        nunubarratio.push_back(0); // for NuBar
	test_module("DIFFUSE", "NuMu", nunubarratio, "FULL");
}

TEST(test_diffuse_NuTau)
{ 
        std::cerr << "start test_diffuse_default_NuTau" << std::endl;
        std::vector<double> nunubarratio;
        nunubarratio.push_back(1.0); // for Nu 
        nunubarratio.push_back(2.0); // for NuBar
	test_module("DIFFUSE", "NuTau", nunubarratio, "FULL");
}

TEST(test_diffuse_NuMu_FINAL)
{ 
        std::cerr << "start test_diffuse_default_NuMu_FINAL" << std::endl;
        std::vector<double> nunubarratio;
        nunubarratio.push_back(1.0); // for Nu 
        nunubarratio.push_back(2.0); // for NuBar
	test_module("DIFFUSE", "NuMu", nunubarratio, "FINALONLY");
}

TEST(test_diffuse_NuE_FINAL)
{ 
        std::cerr << "start test_diffuse_default_NuE_FINAL" << std::endl;
        std::vector<double> nunubarratio;
        nunubarratio.push_back(1.0); // for Nu 
        nunubarratio.push_back(1.0); // for NuBar
	test_module("DIFFUSE", "NuE", nunubarratio, "FINALONLY");
}

TEST(test_diffuse_NuTau_FINAL)
{ 
        std::cerr << "start test_diffuse_default_NuTau_FINAL" << std::endl;
        std::vector<double> nunubarratio;
        nunubarratio.push_back(2.0); // for Nu 
        nunubarratio.push_back(1.0); // for NuBar
	test_module("DIFFUSE", "NuTau", nunubarratio, "FINALONLY");
}



