#!/usr/bin/env python
#
# Simple NuGen example
#
# For parameter options, see 
# http://wiki.icecube.wisc.edu/index.php/Neutrino_Generator/Setting_Parameters
#

from os.path import expandvars
import argparse


from I3Tray import *
from icecube import phys_services, dataio, neutrino_generator
from icecube.hdfwriter import I3HDFWriter


parser = argparse.ArgumentParser(description='Process some integers.')
parser.add_argument('--log-level','-l',
                    default = "warn",
                    choices = ["warn","info","debug","trace"],
                    help='Level of logging')
parser.add_argument('--gcd-file','-G',
                    default = expandvars("$I3_TESTDATA/sim/"
                                         "GeoCalibDetectorStatus_IC86.55380_corrected.i3.gz"),
                    help='name of GCD file to use')
parser.add_argument('--number-of-events','-n',
                    type=int, default = 1000,
                    help='Number of neutrino events to generate')
parser.add_argument('--zenith-range', '-z',
                    type =float, nargs = 2, default=(0,180),
                    metavar=("Zmin","Zmax"),
                    help = "Range of Zenith Values to generate")
parser.add_argument('--energy-range','-e',
                    type =float, nargs = 2, default=(2,8),
                    metavar=("logEmin","logEmax"),
                    help = "Log of the energy range")
parser.add_argument('--gamma', '-g',
                    type =float, default=1.0,
                    help = "Power law index of Generation Spectrum")
parser.add_argument('--flavor', '-f',
                    default="NuMu",
                    choices = ["NuE","NuMu","NuTau"],
                    help = "Power law index of Generation Spectrum")
parser.add_argument('--outfile',default='NeutrinoGenerator.i3',
                    help = "Name of i3 file to output")
args = parser.parse_args()

print args

icetray.set_log_level(icetray.I3LogLevel.names["LOG_"+args.log_level.upper()])

#create tray
tray = I3Tray()

#random provides random numbers for Monte Carlo
tray.context['I3RandomService'] = phys_services.I3GSLRandomService(0)

tray.Add("I3InfiniteSource", prefix = args.gcd_file)

#this puts an event header in every Q frame
tray.Add("I3MCEventHeaderGenerator")

#earth model provides the density of the earth for propigation of
#neutirnos through the Earth
tray.Add("I3EarthModelServiceFactory", "EarthModelService",
         IceCapType = "IceSheet",
         )

#steering service provides the neutrino from the source service and
#injectector
tray.Add("I3NuGSteeringFactory", "SteeringService",
         EarthModelName = "EarthModelService",
         NEvents = args.number_of_events,
         )

tray.Add("I3NuGInteractionInfoFactory", "InteractionService",
         SteeringName = "SteeringService",
         )

# Source service provides the the energy,
# direction and flavor of neutrinos to be propigated
tray.Add("I3NuGDiffuseSource",
         SteeringName = "SteeringService",
         NuFlavor = args.flavor,
         GammaIndex = args.gamma,
         EnergyMinLog = args.energy_range[0],
         EnergyMaxLog = args.energy_range[1],
         ZenithMin = args.zenith_range[0]*I3Units.degree,
         ZenithMax = args.zenith_range[1]*I3Units.degree,
         )

tray.Add("I3NeutrinoGenerator",
         SteeringName = "SteeringService",
         InteractionInfoName = "InteractionService",
         )

tray.Add(I3HDFWriter,'writer',
         output = 'foo.hdf5',
         keys = ["I3MCTree","I3MCWeightDict","MCTimeIncEventID","NuGPrimary",],
         SubEventStreams=["fullevent"])

tray.Add("I3Writer",
         filename=args.outfile,
         streams=[icetray.I3Frame.DAQ],
         )

tray.Execute()
tray.Finish()

print "Finished Running IceTray"

