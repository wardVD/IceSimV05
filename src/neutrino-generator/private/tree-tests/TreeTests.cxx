#include <I3Test.h>
#include <icetray/I3Tray.h>
#include <icetray/I3Units.h>
#include <earthmodel-service/EarthModelService.h>
#include <neutrino-generator/Steering.h>
#include <neutrino-generator/legacy/I3NuGInteractionInfo.h>
#include <neutrino-generator/legacy/I3NuGInjector.h>
#include <neutrino-generator/legacy/I3NeutrinoGenerator.h>

#include <boost/python/import.hpp>

TEST_GROUP(NuGenTreeTests);

void test_flavor(std::string flavorString)
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
 
   tray.AddService("I3EarthModelServiceFactory", "EarthModelService")
     ("IceCapType", "IceSheet")
     ("DetectorDepth",1948*I3Units::m)
     ("PathToDataFileDir","");
  
   std::vector<double> cylinder_param;
   cylinder_param.push_back(1200*I3Units::m); // for circle radius
   
   tray.AddService("I3NuGSteeringFactory", "steering")
                 ("EarthModelName","EarthModelService")
                 ("NEvents", 20)
                 ("SimMode", "Full")
                 ("VTXGenMode", "NuGen")
                 ("InjectionMode", "Circle")
                 ("CylinderParams", cylinder_param);

   tray.AddService("I3NuGInjectorFactory", "injector")
                 ("SteeringName", "steering")
                 ("NuFlavor", flavorString)
                 ("GammaIndex", 1.0)
                 ("EnergyMinLog", 5)
                 ("EnergyMaxLog", 8)
                 ("ZenithMin", 0.*I3Units::degree)
                 ("ZenithMax", 180.*I3Units::degree);

   tray.AddService("I3NuGInteractionInfoFactory", "interaction")
                 ("SteeringName", "steering")
                 ("CrossSectionModel", "cteq5");

   tray.AddModule("I3NeutrinoGenerator","generator")
                 ("SteeringName", "steering")
                 ("InjectorName", "injector")
                 ("InteractionInfoName", "interaction");

   tray.AddModule("NuGenTestModule", "test")
     ("NeutrinoFlavor",flavorString);

   tray.AddModule("TrashCan", "trash");

   tray.Execute();
   tray.Finish();
}

TEST(find_numu)
{ 
	test_flavor("NuMu");
}

TEST(find_e)
{ 
	test_flavor("NuE");
}

TEST(find_tau)
{ 
	test_flavor("NuTau");
}
