#!/usr/bin/env python

from I3Tray import *
from icecube import icetray, dataio, sim_services
import sys

if len(sys.argv) != 3: 
    print("usage: SplitMCTreeForHybridSimulation.py input_file output_file")
    sys.exit(-1)

tray = I3Tray()
tray.AddModule('I3Reader', 'reader', Filename=sys.argv[1])
tray.AddModule('I3MCTreeHybridSimulationSplitter', 'treesplitter')
tray.AddModule('I3Writer', 'writer', Filename=sys.argv[2])
tray.AddModule('TrashCan', 'can')

tray.Execute()
tray.Finish()
