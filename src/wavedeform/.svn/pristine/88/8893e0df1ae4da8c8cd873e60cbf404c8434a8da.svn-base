from icecube.load_pybindings import load_pybindings
load_pybindings(__name__, __path__)
del load_pybindings

from icecube import dataclasses, icetray

class AddMissingTimeWindow(icetray.I3ConditionalModule):
	'''Add I3TimeWindow for pulse series to old data made with versions of wavedeform that do not write this to the frame'''
	def __init__(self, context):
		icetray.I3ConditionalModule.__init__(self, context)
		self.AddOutBox("OutBox")
		self.AddParameter("WaveformTimeRange", "Name of time range from WaveCalibrator", "CalibratedWaveformRange")
		self.AddParameter("Pulses", "Name of pulse series map to write time range for", "OfflinePulses")
	def Configure(self):
		self.TimeRange = self.GetParameter("WaveformTimeRange")
		self.Pulses = self.GetParameter("Pulses")
	def DAQ(self, fr):
		fr[self.Pulses + 'TimeRange'] = dataclasses.I3TimeWindow(fr[self.TimeRange].start - 25.*icetray.I3Units.ns, fr[self.TimeRange].stop)
		self.PushFrame(fr)
	
