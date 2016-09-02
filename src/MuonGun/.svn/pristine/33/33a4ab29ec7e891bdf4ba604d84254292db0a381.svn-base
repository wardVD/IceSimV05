#!/usr/bin/env python

from icecube import icetray, dataclasses, dataio, MuonGun
from I3Tray import I3Tray

import sys
infiles = sys.argv[1:-1]
outfile = sys.argv[-1]

icetray.logging.set_level_for_unit("MuonGun", "INFO")
icetray.logging.set_level_for_unit("I3MuonGun::WeightCalculator", "TRACE")

def harvest_generators(infiles):
	"""
	Harvest serialized generator configurations from a set of I3 files.
	"""
	from icecube.icetray.i3logging import log_info as log
	generator = None
	for fname in infiles:
		f = dataio.I3File(fname)
		fr = f.pop_frame(icetray.I3Frame.Stream('S'))
		f.close()
		if fr is not None:
			for k in fr.keys():
				v = fr[k]
				if isinstance(v, MuonGun.GenerationProbability):
					log('%s: found "%s" (%s)' % (fname, k, type(v).__name__), unit="MuonGun")
					if generator is None:
						generator = v
					else:
						generator += v
	return generator

def book_weights(infiles, outfile, model='Hoerandel5_atmod12_SIBYLL'):
	tray = I3Tray()
	
	tray.AddModule('I3Reader', 'reader', filenamelist=infiles)
	tray.AddModule('I3NullSplitter', 'nullsplit')
	
	tray.AddModule(lambda frame: frame.Put('MCPrimary', frame['I3MCTree'].primaries[0]))
	tray.AddModule(lambda frame: frame.Put('Muon', frame['I3MCTree'].most_energetic_muon))
	
	model = MuonGun.load_model(model)
	generator = harvest_generators(infiles)
	tray.AddModule('I3MuonGun::WeightCalculatorModule', 'MuonWeight', Model=model, Generator=generator)
	
	from icecube.hdfwriter import I3HDFWriter
	tray.AddSegment(I3HDFWriter, 'scribe',
	    Output=outfile,
	    Keys=['MCPrimary', 'Muon', 'MuonWeight',
	        dict(key='I3MCTree', name='BundleParameters',
	             converter=MuonGun.converters.MuonBundleConverter(1, generator.surface))],
	    Types=[],
	    SubEventStreams=['nullsplit'],
	)
	
	tray.AddModule('TrashCan', 'YesWeCan')
	tray.Execute()
	tray.Finish()

book_weights(infiles, outfile)

try:
	import tables, pylab, numpy
	
	hdf = tables.openFile(outfile)
	
	# We can get pre-calculated weights calcuated by the icetray module
	muon_energy = hdf.root.Muon.cols.energy[:]
	pre_weights = hdf.root.MuonWeight.cols.value[:]
	
	# or make a freestanding WeightCalculator with a different flux model
	model = MuonGun.load_model('GaisserH4a_atmod12_SIBYLL')
	generator = harvest_generators(infiles)
	weighter = MuonGun.WeightCalculator(model, generator)
	
	# and use the bundle axis and its radius/energy distribution to calculate a weight
	axis = hdf.root.MCPrimary.read()
	bundle = hdf.root.BundleParameters.read()
	post_weights = weighter(axis['x'], axis['y'], axis['z'], axis['zenith'], axis['azimuth'],
	    bundle['multiplicity'], bundle['energy'], bundle['radius'])
	
	pylab.hist(muon_energy, bins=numpy.logspace(4, 7, 101), log=True, histtype='bar', label='unweighted (counts)')
	pylab.hist(muon_energy, weights=post_weights, bins=numpy.logspace(4, 7, 101), log=True, histtype='bar', label='GaisserH4a')
	pylab.hist(muon_energy, weights=pre_weights, bins=numpy.logspace(4, 7, 101), log=True, histtype='bar', label='Hoerandel')
	
	pylab.legend(loc='lower left')
	pylab.xlabel('Muon energy at sampling surface')
	pylab.ylabel('Rate [Hz]')
	pylab.loglog()
	
	pylab.show()
except ImportError:
	pass

