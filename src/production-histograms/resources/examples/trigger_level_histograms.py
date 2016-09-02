#!/usr/bin/env python

from os.path import expandvars

DEFAULT_GCD = expandvars("$I3_TESTDATA/sim/GeoCalibDetectorStatus_2013.56429_V1.i3.gz")

from optparse import OptionParser

parser = OptionParser()

parser.add_option("-i","--infiles",
                  dest = "INFILES",
                  help = "Regex to pass to glob that specify the input files.")
parser.add_option("-g","--gcdfile",
                  dest = "GCDFN",
                  default = DEFAULT_GCD,
                  help = "Input GCD file.")
parser.add_option("-o","--output-filename",
                  dest = "OUTPUT_FN",
                  default = "output.pkl",
                  help = "Filename of the output pickle file.")
(options, args) = parser.parse_args()

import glob, os

filelist = [options.GCDFN]
filelist.extend(glob.glob(options.INFILES))

from I3Tray import I3Tray
from icecube import icetray, dataio
from icecube.production_histograms import ProductionHistogramModule
from icecube.production_histograms.histogram_modules.simulation.mctree_primary import I3MCTreePrimaryModule
from icecube.production_histograms.histogram_modules.simulation.mctree import I3MCTreeModule
from icecube.production_histograms.histogram_modules.simulation.mcpe_module import I3MCPEModule
from icecube.production_histograms.histogram_modules.simulation.pmt_response import PMTResponseModule
from icecube.production_histograms.histogram_modules.simulation.dom_mainboard_response import InIceResponseModule
from icecube.production_histograms.histogram_modules.simulation.trigger import TriggerModule

from icecube.production_histograms.histograms.simulation.secondary_multiplicity import SecondaryMultiplicity
from icecube.production_histograms.histograms.simulation.noise_occupancy import NoiseOccupancy

tray = I3Tray()

tray.Add("I3Reader", FilenameList = filelist)
tray.Add(ProductionHistogramModule, 
         Histograms = [I3MCTreePrimaryModule,
                       I3MCTreeModule,
                       I3MCPEModule,
                       SecondaryMultiplicity,
                       NoiseOccupancy,
                       PMTResponseModule,
                       InIceResponseModule,
                       TriggerModule],         
         OutputFilename = options.OUTPUT_FN
        )
tray.Execute()
tray.Finish()
