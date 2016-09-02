
__all__ = ["fluxes", "weighting"]

import numpy

def get_weighted_primary(frame, MCPrimary='MCPrimary'):
	"""
	Weighted CORSIKA simulation (as well as some NuGen simulation) can have
	coincidences mixed in that should not be used to calculate weights, as they
	were chosen at "natural" frequency. Find the primary that was chosen from a
	biased spectrum, and put it in the frame.
	
	:param MCPrimary: name of the primary particle to put in the frame
	"""
	from weighting import ParticleType
	
	if not 'I3MCTree' in frame or (len(frame['I3MCTree'].primaries) == 0):
		return
	primaries = frame['I3MCTree'].primaries
	if len(primaries) == 1:
		idx = 0
	elif 'I3MCWeightDict' in frame:
		idx = [i for i in range(len(primaries)) if primaries[i].is_neutrino][0]
	elif 'CorsikaWeightMap' in frame:
		wmap = frame['CorsikaWeightMap']
		# Only filter by particle type if we're still using CORSIKA-style
		# codes. This is a horrendous hack that will have to be revisited
		# once PDG-coded simulation becomes more common.
		if dataclasses.I3Particle.PPlus == ParticleType.PPlus:
			if 'PrimaryType' in wmap:
				primaries = [p for p in primaries if p.type==wmap['PrimaryType']]
			elif 'ParticleType' in wmap:
				primaries = [p for p in primaries if p.type==wmap['ParticleType']]
		if len(primaries) == 0:
			return
		elif len(primaries) == 1:
			idx = 0
		elif 'PrimaryEnergy' in wmap:
			prim_e = wmap['PrimaryEnergy']
			idx = int(numpy.argmin([abs(p.energy-prim_e) for p in primaries]))
		elif 'PrimarySpectralIndex' in wmap:
			prim_e = wmap['Weight']**(-1./wmap['PrimarySpectralIndex'])
			idx = int(numpy.argmin([abs(p.energy-prim_e) for p in primaries]))
		else:
			idx = 0
	
	frame[MCPrimary] = primaries[idx]

from icecube import icetray, dataclasses
from icecube.icetray import I3Units

from os.path import expandvars
import cPickle as pickle
class SimprodNormalizations(dict):
	def __init__(self, filename=expandvars("$I3_BUILD/weighting/resources/dbcache.pickle"), mode='a'):
		"""
		:param filename: path to database file (without suffix!)
		"""
		if mode == 'r':
			self.readonly = True
		else:
			self.readonly = False
		self.filename = filename
		try:
			dict.__init__(self, pickle.load(open(self.filename, 'r')))
		except IOError:
			dict.__init__(self)
	def __del__(self):
		self.commit()
	def refresh(self, dataset):
		"""
		Forcibly reload normalization data from the simprod database.
		"""
		dataset = int(dataset)
		if dataset in self:
			del self[dataset]
		return self[dataset]
	def commit(self):
		if not self.readonly:
			pickle.dump(dict(self), open(self.filename, 'w'), protocol=pickle.HIGHEST_PROTOCOL)
	def __getitem__(self, dataset):
		dataset = int(dataset)
		if not dataset in self:
			if self.readonly:
				raise KeyError('Dataset %s is not in the cache. Pre-populate the cache (%s) before using it non-interactively.' % (dataset, self.filename))
			from icecube.weighting.weighting import from_simprod
			self[dataset] = from_simprod(int(dataset))
		return dict.__getitem__(self, dataset)


class WeightCalculatorBase(icetray.I3ConditionalModule):
	def __init__(self, context):
		icetray.I3ConditionalModule.__init__(self, context)
		self.AddParameter("Dataset", "Number of Simprod dataset, or a pre-constructed GenerationProbabilityCollection", None)
		self.AddParameter("NFiles", "Number of files in the dataset", 1)
		self.AddParameter("Cache", "Path to a normalization cache file", expandvars("$I3_BUILD/weighting/resources/dbcache.pickle"))
	
	def get_weight(self, frame, primary):
		raise NotImplementedError
	
	def Configure(self):
		from icecube.weighting.weighting import GenerationProbabilityCollection
		dataset = self.GetParameter("Dataset")
		if dataset is None:
			icetray.logging.log_fatal("Unknown dataset ID %s" % dataset)
		elif isinstance(dataset, GenerationProbabilityCollection):
			self.norm = dataset
		else:
			try:
				cache = SimprodNormalizations(self.GetParameter("Cache"), 'r')
			except NameError:
				raise ImportError("Couldn't import sqlitedict; the weighting modules will not be functional.")
			self.norm = cache[dataset]
		self.norm = self.norm*self.GetParameter("NFiles")
		
		self.flux = self.GetParameter("Flux")
		if self.flux is None:
			raise ValueError("You must supply a flux parametrization")
	
	def DAQ(self, frame):
		if not 'MCPrimary' in frame:
			get_weighted_primary(frame)
		if not 'MCPrimary' in frame:
			self.PushFrame(frame)
			return
		primary = frame['MCPrimary']
		frame[self.name] = dataclasses.I3Double(self.get_weight(frame, primary))
		
		self.PushFrame(frame)

class CORSIKAWeightCalculator(WeightCalculatorBase):
	"""
	Calculate weights for air shower events
	"""
	def __init__(self, context):
		super(CORSIKAWeightCalculator, self).__init__(context)
		
		from icecube.weighting.fluxes import GaisserH3a
		self.AddParameter("Flux", "Primary cosmic ray flux model", GaisserH3a())
	
	def get_weight(self, frame, primary):
		norm = self.norm(primary.energy, primary.type)
		if norm == 0:
			return 0.
		else:
			return self.flux(primary.energy, primary.type)/norm

class WeightCalculator(CORSIKAWeightCalculator):
	def __init__(self, context):
		super(WeightCalculator, self).__init__(context)
		raise DeprecationWarning("The WeightCalculator module has been replaced by the CORSIKAWeightCalculator module. Please update your code.")

class NuGenWeightCalculator(WeightCalculatorBase):
	"""
	Calculate weights neutrino events from NeutrinoGenerator
	"""
	def __init__(self, context):
		super(NuGenWeightCalculator, self).__init__(context)
		try:
			from icecube import NewNuFlux
			flux = NewNuFlux.makeFlux('honda2006')
		except ImportError:
			flux = None
		self.AddParameter("Flux", "Neutrino flux model: an object with a member function getFlux(particleType, energy, cos_theta) that returns fluxes per particle type in 1/(GeV cm^2 sr s) (e.g. an instance of NewNuFlux.FluxFunction)", flux)
	
	def get_weight(self, frame, primary):
		p_int = frame['I3MCWeightDict']['TotalInteractionProbabilityWeight']
		norm = self.norm(primary.energy, primary.type, numpy.cos(primary.dir.zenith))*I3Units.cm2
		if norm == 0:
			return 0.
		else:
			return p_int*self.flux.getFlux(primary.type, primary.energy, numpy.cos(primary.dir.zenith))/norm
