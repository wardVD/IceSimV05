#!/usr/bin/env python
#
# NuGen InEarth propagation script
# for Old NuGen setup (NuGen v2)
#

from os.path import expandvars
from icecube import icetray, dataclasses, phys_services, sim_services, dataio,earthmodel_service, neutrino_generator
import os
import sys

from I3Tray import *
load("libicetray")

#----------------
# default values
#----------------

ev_n=100000
#ev_n=30

flavorString = "NuMu"
xsecmodel = "csms"

# this is scale factor for cross section.
# [CC scale, NC scale, GR scale]
xsec_scale_cc = 1.0
xsec_scale_nc = 1.0
xsec_scale_gr = 1.0

elogmin = 2.0
elogmax = 8.0
gamma = 1.0
zenmin = 90.*I3Units.degree
#zenmin = 140.*I3Units.degree
zenmax = 180.*I3Units.degree
zenalpha = 1.0  # 0.1 ~ 1.9, with a larger number the injector injects more upgoing events

# random parameters
# seed : use same seed through the dataset
# nfiles : number of files per dataset
# fileno : file number < nfiles
seed = 1234
nfiles = 1
fileno = 0

# don't change them
propmode = neutrino_generator.autodetect
simmode = "InEarth"
material = ["Standard"]

#----------------
# arguments
#----------------

if(len(sys.argv) == 1 ):
    print("you have to specify outfile name")
    exit(0)

# arg 1 : out filename
outi3filename= sys.argv[1]

# arg 2 : flavor
if(len(sys.argv) > 2 ):
    flavorString = sys.argv[2]

# arg 3 : NGen
if(len(sys.argv) > 3 ):
    ev_n = int(sys.argv[3])

# arg 4 : gamma index
if(len(sys.argv) > 4 ):
    gamma = float(sys.argv[4])

# arg 5 : minlogE:maxlogE
if(len(sys.argv) > 5 ):
    elogs= (sys.argv[5]).split(':')
    elogmin = float(elogs[0])
    elogmax = float(elogs[1])

# arg 6 : minzendeg:maxzendeg
if(len(sys.argv) > 6 ):
    zens= (sys.argv[6]).split(':')
    zenmin= float(zens[0])*I3Units.degree
    zenmax = float(zens[1])*I3Units.degree

# arg 7 : zenith generation weight
# 0.1 to 1.9, larger value gives more virtically upgoing events
# 1.0 gives flat distribution
if(len(sys.argv) > 7 ):
    zenalpha = float(sys.argv[7])

# arg 8 : cross section model
if(len(sys.argv) > 8 ):
    xsecmodel = sys.argv[8]

# arg 9 : global xsec scale params cc:nc:gr
if(len(sys.argv) > 9 ):
    xsec_scales= (sys.argv[9]).split(':')
    xsec_scale_cc = float(xsec_scales[0])
    xsec_scale_nc = float(xsec_scales[1])
    xsec_scale_gr = float(xsec_scales[2])

global_xsec_scale = [xsec_scale_cc, xsec_scale_nc, xsec_scale_gr]

# arg 10 - : earth models
# you may specify multiple models with ':'
earth = [] # default will be used if not specified
if (len(sys.argv) > 10):
    earth = (sys.argv[10]).split(':')

# arg 11 : random seed
if (len(sys.argv) > 11):
    seed = int(sys.argv[11])

# arg 12 : number of files per dataset
if (len(sys.argv) > 12):
    nfiles = int(sys.argv[12])

# arg 13 : file ID
if (len(sys.argv) > 13):
    fileno  = int(sys.argv[13])


print("outfile %s" % outi3filename)
print("flavor %s" % flavorString)
print("NGen %d" % ev_n)
print("gamma %f" % gamma)
print("elogmin %f, elogmax %f" % (elogmin, elogmax))
print("zenmin %f, zenmax %f" % (zenmin, zenmax))
print("zenalpha %f" % zenalpha)
print("xsec %s" % xsecmodel)
print("xsec scale " , global_xsec_scale)
print("earthmodel " , earth)
print("seed %d" % seed)
print("nfiles %d" % nfiles)
print("fileno %d" % fileno)

#--------------
# for logging
#--------------

icetray.I3Logger.global_logger = icetray.I3PrintfLogger()
icetray.set_log_level(icetray.I3LogLevel.LOG_WARN)
icetray.set_log_level_for_unit("EarthModelService",icetray.I3LogLevel.LOG_INFO)
#icetray.set_log_level_for_unit("I3NuG",icetray.I3LogLevel.LOG_TRACE)
#icetray.set_log_level_for_unit("I3NuG",icetray.I3LogLevel.LOG_INFO)
icetray.set_log_level_for_unit("I3NuG",icetray.I3LogLevel.LOG_WARN)

#--------------
# icetray start
#--------------

tray = I3Tray()

#from icecube.examples_simulation.configure_simulation import configure_service_modules
#configure_service_modules( tray, season )
#random = tray.context['random']

# generate random service

print ("RandomService params: Seed %d, NFiles %d, FileNo %d" %(seed, nfiles, fileno))

random = phys_services.I3SPRNGRandomService(seed, nfiles, fileno)
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
                IceCapType = "IceSheet",
                DetectorDepth = 1948*I3Units.m,
                PathToDataFileDir = "")

tray.AddService("I3NuGSteeringFactory", "steering",
                EarthModelName = "EarthModelService",
                NEvents = ev_n,
                SimMode = simmode, 
                VTXGenMode = "NuGen", 
                GlobalXsecScaleFactor = global_xsec_scale
                )

#
# for pointsource simulation, see I3NuGInjector.cxx
#
tray.AddService("I3NuGInjectorFactory", "injector",
                RandomService = random,
                SteeringName = "steering",
                NuFlavor = flavorString,
                GammaIndex = float(gamma),
                EnergyMinLog = elogmin,
                EnergyMaxLog = elogmax,
                ZenithMin = zenmin, 
                ZenithMax = zenmax,
                ZenithWeightParam = zenalpha,
                AngleSamplingMode = "ANG"  # sample flat in zenith (not in cos(zen))
               )

tray.AddService("I3NuGInteractionInfoFactory", "interaction",
                RandomService = random,
                SteeringName = "steering",
                CrossSectionModel = xsecmodel
               )

tray.AddModule("I3NeutrinoGenerator","generator",
                RandomService = random,
                SteeringName = "steering",
                InjectorName = "injector",
                InteractionInfoName = "interaction",
                PropagationWeightMode = propmode 
              )

tray.AddModule("I3Writer","writer")(
    ("filename", outi3filename), 
    ("streams", [icetray.I3Frame.DAQ]), 
    ("skipkeys", ["I3MCWeightDict"]), 
    )

class Counter(icetray.I3ConditionalModule) : 
    def __init__(self, ctx):
        icetray.I3ConditionalModule.__init__(self, ctx)
        self.counter_ = 0

    def Configure(self):
        self.counter_ = 0

    def DAQ(self, frame):
        if (self.counter_ % 10000 == 0) :
            print("%d events finished" % self.counter_)
        self.counter_ = self.counter_ + 1
        return True

    def Finish(self):
        return


tray.AddModule(Counter, "counter")

tray.AddModule("TrashCan", "the can")

tray.Execute()
tray.Finish()

