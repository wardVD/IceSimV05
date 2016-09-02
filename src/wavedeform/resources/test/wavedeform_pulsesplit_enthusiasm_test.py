#!/usr/bin/env python

from I3Tray import *
import os, random, math
from icecube import icetray, dataio, dataclasses
from icecube import phys_services, wavedeform

tray = I3Tray()
tray.AddModule('I3Reader', 'reader', Filename=os.getenv('I3_BUILD') + '/wavedeform_test.i3')

import unittest
class NPulsesCheck(unittest.TestCase):
	fakePulseKey = "RandomPulses"
	pulseKey = "WavedeformPulses"

	# Test that pulse merging/splitting are not over-enthusiastic and
	# the number of reco pulses is at least similar to the number of sim
	# pulses. Note that this is not important, nor guaranteed, but tends
	# to break if the algorithm is broken for other reasons.
	def testNPulsesSane(self):
		fracdiffs = []
		for om in self.frame[self.fakePulseKey].keys():
			nsim = len(self.frame[self.fakePulseKey][om])
			nreco = len(self.frame[self.pulseKey][om])

			denom = nreco
			if denom == 0:
				denom = nsim
			if denom == 0:
				frac = 0
			else:
				frac = math.fabs((float(nreco) - nsim)/denom)
			margin = 0.60
			fracdiffs.append(frac)
			self.assert_(frac < margin, \
			    ("Sim npulses (%d) and reco (%d) match to " +
			     "within %d%% in OM %s") % (nsim, \
			    nreco, margin*100, om))

		print(sum(fracdiffs)/len(fracdiffs))
		self.assert_(sum(fracdiffs)/len(fracdiffs) < 0.15, \
		    "Simulation and reco N pulses within 15% on average")

tray.AddModule(icetray.I3TestModuleFactory(NPulsesCheck), 'testy', Streams=[icetray.I3Frame.DAQ])
tray.AddModule('TrashCan','can')
tray.Execute()
tray.Finish()
