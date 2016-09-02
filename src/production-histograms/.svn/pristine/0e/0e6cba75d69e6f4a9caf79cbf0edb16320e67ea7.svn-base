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
from icecube.production_histograms.histograms.filtering.common_variables import cv_histograms
from icecube.production_histograms.histograms.filtering.particles import l1_particle_histograms

tray = I3Tray()

tray.Add("I3Reader", FilenameList = filelist)
tray.Add(ProductionHistogramModule, 
         Histograms = cv_histograms + l1_particle_histograms,
         OutputFilename = options.OUTPUT_FN
        )
tray.Execute()
tray.Finish()
