#!/usr/bin/env python

from I3Tray import *
import os, random, math
from icecube import icetray, dataio, dataclasses
from icecube import phys_services, wavedeform

tray = I3Tray()
tray.AddModule('I3Reader', 'reader', Filename=os.getenv('I3_BUILD') + '/wavedeform_test.i3')

import unittest
class QTotCheck(unittest.TestCase):
	fakePulseKey = "RandomPulses"
	pulseKey = "WavedeformPulses"

	def testChargeConservation(self):
		fracdiffs = []
		for om in self.frame[self.fakePulseKey].keys():
			simcharge = sum([p.charge for p in \
			    self.frame[self.fakePulseKey][om]])
			recocharge = sum([p.charge for p in \
			    self.frame[self.pulseKey][om]])

			if simcharge == 0:
				basecharge = recocharge
			else:
				basecharge = simcharge
			if basecharge == 0:
				continue

			margin = 0.50
			frac = math.fabs((simcharge - recocharge)/basecharge)
			fracdiffs.append(frac)
			self.assert_(frac < margin, \
			    ("Sim charge (%f) and reco charge (%f) match to " +
			     "within %d%% (OM %s)") % (simcharge, recocharge, \
			    margin*100, om))

		self.assert_(sum(fracdiffs)/len(fracdiffs) < 0.03, \
		    "Simulation and reco charges within 3% on average")

tray.AddModule(icetray.I3TestModuleFactory(QTotCheck), 'testy', Streams=[icetray.I3Frame.DAQ])
tray.AddModule('TrashCan','can')
tray.Execute()
tray.Finish()
