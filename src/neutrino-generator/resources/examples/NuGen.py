#!/usr/bin/env python
#
# Simple NuGen example
# for Old NuGen setup (NuGen v2)
#
# For parameter options, see 
# http://wiki.icecube.wisc.edu/index.php/Neutrino_Generator/Setting_Parameters
#

from I3Tray import *

from os.path import expandvars

from icecube import icetray, dataclasses, phys_services, sim_services, dataio, neutrino_generator

import os
import sys

#
# for logging
#
icetray.I3Logger.global_logger = icetray.I3PrintfLogger()
#icetray.set_log_level(icetray.I3LogLevel.LOG_WARN)
#icetray.set_log_level_for_unit("EarthModelService",icetray.I3LogLevel.LOG_TRACE)
#icetray.set_log_level_for_unit("I3NuG",icetray.I3LogLevel.LOG_TRACE)
#icetray.set_log_level_for_unit("I3NuG",icetray.I3LogLevel.LOG_DEBUG)

tray = I3Tray()
#ev_n=1000000
#ev_n=200000
ev_n=100

# for this example I set propmode as AUTODETECT
# to keep all input neutrinos. 
# if you want to simulate CC interaction inside Earth 
# so that some neutrinos will be absorbed,
# set nugen.nopropweight instead.
# AUTODETECT option takes into account of particle flavor,
# if a propagating particle is NuTau it switch off weighted propataion.
propmode = neutrino_generator.autodetect

flavorString = "NuMu"
nunubarratio = [1,1]  # Nu : NuBar

earth = ["PREM_mmc"]
material = ["Standard"]

# you may change the cross section model here
xsec = "csms"
#xsec = "cteq5"

# energy setting
gamma = "1"
emin = "2"
emax = "8"

# zenith setting
minzen = "0"
maxzen = "180"
zenithweightparam = "1"

# icecap model
# default is now SimpleIceCap. You need to set IceSheet explicitly 
# if you want to reproduce old simulation.
#icecapmodel = "SimpleIceCap"
icecapmodel = "IceSheet"

# injection type
# default is now Surface (old name : Cylinder), which is similar to MuonGun(more efficient).
# You need to set Circle explicitly if you want to reproduce old simulation.
'''
injectionmode = "Circle"
injectionrad = 1200.*I3Units.m
distanceEntrance = 1200*I3Units.m
distanceExit = 1200*I3Units.m
cylinderparams = [injectionrad, distanceEntrance, distanceExit]
'''
injectionmode = "Surface"
detcylrad = 950.*I3Units.m
detcyllen = 1900.*I3Units.m
origin_x = 0.*I3Units.m
origin_y = 0.*I3Units.m
origin_z = 0.*I3Units.m
cylinderparams = [detcylrad,detcyllen,origin_x,origin_y,origin_z]

if(len(sys.argv) > 1 ):
    flavorString = sys.argv[1]
if(len(sys.argv) > 2 ):
    xsec = sys.argv[2]
if(len(sys.argv) > 3 ):
    gamma = sys.argv[3]
if(len(sys.argv) > 4 ):
    emin = sys.argv[4]
if(len(sys.argv) > 5 ):
    emax = sys.argv[5]
if(len(sys.argv) > 6 ):
    minzen = sys.argv[6]
if(len(sys.argv) > 7 ):
    maxzen = sys.argv[7]
if(len(sys.argv) > 8 ):
    zenithweightparam = sys.argv[8]
if(len(sys.argv) > 9 ):
    injectionmode = sys.argv[9]

outi3filename = flavorString + "_" + xsec + "_ZenA" + zenithweightparam + "_InjMode" + injectionmode + "_MinZen" + minzen + "_MaxZen" + maxzen + "_ELogMin" + emin + "_ELogMax" + emax + "_E" + gamma + ".i3"

print("out i3 filename is ", outi3filename)


# generate random service
random = phys_services.I3SPRNGRandomService(1, 10000, 1)
tray.context['I3RandomService'] = random


from os.path import expandvars
tray.AddModule("I3InfiniteSource", "source",
               prefix = expandvars("$I3_TESTDATA/sim/GeoCalibDetectorStatus_IC86.55380_corrected.i3.gz")
               )

tray.AddModule("I3MCEventHeaderGenerator","ev")

#
# At least EarthModelService & Steering Service are required
#

tray.AddService("I3EarthModelServiceFactory", "EarthModelService",
                EarthModels = earth,
                MaterialModels = material,
                IceCapType = icecapmodel,
                DetectorDepth = 1948*I3Units.m,
                PathToDataFileDir = "")

#
# SimMode : Full (default) Earth propagation + final interaction
# FinalOnly : final interaction only
#
tray.AddService("I3NuGSteeringFactory", "steering",
                EarthModelName = "EarthModelService",
                NEvents = ev_n,
                SimMode = "FULL",   
                VTXGenMode = "NuGen",
                InjectionMode = injectionmode,
                CylinderParams = cylinderparams
                )

'''
#
# This old setting still works
#
tray.AddService("I3NuGInjectorFactory", "injector",
                RandomService = random,
                SteeringName = "steering",
                NuFlavor = flavorString,
                GammaIndex = float(gamma),
                EnergyMinLog = float(emin),
                EnergyMaxLog = float(emax),
                ZenithMin = float(minzen)*I3Units.degree,
                ZenithMax = float(maxzen)*I3Units.degree,
                ZenithWeightParam = float(zenithweightparam) 
               )

'''
#
# But this new setting have more control
#
tray.AddModule("I3NuGDiffuseSource","diffusesource", 
                RandomService = random,
                SteeringName = "steering",
                NuFlavor = flavorString,
                NuNuBarRatio = nunubarratio,
                GammaIndex = int(gamma),
                EnergyMinLog = float(emin),
                EnergyMaxLog = float(emax),
                ZenithMin = float(minzen)*I3Units.degree,
                ZenithMax = float(maxzen)*I3Units.degree,
                ZenithWeightParam = float(zenithweightparam))

tray.AddService("I3NuGInteractionInfoFactory", "interaction",
                RandomService = random,
                SteeringName = "steering",
                CrossSectionModel = xsec
               )

tray.AddModule("I3NeutrinoGenerator","generator",
                RandomService = random,
                SteeringName = "steering",
                #InjectorName = "injector", # you don't need this line if you don't use old injector
                InteractionInfoName = "interaction",
                PropagationWeightMode = propmode
              )

'''
# this is an example to fill weights with old name scheme.
# the "TotalInteractionProbabilityWeight" is renamed to "TotalWeight" and I would like to encoulage using the new name,
# but if you don't have time to rename these parameters in your script, use the module before your analysis.
# fill_old_weights.py is in neutrino-generator/resources/example/

from fill_old_weights import fill_old_weights
tray.AddModule(fill_old_weights,"filloldweights")
'''

tray.AddModule("I3Writer","writer")(
    ("filename", outi3filename),
    ("streams", [icetray.I3Frame.DAQ])
    )

tray.AddModule("TrashCan", "the can")

tray.Execute()
tray.Finish()

