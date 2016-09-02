#!/usr/bin/env python
#
# Simple NuGen example
# for Old NuGen setup (NuGen v2)
#
#
# For parameter options, see 
# http://wiki.icecube.wisc.edu/index.php/Neutrino_Generator/Setting_Parameters
#

from I3Tray import *

from os.path import expandvars

from icecube import icetray, dataclasses, phys_services, sim_services, dataio, earthmodel_service, neutrino_generator

import os
import sys

#
# for logging
#
icetray.I3Logger.global_logger = icetray.I3PrintfLogger()
icetray.set_log_level(icetray.I3LogLevel.LOG_WARN)

#icetray.set_log_level_for_unit("EarthModelService",icetray.I3LogLevel.LOG_TRACE)
icetray.set_log_level_for_unit("I3NuG",icetray.I3LogLevel.LOG_INFO)
icetray.logging.set_level_for_unit('I3PropagatorModule', 'INFO')

#
# setup parameters
#

# number of processed evts
ev_n = 10

#------------------
# injection neutrino
#------------------
flavorString = "NuMu"
gammaIndex = "1"
energyMinLog = "1"
energyMaxLog = "8"
zenithMin =    "0"
zenithMax =  "180"
azimuthMin =   0.0*I3Units.deg
azimuthMax = 360.0*I3Units.deg

propmode = neutrino_generator.ncgrweighted
if (flavorString == "NuTau") :
    propmode = neutrino_generator.nopropweight

#------------------
# geometory and cross section
#------------------

# injection radius and detection cylinder
injectiontype = neutrino_generator.circle
injectionrad = 1200.*I3Units.m
distanceEntrance = 1200*I3Units.m
distanceExit = 1200*I3Units.m
cylinderparams = [injectionrad, distanceEntrance, distanceExit]

#injectiontype = neutrino_generator.cylinder
#detcylrad = 600.*I3Units.m
#detcyllen = 1200.*I3Units.m
#origin_x = 0.*I3Units.m
#origin_y = 0.*I3Units.m
#origin_z = 0.*I3Units.m
#cylinderparams = [detcylrad,detcyllen,origin_x,origin_y,origin_z]

# detection volume

# name of cross section model.
# use 'cteq5' 'css' 'csms'
xsecmodel = "csms"

#------------------
# weight
#------------------

# Don't touch these options unless you know well.
murangeopt = "11.0"
zenithweightparam = "1.0"

#------------------
# params for earthmodel-service
#------------------
earthmodelservice = "EarthModelService"
earthmodel = ["PREM_mmc"]
materialmodel = ["Standard"]
#icecap = "SimpleIceCap"
icecap = "IceSheet"
icecapangle = 30.*I3Units.degree
detdepth = 1948*I3Units.m

#------------------
# params for steering
#------------------
steername = "NuGSteer"
treename = "I3MCTree"


if(len(sys.argv) >= 2 ):
    xsecmodel = sys.argv[1]
if(len(sys.argv) >= 3 ):
    zenithweightparam = sys.argv[2]
if(len(sys.argv) >= 4 ):
    icecap = sys.argv[3]
if(len(sys.argv) >= 5 ):
    zenithMin = sys.argv[4]
if(len(sys.argv) >= 6 ):
    zenithMax = sys.argv[5]
if(len(sys.argv) >= 7 ):
    energyMinLog = sys.argv[6]
if(len(sys.argv) >= 8 ):
    energyMaxLog = sys.argv[7]
if(len(sys.argv) >= 9 ):
    gammaIndex = sys.argv[8]

outi3filename = flavorString + "_" + murangeopt + "_ZenA" + zenithweightparam  + "_IceCap" + icecap + "_MinZen" + zenithMin + "_MaxZen" + zenithMax + "_ELogMin" + energyMinLog + "_ELogMax" + energyMaxLog + "_E" + gammaIndex + ".i3"

#
# create I3Tray
#
tray = I3Tray()

from os.path import expandvars
tray.AddModule("I3InfiniteSource", "source",
               prefix = expandvars("$I3_TESTDATA/sim/GeoCalibDetectorStatus_IC86.55380_corrected.i3.gz")
               )

tray.AddModule("I3MCEventHeaderGenerator","ev")

#
# generate random service
#
random = phys_services.I3SPRNGRandomService(1, 10000, 1)
tray.context['I3RandomService'] = random

#
# generate earthmodel service
#
earth = earthmodel_service.EarthModelService("EarthModelService","",
                              earthmodel, materialmodel, 
                              icecap, icecapangle, detdepth)
tray.context['EarthModel'] = earth 

#
# generate steering service
#
steer = neutrino_generator.Steering(earth, 
                                    neutrino_generator.full,
                                    neutrino_generator.nugen, 
                                    injectiontype)

steer.cylinder_params = cylinderparams

tray.context[steername] = steer

#
# generate interaction service
#
interaction = neutrino_generator.I3NuGInteractionInfo(random, steer, xsecmodel)
interaction.initialize()
#interaction.view_interaction_info()

#
# generate propagator
#

nugen = neutrino_generator.I3NeutrinoPropagator(random, steer, interaction)
nugen.prop_mode = propmode

# Base propagators for charged leptons
from icecube.sim_services.propagation import get_propagators
from icecube import sim_services

# this function makes propagators for ALL particles!
#propagators = get_propagators()

# For single unit test, use the function...
propagators = sim_services.I3ParticleTypePropagatorServiceMap()

for flavor in 'E', 'Mu', 'Tau':
	for ptype in '', 'Bar':
		propagators[getattr(dataclasses.I3Particle.ParticleType, 'Nu'+flavor+ptype)] = nugen


#
# start icetray
#

# you have to set number of events manually 
# if you use DiffuseSource module

steer.n_gen = ev_n

tray.AddModule("I3NuGDiffuseSource","diffusesource")(
               ("SteeringName", steername),
               ("NuFlavor", flavorString),
               ("GammaIndex",          int(gammaIndex)),
               ("EnergyMinLog",        float(energyMinLog)),
               ("EnergyMaxLog",        float(energyMaxLog)),
               ("ZenithMin",           float(zenithMin)*I3Units.deg),
               ("ZenithMax",           float(zenithMax)*I3Units.deg),
               ("AzimuthMin",          azimuthMin),
               ("AzimuthMax",          azimuthMax),
               ("ZenithWeightParam",  float(zenithweightparam)),
    )

tray.Add('I3PropagatorModule', PropagatorServices=propagators, 
          RandomService=random)

tray.AddModule("I3Writer","writer")(
    ("filename", outi3filename))

tray.AddModule("TrashCan", "the can")

tray.Execute()
tray.Finish()

