#!/usr/bin/env python

from I3Tray import *
import os, random, math
from icecube import icetray, dataio, dataclasses
from icecube import phys_services, wavedeform

tray = I3Tray()
tray.AddModule('I3Reader', 'reader', Filename=os.getenv('I3_BUILD') + '/wavedeform_test.i3')

import unittest
class TimeCheck(unittest.TestCase):
	fakePulseKey = "RandomPulses"
	pulseKey = "WavedeformPulses"

	def testMeanTime(self):
		diffs = []
		for om in self.frame[self.fakePulseKey].keys():
			try:
				meanrecotime = sum([p.time*p.charge for p in \
				    self.frame[self.pulseKey][om]])/ \
				    sum([p.charge for p in \
				    self.frame[self.pulseKey][om]])
				meansimtime = sum([p.time*p.charge for p in \
				    self.frame[self.fakePulseKey][om]])/ \
				    sum([p.charge for p in \
				    self.frame[self.fakePulseKey][om]])
			except ZeroDivisionError:
				continue

			diffs.append(meanrecotime - meansimtime)
			self.assert_(math.fabs(meanrecotime - meansimtime) < 700,
			    ("Mean sim time (%f) and reco time (%f) match to " +
			     "within 10 ns in OM %s") % (meansimtime, \
			    meanrecotime, om))
		self.assert_(math.fabs(sum(diffs)/len(diffs)) < 30,
		    ("Mean mean sim time and reco time not within 30 ns"))

	def testLeadingEdgeTime(self):
		fracdiffs = []
		for om in self.frame[self.fakePulseKey].keys():
			if len(self.frame[self.fakePulseKey][om]) == 0:
				continue

			simfirst = self.frame[self.fakePulseKey][om][0].time
			for pulse in self.frame[self.pulseKey][om]:
				recofirst = pulse.time
				recowidth = pulse.width
				# Get a for-sure real pulse
				if pulse.charge > 0.6: break

			margin = 10 # sigma -- this is a crappy test, so it's
			            # big (better test below)
			frac = math.fabs((recofirst - simfirst)/recowidth)
			fracdiffs.append(frac)
			self.assert_(frac < margin, \
			    ("Sim time (%f) and reco time (%f) match to " +
			     "within %d sigma (%f ns) in OM %s") % (simfirst, \
			    recofirst, margin, margin*recowidth, om))

		self.assert_(sum(fracdiffs)/len(fracdiffs) < 1, \
		    "Simulation and reco times within 1 sigma on average")

tray.AddModule(icetray.I3TestModuleFactory(TimeCheck), 'testy', Streams=[icetray.I3Frame.DAQ])
tray.AddModule('TrashCan','can')
tray.Execute()
tray.Finish()
