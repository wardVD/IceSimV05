from I3Tray import *
import sys
from icecube import icetray, dataio, dataclasses, WaveCalibrator
load('wavedeform')

# Deconvolves SPE pulses from waveforms in an I3 file.
#
# Usage: wavedeform-test.py output.i3 input1.i3 input2.i3 ...

i = 0
def count(frame):
	global i
	i = i+1
	print('Frame: %d (%d OMs)' % (i, len(frame['CalibratedWaveforms'])))

def done(frame):
	print('\tDone')

files = sys.argv[2:]
files.sort()

tray = I3Tray()
tray.AddModule('I3Reader', 'reader', FilenameList=files)
tray.AddModule('I3WaveCalibrator', 'domcal')
tray.AddModule(count, 'count', Streams=[icetray.I3Frame.DAQ])
tray.AddModule('I3Wavedeform', 'deform')
tray.AddModule(done, 'done', Streams=[icetray.I3Frame.DAQ])
tray.AddModule('I3Writer', 'writer', filename=sys.argv[1],
	Streams = [icetray.I3Frame.Calibration, icetray.I3Frame.DetectorStatus,
		   icetray.I3Frame.DAQ])
tray.AddModule('TrashCan','can')
tray.Execute()
tray.Finish()


