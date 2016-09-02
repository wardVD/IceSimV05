#!/usr/bin/python
'''
Injects particles into tank 36A.

Type of particle, number of particles per event, and number
of events are provided as command line options.
'''

import os
from os.path import expandvars
import unittest
import argparse

from I3Tray import I3Tray
from icecube import icetray, dataclasses, dataio, tableio, phys_services, simclasses, sim_services
from icecube.icetray import I3Units
from icecube import topsimulator, g4_tankresponse

gcd_file = expandvars(
    "$I3_TESTDATA/sim/GeoCalibDetectorStatus_IC79.55380_corrected.i3.gz")

parser = argparse.ArgumentParser()
parser.add_argument("-p", dest="particle", default="mu-",
                    help="particle type (see help of g4-tankresponse)")
parser.add_argument("-n", dest="nevents", type=int, default=1,
                    help="number of events")
parser.add_argument("-k", dest="nhits", type=int, default=1,
                    help="number of particle hits per event")

args = parser.parse_args()

tray = I3Tray()

tray.AddService("I3GSLRandomServiceFactory", "random_injector",
                Seed=1,
                InstallServiceAs = "random_injector"
                )

tray.AddService("I3GSLRandomServiceFactory", "random_tankresponse",
                Seed=1,
                InstallServiceAs = "random_tankresponse"
                )

tray.AddModule("I3InfiniteSource", "source",
               Stream=icetray.I3Frame.DAQ,
               prefix=gcd_file
               )

tray.AddService("I3InjectorFactory<I3ParticleInjector>", "injector",
                RandomServiceName='random_injector',
                NumParticles=args.nhits,
                NumEvents=args.nevents,
                ParticleType=args.particle,
                EnergyRange=[10.0 * I3Units.GeV],
                TankKeys=["36A"],
                )

tray.AddService("I3IceTopResponseFactory<I3G4TankResponse>", "topresponse",
                RandomServiceName="random_tankresponse",
                #VisMacro = "vis.mac"
                )

# The actual topsimulator module (depends on injector- and tank
# response services)
tray.AddModule("I3TopSimulator", "top-simulator",
               InjectorServiceName="injector",
               ResponseServiceName="topresponse",
               PrimaryName="MCPrimary",
               IceTopPESeriesName="MCTopPESeriesMap",
               InIceMCTreeName="",
               IceTopMCTreeName="IceTopMCTree"
               )

tray.AddModule("I3Writer","i3-writer",
               filename = "g4-tankresponse-test.i3",
               streams = [icetray.I3Frame.DAQ]
               )

tray.AddModule("TrashCan", "trash-can")

tray.Execute()
tray.Finish()
