#!/usr/bin/env python

"""
Calculate weights for NeutrinoGenerator simulation with NewNuFlux.
"""

from icecube import icetray, dataclasses, dataio
from icecube.hdfwriter import I3HDFWriter
from I3Tray import I3Tray

from argparse import ArgumentParser
parser = ArgumentParser(description=__doc__)
parser.add_argument('infile', nargs='*')
parser.add_argument('outfile')
parser.add_argument('--dataset', type=int, default=11029, help="Dataset number [%(default)s]")
parser.add_argument('--flux', type=str, default='honda2006', help="NewNuFlux flux model [%(default)s]")
opts = parser.parse_args()

try:
    from icecube import NewNuFlux
except ImportError:
    raise ImportError('Atmospheric flux weighting requires NewNuFlux. Get it from http://code.icecube.wisc.edu/svn/sandbox/cweaver/NewNuFlux')

tray = I3Tray()

tray.AddModule('I3Reader', 'reader', filenamelist=opts.dataset)

from icecube.weighting import NuGenWeightCalculator
tray.AddModule(NuGenWeightCalculator, 'HondaWeight', Dataset=opts.dataset, Flux=NewNuFlux.makeFlux('honda2006'))

tray.AddSegment(I3HDFWriter, 'scribe',
    Output=opts.outfile,
    Keys=['MCPrimary', 'I3MCWeightDict', 'HondaWeight'],
    Types=[],
    SubEventStreams=['in_ice'],
)

tray.AddModule('TrashCan', 'YesWeCan')
tray.Execute()
tray.Finish()
