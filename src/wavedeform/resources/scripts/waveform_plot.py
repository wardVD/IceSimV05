from icecube import icetray, dataclasses, dataio
import Gnuplot, sys, numpy

#
# Plot waveforms from an I3 file, an I3RecoPulseSeries, and the corresponding
# forward convolution of the I3RecoPulseSeries, which should match the waveform
# if all went well.
#
# Usage: waveform_plot.py /path/to/file.i3 [pulse series] [waveforms]
#
# NB: C and D frames *must* be present. Frame object names are given below:
#

wf_name = 'CalibratedWaveforms'
pulseseries = 'WavedeformPulses'

if len(sys.argv) > 2:
	pulseseries = sys.argv[2]
if len(sys.argv) > 3:
	wf_name = sys.argv[3]

gp = Gnuplot.Gnuplot()
gp.gnuplot.write('set key bottom right\n')
gp.gnuplot.write('set ytics nomirror\n')
gp.gnuplot.write('set y2tics auto\n')
gp.set_label('xlabel', 'Time (ns)')
gp.set_label('ylabel', 'ADC Voltage (mV)')
gp.set_label('y2label', 'Pulse Height (PE)')

def wfdata(wf, title=None):
	if len(wf.waveform) == 0:
		return '0'
	return Gnuplot.Data(wf.time + numpy.linspace(0, wf.bin_width*(len(wf.waveform)-1), len(wf.waveform)),-numpy.asarray(wf.waveform)*1e12, with_='steps', title=title)

def pulseseries_to_wf(pulses, template, times, norm, title=None):
	wf = numpy.zeros(len(times))
	for pulse in pulses:
		wf += -pulse.charge*template(times - pulse.time)

	wf *= norm;
	return Gnuplot.Data(times, wf, with_='lines', title=title)

def pulseseries_data(pulses, title=None):
	times = []
	heights = []
	if len(pulses) == 0:
		return '0'

	for pulse in pulses:
		times += [pulse.time - 0.2, pulse.time, pulse.time + 0.2]
		heights += [0, pulse.charge, 0]

	return Gnuplot.Data(times, heights, with_='linespoints', axes='x1y2',
	    title=title)

f = dataio.I3File(sys.argv[1])

i = 0
lastq = None
while True:
	i = i+1
	fr = f.pop_frame()
	if fr.Stop == icetray.I3Frame.DAQ:
		lastq = fr
	if not fr.Has(pulseseries):
		continue
	if lastq != None and fr.Stop == icetray.I3Frame.Physics:
		fr.merge(lastq)
	if not fr.Has(wf_name):
		continue
	calib = fr['I3Calibration']
	status = fr['I3DetectorStatus']
	wfmap = fr[wf_name]
	pulsemap = fr[pulseseries]
	if pulsemap.__class__ == dataclasses.I3RecoPulseSeriesMapMask:
		pulsemap = pulsemap.apply(fr)

	for dom in wfmap.keys():
		print(dom)
		wfs = wfmap[dom]
		try:
			pulses = pulsemap[dom]
		except:
			pulses = dataclasses.I3RecoPulseSeries()
		if len(pulses) == 0:
			continue
		qtot = numpy.sum([pulse.charge for pulse in pulses])

		gp.title('Frame %d, %s - %lf PE, %d pulses' % \
		    (i, dom, qtot, len(pulses)))

		spe_charge = dataclasses.spe_mean(status.dom_status[dom],
		    calib.dom_cal[dom])
		spe_charge *= calib.dom_cal[dom].front_end_impedance
		spe_charge *= 1e12

		times = numpy.linspace(numpy.min([wf.time for wf in wfs]) - 100,
		    numpy.max([wf.time + wf.bin_width*len(wf.waveform)]), 5000)
		gp.set_range('xrange', (times[0], times[-1]))
		plotted = []
		for wf in wfs:
			#if wf.source == dataclasses.I3Waveform.SLC:
			#	continue
			if wf.digitizer == dataclasses.I3Waveform.ATWD:
				diginame = 'ATWD %d' % wf.channel
			else:
				diginame = 'FADC'
			plotted += [wfdata(wf, title=diginame)]
			def template(ts):
				ts = ts + wf.bin_width/2
				if wf.digitizer == dataclasses.I3Waveform.ATWD:
					templ = calib.dom_cal[dom].\
					 atwd_pulse_template(wf.channel, False)
				else:
					templ = calib.dom_cal[dom].\
					 fadc_pulse_template(False)
				return numpy.asarray([templ(t) for t in ts])
			plotted += [pulseseries_to_wf(pulses, template, times,
			    spe_charge, title='Refolded ' + diginame)]
		if len(plotted) == 0:
			continue
		gp.plot(pulseseries_data(pulses, 'Unfolded Pulses'), *plotted)
		gp.interact()

		gp.set_range('xrange', (times[0] + 100, times[0] + 700))
		gp.plot(pulseseries_data(pulses, 'Unfolded Pulses'), *plotted)
		gp.interact()

