#!/usr/bin/env python

from icecube import icetray, dataio, WaveCalibrator
import I3Tray

import sys

infiles = sys.argv[1:-1]
outfile = sys.argv[-1]

tray = I3Tray.I3Tray()

# For this script to work correctly GCD frames need to
# be part of the input.
tray.AddModule("I3Reader", "reader", filenamelist=infiles)

# This module calibrates the raw waveforms in "InIceRawData"
# The resulting calibrated waveforms are in "CalibratedWaveforms"
tray.AddModule("I3WaveCalibrator", "calibrator",
	Launches="InIceRawData",
	Waveforms="CalibratedWaveforms",
	ATWDSaturationMargin=123, # 1023-900 == 123
	FADCSaturationMargin=0,
	)
	
tray.AddModule("I3Writer", "writer", filename=outfile)

tray.Execute()
tray.Finish()
