#!/usr/bin/env python
#
# pybinding load test
#

from I3Tray import *
from icecube import icetray, dataclasses, phys_services, sim_services, dataio, earthmodel_service, neutrino_generator

#load("libicetray")

#
# for logging
#
icetray.I3Logger.global_logger = icetray.I3PrintfLogger()
icetray.set_log_level(icetray.I3LogLevel.LOG_WARN)
#icetray.set_log_level_for_unit("EarthModelService",icetray.I3LogLevel.LOG_TRACE)
#icetray.set_log_level_for_unit("I3NuG",icetray.I3LogLevel.LOG_TRACE)
icetray.set_log_level_for_unit("I3NuG",icetray.I3LogLevel.LOG_INFO)


# generate random service
random = phys_services.I3GSLRandomService(42)


# generate earthmodel service
earthmodel = ["PREM_mmc"]
materialmodel = ["Standard"]
earth = earthmodel_service.EarthModelService("EarthModelService","",
                              earthmodel, materialmodel, 
                              "IceSheet",
                              20.*I3Units.degree, 1948*I3Units.m)

steer = neutrino_generator.Steering(earth)

icetray.logging.log_info("default steer injection mode is %s" % (steer.injection_mode), "steering")
icetray.logging.log_info("default steer vtc genmode is %s" % (steer.vtx_gen_mode), "steering")

steer.injectionmode = neutrino_generator.surface 

# see Steering.h for cylinder params.
cylrad = 600*I3Units.m
cyllen = 1200*I3Units.m
cylx   = 0
cyly   = 0
cylz   = -200*I3Units.m
cylinderparams = [cylrad,cyllen,cylx,cyly,cylz]
steer.cylinder_params = cylinderparams
steer.print_setting()

# ensure that the params are correctly set
copied_cylparams = steer.cylinder_params
assert(copied_cylparams[0] == 600*I3Units.m)
assert(copied_cylparams[4] == -200*I3Units.m)

# generate interaction service
interaction = neutrino_generator.I3NuGInteractionInfo(random, steer, "csms")
interaction.initialize()
interaction.view_interaction_info()

# generate NuGen
nugen = neutrino_generator.I3NeutrinoPropagator(random, steer, interaction)

# ensure that propagation mode setting sticks
assert(nugen.prop_mode == neutrino_generator.PropagationMode.autodetect)
nugen.prop_mode = neutrino_generator.PropagationMode.ncgrweighted
assert(nugen.prop_mode == neutrino_generator.PropagationMode.ncgrweighted)

# check if type conversion is working correctly
assert(neutrino_generator.PropagationMode.ncgrweighted == neutrino_generator.to_propagation_mode("NCGRWeighted"))
assert(neutrino_generator.PropagationMode.autodetect == neutrino_generator.to_propagation_mode("AutoDetect"))
assert(neutrino_generator.PropagationMode.nopropweight == neutrino_generator.to_propagation_mode("NoPropWeight"))

from os.path import expandvars
if os.environ.get("I3_TESTDATA") :
    i3_testdata = expandvars("$I3_TESTDATA") 
else :
    i3_testdata = expandvars("$I3_TESTDATA")

# load test 2

tray = I3Tray()
tray.context['I3RandomService'] = random

tray.AddModule("I3InfiniteSource", "source",
               prefix = i3_testdata + "/sim/GeoCalibDetectorStatus_IC86.55380_corrected.i3.gz")

tray.AddModule("I3MCEventHeaderGenerator","ev")
tray.AddService("I3EarthModelServiceFactory", "EarthModelService")

tray.AddService("I3NuGSteeringFactory", "steering",
                NEvents = 10,
                EarthModelName = "EarthModelService",
                InjectionMode = "Surface",
                CylinderParams = cylinderparams
                )

tray.AddService("I3NuGInjectorFactory", "injector",
                RandomService = random,
                SteeringName = "steering")

tray.AddService("I3NuGInteractionInfoFactory", "interaction",
                RandomService = random,
                SteeringName = "steering")

tray.AddModule("I3NeutrinoGenerator","generator",
                RandomService = random,
                SteeringName = "steering",
                InjectorName = "injector",
                InteractionInfoName = "interaction"
              )

#tray.AddModule("Dump", "dump")
tray.AddModule("TrashCan", "the can")

tray.Execute()
tray.Finish()




