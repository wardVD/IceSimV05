#!/usr/bin/env python

"""
Calculate weights for 5-component CORSIKA simulation.
"""

from icecube import icetray, dataclasses, dataio
from icecube.hdfwriter import I3HDFWriter
from I3Tray import I3Tray

from argparse import ArgumentParser
parser = ArgumentParser(description=__doc__)
parser.add_argument('infile', nargs='*')
parser.add_argument('outfile')
parser.add_argument('--dataset', type=int, default=10285, help="Dataset number [%(default)s]")
opts = parser.parse_args()

tray = I3Tray()

tray.AddModule('I3Reader', 'reader', filenamelist=opts.infile)

from icecube.weighting import CORSIKAWeightCalculator
from icecube.weighting import fluxes
tray.AddModule(CORSIKAWeightCalculator, 'GaisserH3aWeight', Dataset=opts.dataset, Flux=fluxes.GaisserH3a())

tray.AddSegment(I3HDFWriter, 'scribe',
    Output=opts.outfile,
    Keys=['MCPrimary', 'CorsikaWeightMap', 'GaisserH3aWeight'],
    Types=[],
    SubEventStreams=['in_ice'],
)

tray.AddModule('TrashCan', 'YesWeCan')
tray.Execute()
tray.Finish()