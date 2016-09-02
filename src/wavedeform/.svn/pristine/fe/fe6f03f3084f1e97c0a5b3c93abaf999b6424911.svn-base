#!/usr/bin/env python

from I3Tray import *
import os, random, math, sys
from icecube import icetray, dataio, dataclasses
from icecube import phys_services, wavedeform
from os.path import expandvars

if os.environ.get("I3_TESTDATA") :
    i3_testdata = expandvars("$I3_TESTDATA") 
else :
    i3_testdata = expandvars("$I3_TESTDATA")

tray = I3Tray()
tray.AddModule('I3InfiniteSource', 'reader', Prefix = i3_testdata + '/sim/GCD_String21.i3.gz')

if sys.version_info[0] >= 3:
	random.seed(0)
else:
	random.seed(5)
def RandomWaveforms(fr):
	calib = fr['I3Calibration']
	status = fr['I3DetectorStatus']

	pulsemap = dataclasses.I3RecoPulseSeriesMap()
	wfmap = dataclasses.I3WaveformSeriesMap()

	for om in calib.dom_cal.keys():
		pulses = dataclasses.I3RecoPulseSeries()
		waveforms = dataclasses.I3WaveformSeries()

		fadc_templ = calib.dom_cal[om].fadc_pulse_template(False)
		atwd0_templ = calib.dom_cal[om].atwd_pulse_template(0, False)

		spe_charge = dataclasses.spe_mean(status.dom_status[om],
		    calib.dom_cal[om])
		if spe_charge < I3Units.pC: continue
		spe_charge *= calib.dom_cal[om].front_end_impedance

		for launch in range(0, random.randint(0, 4)):
			npulses = random.randint(0, 5)
			launchtime = launch*10000
			# Make 30% of SPE launches SLC
			slc = (npulses == 1 and random.uniform(0,1) < 0.3)

			# ATWD Waveform
			atwd0_wf = dataclasses.I3Waveform()
			atwd0_wf.waveform = [ \
			    random.normalvariate(0, 0.3)*I3Units.mV for \
			    i in range(0, 128)]
			atwd0_wf.digitizer = dataclasses.I3Waveform.ATWD
			atwd0_wf.bin_width = 3.3
			atwd0_wf.hlc = not slc
			atwd0_wf.time = launchtime

			# FADC Waveform
			if slc:
				fadc_nbins = 3
			else:
				fadc_nbins = 256
			fadc_wf = dataclasses.I3Waveform()
			fadc_wf.waveform = [ \
			    random.normalvariate(0, 0.1)*I3Units.mV for \
			    i in range(0, fadc_nbins)]
			fadc_wf.digitizer = dataclasses.I3Waveform.FADC
			fadc_wf.bin_width = 25
			fadc_wf.hlc = not slc
			fadc_wf.time = launchtime

			for p in range(0, npulses):
				pulse = dataclasses.I3RecoPulse()
				pulse.charge = random.randint(1, 3)
				if not slc:
					pulse.time = launchtime + \
					    random.gammavariate(2.5, 80)
					pulse.flags = pulse.PulseFlags.LC
				else:
					pulse.time = launchtime + \
					    random.uniform(-25, 25)
				pulses.append(pulse)

				norm = spe_charge * pulse.charge
				for i in range(0, len(fadc_wf.waveform)):
					fadc_wf.waveform[i] += norm * \
					    fadc_templ((i+1)*fadc_wf.bin_width - \
					    (pulse.time - launchtime))
				for i in range(0, len(atwd0_wf.waveform)):
					atwd0_wf.waveform[i] += norm * \
					    atwd0_templ((i+1)*atwd0_wf.bin_width - \
					    (pulse.time - launchtime))

			waveforms.append(fadc_wf)
			if not slc:
				waveforms.append(atwd0_wf)
		wfmap[om] = waveforms
		pulsemap[om] = dataclasses.I3RecoPulseSeries(sorted(pulses,
		    key=lambda pulse: pulse.time))
	fr['RandomPulses'] = pulsemap
	fr['CalibratedWaveforms'] = wfmap
	fr['CalibratedWaveformRange'] = dataclasses.I3TimeWindow(0,10000)
				
	
tray.AddModule(RandomWaveforms, 'random', Streams=[icetray.I3Frame.DAQ])
tray.AddModule('I3Wavedeform', 'deform')

import unittest
class SanityCheck(unittest.TestCase):
	fakePulseKey = "RandomPulses"
	pulseKey = "WavedeformPulses"

	def testKeys(self):
		self.assert_(self.pulseKey in self.frame, "The pulses actually show up in the frame.")
	def testPositiveWidth(self):
		psm = self.frame[self.pulseKey]
		for om, vec in psm:
			for p in vec:
				self.assert_(p.width >= 0, "Pulse width (%g) is non-negative" % p.width)
	def testPulseSorting(self):
		psm = self.frame[self.pulseKey]
		for om, pulses in psm:
			for prev, next in zip(pulses[:-1], pulses[1:]):
				self.assert_(next.time - prev.time > 0, "Pulses at t=%g and %g are strictly ordered in time" % (prev.time, next.time))
	def testTimeRange(self):
		range = self.frame[self.pulseKey + 'TimeRange']
		sourcerange = self.frame['CalibratedWaveformRange']
		self.assert_(range.stop == sourcerange.stop, "Time ranges stop at the same time (%f, %f)" % (range.stop, sourcerange.stop))
		self.assert_(range.start == sourcerange.start - 25*I3Units.ns, "Time range starts 25 ns before waveforms (%f, %f)" % (range.start, sourcerange.start))

tray.AddModule(icetray.I3TestModuleFactory(SanityCheck), 'testy', Streams=[icetray.I3Frame.DAQ])
tray.AddModule('I3Writer', 'writer', Filename=os.getenv('I3_BUILD') + '/wavedeform_test.i3')
tray.AddModule('TrashCan', 'can')
tray.Execute(6)
tray.Finish()
