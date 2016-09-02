#!/usr/bin/env python
#
# NuGen example with parameter options
#
# For parameter options, see 
# http://wiki.icecube.wisc.edu/index.php/Neutrino_Generator/Setting_Parameters
#

from I3Tray import *

from os.path import expandvars

import os
import sys

from icecube import icetray, dataclasses, phys_services, sim_services, dataio, neutrino_generator
from icecube import genie_icetray

tray = I3Tray()
seed=1

#ev_n=100000
#ev_n=10000
#ev_n=1000
#ev_n=100
ev_n=10

#
# logging setting
#
icetray.I3Logger.global_logger = icetray.I3PrintfLogger()
icetray.set_log_level(icetray.I3LogLevel.LOG_INFO)
icetray.set_log_level_for_unit("I3NuG",icetray.I3LogLevel.LOG_TRACE)

tray.AddService("I3SPRNGRandomServiceFactory","random")(
    ("Seed",seed),
    ("NStreams",seed+1),
    ("StreamNum",1))

from os.path import expandvars
tray.AddModule("I3InfiniteSource", "source",
               prefix = expandvars("$I3_TESTDATA/sim/GeoCalibDetectorStatus_IC86.55380_corrected.i3.gz")
               )

tray.AddModule("I3MCEventHeaderGenerator","ev")

#------------------
# injection neutrino
#------------------
flavorString = "NuMu"
gammaIndex = "2"
energyMinLog = "0"
energyMaxLog = "2.3"
zenithMin =    "0"
zenithMax =  "180"
azimuthMin =   0.0*I3Units.deg
azimuthMax = 360.0*I3Units.deg

#------------------
# geometory
#------------------

#injection mode
# Circle or Surface 
injectionMode = "Circle"

# old style - define cylinder radius
# effective only when Circle mode is chosen 
injectionRadius = 1200*I3Units.m

# detection volume
distanceEntrance = 1200*I3Units.m
distanceExit = 1200*I3Units.m

#------------------
# weight
#------------------

# Don't touch these options unless you know well.
murangeopt = "11.0"
zenith_weight = "1.0"

#------------------
# params for earthmodel-service
#------------------
earthmodel = ["PREM_mmc"]
#icecap = "SimpleIceCap"
icecap = "IceSheet"
detectorDepth = 1948*I3Units.m

#------------------
# params for steering
#------------------
treename = "I3MCTree"

if(len(sys.argv) >= 2 ):
    zenith_weight = sys.argv[1]
if(len(sys.argv) >= 5 ):
    icecap = sys.argv[2]
if(len(sys.argv) >= 4 ):
    zenithMin = sys.argv[3]
if(len(sys.argv) >= 5 ):
    zenithMax = sys.argv[4]
if(len(sys.argv) >= 6 ):
    energyMinLog = sys.argv[5]
if(len(sys.argv) >= 7 ):
    energyMaxLog = sys.argv[6]
if(len(sys.argv) >= 8 ):
    gammaIndex = sys.argv[7]

outi3filename = "Genie" + flavorString + "_ZenA" + zenith_weight  + "_IceCap" + icecap + "_MinZen" + zenithMin + "_MaxZen" + zenithMax + "_ELogMin" + energyMinLog + "_ELogMax" + energyMaxLog + "_E" + gammaIndex + ".i3"

print("out i3 filename is ", outi3filename)
    
#
# Step 1. add EarthModel Service.
#

earthmodelservice = "EarthModelService"

tray.AddService("I3EarthModelServiceFactory", earthmodelservice,
               EarthModels = earthmodel,
               IceCapType = icecap,
               DetectorDepth = detectorDepth,
               PathToDataFileDir = "")

#
# Step 2. add Steering Service
#

steername = "NuGSteer"

cylinderparams = [injectionRadius,distanceEntrance,distanceExit]
tray.AddService("I3NuGSteeringFactory", steername,
               EarthModelName = earthmodelservice,
               NEvents = ev_n,
               MCTreeName = treename,
               VTXGenMode = "Genie",
               InjectionMode = injectionMode,
               CylinderParams = cylinderparams
               )
               
#
# Step 3. add primary module
#
tray.AddModule("I3NuGDiffuseSource","diffusesource", 
               SteeringName = steername,
               NuFlavor = flavorString,
               GammaIndex = int(gammaIndex),
               EnergyMinLog = float(energyMinLog),
               EnergyMaxLog = float(energyMaxLog),
               ZenithMin = float(zenithMin)*I3Units.deg,
               ZenithMax = float(zenithMax)*I3Units.deg,
               AzimuthMin = azimuthMin,
               AzimuthMax = azimuthMax,
               ZenithWeightParam = float(zenith_weight))

#
# Step 4. add Genie module
#
# it requires lhapdf and spline tables.
# Download following files before you run the script.
#
# spline files (you need one of them):
# http://www.icecube.wisc.edu/~hoshina/genie/xsec-spline-default-allnu-icerock-n1000-e300.xml (91MB)
# http://www.icecube.wisc.edu/~hoshina/genie/xsec-spline-default-allnu-clarke-n1000-e300.xml (384MB)
#
# lhapdf table (this is full set, you may not need some of them)
# http://www.icecube.wisc.edu/~hoshina/genie/lhapdf.tgz (6.0GB)
# 

# a random number generator
randomService = phys_services.I3SPRNGRandomService(
    seed = 1,
    nstreams = 10000,
    streamnum = 100)

tray.AddModule("I3NuGGenie","genie",
    SteeringName = steername,
    RandomService = randomService, # alternatively, this can be None and the I3RandomService can be installed using tray.AddService()
    SplineFilename = expandvars("$I3_SRC/neutrino-generator/resources/genie/splines/xsec-spline-default-allnu-icerock-n1000-e300.xml"),
    LHAPDFPath = expandvars("$I3_SRC/neutrino-generator/resources/genie/lhapdf"),
    MinEnergyLog = float(energyMinLog),
    MaxEnergyLog = float(energyMaxLog))

tray.AddModule("I3Writer","writer")(
    ("filename", outi3filename))

#tray.AddModule("Dump","dump")
tray.AddModule("TrashCan", "the can")

tray.Execute()
tray.Finish()
