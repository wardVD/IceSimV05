from __future__ import print_function

import numpy
import copy
import itertools
import warnings

from icecube.icetray import I3Units
from .enum import enum

# some enums for CORSIKA->PDG compatibility
class ParticleType(enum):
	PPlus       =   14
	He4Nucleus  =  402
	N14Nucleus  = 1407
	O16Nucleus  = 1608
	Al27Nucleus = 2713
	Fe56Nucleus = 5626
	NuE         =   66
	NuEBar      =   67
	NuMu        =   68
	NuMuBar     =   69
	NuTau       =  133
	NuTauBar    =  134

class PDGCode(enum):
	PPlus       =       2212
	He4Nucleus  = 1000020040
	N14Nucleus  = 1000070140
	O16Nucleus  = 1000080160
	Al27Nucleus = 1000130270
	Fe56Nucleus = 1000260560
	NuE         =         12
	NuEBar      =        -12
	NuMu        =         14
	NuMuBar     =        -14
	NuTau       =         16
	NuTauBar    =        -16

PDGCode.from_corsika = classmethod(lambda cls, i: getattr(cls, ParticleType.values[i].name))

class GenerationProbability(object):
	"""
	A probability distribution from which MC events are drawn.
	
	Generation probabilities may be multiplied by an integer to express
	a joint generation probability for multiple identical simulation runs
	or added together to express a set of distinct runs.
	"""
	def __init__(self, emin, emax, nevents=1, area=1, particle_type=None):
		"""
		:param emin: minimum energy for which this probability is non-zero
		:param emax: maximum energy for which this probability is non-zero
		:param nevents: number of generated events
		:param area: integrated area-times-solid-angle of the sampling surface
		:param particle_type: particle type for which this probability is valid. if None,
		                      then it is valid for all particle types.
		"""
		self.emin = emin
		self.emax = emax
		self.nevents = nevents
		self.area = area
		self.particle_type = particle_type
	
	def to_PDG(self):
		"Translate to a form that understands PDG codes instead of CORSIKA"
		if self.particle_type is not None and getattr(PDGCode, self.particle_type.name) == self.particle_type:
			return self
		new = copy.copy(self)
		if new.particle_type is not None:
			new.particle_type = PDGCode.from_corsika(new.particle_type)
		return new
	
	def __imul__(self, factor):
		self.nevents *= factor
		return self
	
	def __mul__(self, factor):
		t = copy.copy(self)
		t *= factor
		return t
	
	def __rmul__(self, factor):
		return self*factor
	
	def __idiv__(self, factor):
		self.nevents /= factor
		return self
		
	def __div__(self, factor):
		t = copy.copy(self)
		t /= factor
		return t
		
	def __iadd__(self, other):
		if isinstance(other, type(self)):
			if self.is_compatible(other):
				self.nevents += other.nevents
				return self
			else:
				return GenerationProbabilityCollection([self, other])
		else:
			raise TypeError("Can't add a %s to this %s" % (type(other).__name__, type(self).__name__))
	
	def __add__(self, other):
		if isinstance(other, type(self)):
			if self.is_compatible(other):
				t = copy.copy(self)
				t += other
				return t
			else:
				return GenerationProbabilityCollection([self, other])
		else:
			return GenerationProbabilityCollection([self, other])
	
	def __call__(self, E, particle_type=None, cos_theta=None):
		"""
		Calculate the generated fluence of particles in this simulation scheme
		
		:param E: the energy of particle in GeV
		:param particle_type: the particle type for which to return a fluence
		:returns: a fluence in units of :math:`m^{-2} sr^{-1} GeV^{-1}`
		"""
		E = numpy.asarray(E)
		if hasattr(self.area, "__call__"):
			area = self.area(cos_theta)
		else:
			area = self.area
		if numpy.ndim(E) == 0:
			if (E>=self.emin and E<self.emax and (particle_type is None or particle_type == self.particle_type)):
				return self.nevents*self.generation_probability(E)/area
			else:
				return 0
		else:
			mask = (E>=self.emin)&(E<self.emax)
			if particle_type is not None:
				mask &= (particle_type==self.particle_type)
			return numpy.where(mask, self.nevents*self.generation_probability(E)/area, 0)
	
	def is_compatible(self, other):
		raise NotImplementedError("should be overridden")
	
	def __eq__(self, other):
		return self.nevents == other.nevents and self.is_compatible(other)
	
	def __ne__(self, other):
		return not (self == other)
	
	def generation_probability(self, E):
		raise NotImplementedError("should be overridden")

class Null(object):
	"""
	An identity object, useful as a starting point for accumulators, e.g.::
		total = Null()
		for i in xrange(10):
			total += SomeClassThatImplementsAddition(i)
	"""
	def __add__(self, other):
		return other
	def __radd__(self, other):
		return other
	def __iadd__(self, other):
		return other
	def __mul__(self, f):
		return self
	def __rmul__(self, f):
		return self
	def __imul__(self, f):
		return self
	def __eq__(self, other):
		return isinstance(other, Null) or 0 == other
	def __ne__(self, other):
		return not (self == other)

class GenerationProbabilityCollection(object):
	"""
	A collection of generation spectra, possibly for different particle types.
	"""
	def __init__(self, spectra):
		"""
		:param spectra: a collection of GenerationProbabilities.
		"""
		from collections import defaultdict
		self.spectra = defaultdict(list)
		for dist in spectra:
			self.spectra[dist.particle_type].append(dist)
	
	def to_PDG(self):
		spectra = []
		for v in self.spectra.values():
			spectra += [s.to_PDG() for s in v]
		return self.__class__(spectra)
		
	def __call__(self, E, particle_type=None, cos_theta=None):
		if particle_type is None:
			return sum([prob(E, cos_theta=cos_theta) for spectra in self.spectra.itervalues() for prob in spectra])
		else:
			if numpy.ndim(particle_type) == 0:
				return sum([prob(E, cos_theta=cos_theta) for prob in self.spectra[particle_type]])
			else:
				E = numpy.asarray(E)
				count = numpy.zeros(E.shape)
				for ptype in numpy.unique(particle_type):
					mask = particle_type==ptype
					if numpy.any(mask):
						Em = E[mask]
						if cos_theta is not None:
							ctm = cos_theta[mask]
						else:
							ctm = cos_theta
						count[mask] += sum([prob(Em, cos_theta=ctm) for prob in self.spectra[ptype]])
			return count
	
	def __imul__(self, factor):
		for spectra in self.spectra.itervalues():
			for prob in spectra:
				prob *= factor
		return self
	
	def __idiv__(self, factor):
		self *= (1./factor)
		return self
	
	def __mul__(self, factor):
		clone = copy.deepcopy(self)
		clone *= factor
		return clone
	
	def __div__(self, factor):
		return self*(1./factor)
	
	def __rmul__(self, factor):
		return self*factor
	
	def __iadd__(self, other):
		if isinstance(other, type(self)):
			for pt, ospectra in other.spectra.iteritems():
				for ospec in ospectra:
					for spec in self.spectra[pt]:
						if spec.is_compatible(ospec):
							spec += ospec
							break
					else:
						self.spectra[pt].append(ospec)
			return self
		else:
			for spec in self.spectra[other.particle_type]:
				if spec.is_compatible(other):
					spec += other
					break
			else:
				self.spectra[other.particle_type].append(other)
			return self
	
	def __add__(self, other):
		t = copy.copy(self)
		t += other
		return t
	
	def __eq__(self, other):
		# must handle the same set of particle types
		if set(self.spectra.keys()) != set(other.spectra.keys()):
			return False
		for k in self.spectra:
			s1 = self.spectra[k]
			s2 = other.spectra[k]
			# must have the same number of unique spectra
			if len(s1) != len(s2):
				return False
			# exactly one match for each spectrum
			for p1 in s1:
				if sum(p1 == p2 for p2 in s2) != 1:
					return False
		return True
	
	def __ne__(self, other):
		return not self == other
	
class PowerLaw(GenerationProbability):
	"""
	Power-law spectra are easy.
	"""
	def __init__(self, eslope, *args, **kwargs):
		"""
		:param eslope: index of the power law :math:`E^{-\gamma}`
		"""
		super(PowerLaw, self).__init__(*args, **kwargs)
		self.eslope = eslope
		self.gen_norm = self.norm(self.emin, self.emax, self.eslope)
	
	def subset(self, emin, emax):
		"""
		Return a copy with the same normalization but a smaller range
		"""
		if emin < self.emin or emax > self.emax:
			raise ValueError("Energy range must be a subset of parent energy range")
		fraction = self.norm(emin, emax, self.eslope)/self.gen_norm
		new = copy.copy(self)
		new.nevents *= fraction
		return new
	
	@staticmethod
	def norm(emin, emax, eslope):
		if eslope < -1:
			g = eslope+1
			return (emax**g - emin**g)/g
		else:
			return numpy.log(emax/emin)
	
	def __repr__(self):
		return "PowerLaw(%.2f, emin=%.2e, emax=%.2e, nevents=%.2e)" % (self.eslope, self.emin, self.emax, self.nevents)
	
	def generation_probability(self, E):
		return E**(self.eslope)/self.gen_norm
	
	def invert(self, p):
		"""
		Return CDF^{-1}(p)
		"""
		return (p*(self.emax**(self.eslope+1) - self.emin**(self.eslope+1)) + self.emin**(self.eslope+1))**(1./(self.eslope+1))
	
	def is_compatible(self, other):
		if isinstance(other, type(self)):
			return self.emin == other.emin and self.emax == other.emax and self.eslope == other.eslope and self.particle_type == other.particle_type
		else:
			return False

class HoerandelComponent(GenerationProbability):
	"""
	A power law with a rigidity-dependent knee.
	"""
	def __init__(self, z, eslope, *args, **kwargs):
		"""
		:param z: charge of the p
		
		"""
		super(HoerandelComponent, self).__init__(*args, **kwargs)
		self.z = z
		self.gamma = eslope
		self.gen_norm = self.fluxsum(self.emin, self.emax, z, self.gamma)
	
	def generation_probability(self, E):
		return self.fluxdiff(E, self.z, self.gamma)/self.gen_norm
	
	def is_compatible(self, other):
		if isinstance(other, type(self)):
			return self.emin == other.emin and self.emax == other.emax and self.gamma == other.gamma and self.particle_type == other.particle_type and self.z == other.z
		else:
			return False
	
	@staticmethod	
	def fluxdiff(e, z, gamma, delta_gamma=2.1, eps_cutoff=1.9, E_knee=4.49*I3Units.PeV):
		"""
		Differential (unnormalized) Hoerandel flux
		"""
		return e**(-gamma)*(1+(e/(E_knee*z))**eps_cutoff)**(-delta_gamma/eps_cutoff)
	
	@staticmethod
	def fluxsum(emin, emax, z, gamma, delta_gamma=2.1, eps_cutoff=1.9, E_knee=4.49*I3Units.PeV):
		"""
		Integral Hoerandel flux
		"""
		# the Gauss hypergeometric function. whee!
		from scipy.special import hyp2f1
		antideriv = lambda e: ((e**(1-gamma))/(1-gamma))*hyp2f1(delta_gamma/eps_cutoff, (1-gamma)/eps_cutoff, (1-gamma)/eps_cutoff+1, -(e/(E_knee*z))**eps_cutoff)
		return antideriv(emax) - antideriv(emin)

def FiveComponent(nevents, emin, emax, normalization=[10., 5., 3., 2., 1.], gamma=[-2.]*5,
    LowerCutoffType='EnergyPerNucleon', UpperCutoffType='EnergyPerParticle', height=1600, radius=800):
	"""
	Construct a normalization term for 5-component dCORSIKA simulation.
	
	:param normalization: relative normalizations of the 5 components
	:param gamma: power law index for each component
	:param LowerCutoffType: 
	:param spric: make lower energy proportional to primary mass
	:param height: full height of the target cylinder in meters
	:param radius: radius of the target cylinder in meters
	
	.. note:: The return value of the GenerationProbability will be in units of :math:`GeV^{-1} sr^{-1} m^{-2}`
	"""
	pt = [getattr(ParticleType, p) for p in ('PPlus', 'He4Nucleus', 'N14Nucleus', 'Al27Nucleus', 'Fe56Nucleus')]
	if LowerCutoffType=='EnergyPerParticle':
		lower_energy_scale = [1.]*5
	else:
		lower_energy_scale = [[1, p/100][p>100] for p in pt]
	if UpperCutoffType=='EnergyPerParticle':
		upper_energy_scale = [1.]*5
	else:
		upper_energy_scale = [[1, p/100][p>100] for p in pt]
	# DCORSIKA does this integral in TeV, so we have to do so as well.
	fluxsums = numpy.array([n*PowerLaw.norm(emin*mlo/I3Units.TeV, emax*mhi/I3Units.TeV, g) for mlo, mhi, g, n in zip(lower_energy_scale, upper_energy_scale, gamma, normalization)])
	nshower = nevents*fluxsums/fluxsums.sum()
	area = numpy.pi**2*radius*(radius+height)
	return GenerationProbabilityCollection([PowerLaw(g, emin*mlo, emax*mhi, nevents=n, area=area, particle_type=p) for mlo, mhi, g, n, p in zip(lower_energy_scale, upper_energy_scale, gamma, nshower, pt)]).to_PDG()

def Hoerandel(nevents, emin, emax, dslope=0, height=1600, radius=800):
	"""
	Construct a normalization term for dCORSIKA simulation run in RANPRI=2 mode,
	where it produces a "natural" spectrum with all elements from H to Fe
	according to the parameterizaton of Hoerandel_. In order to combine this
	with 5-component simulation we use only H, He, N, Al, and Fe, returning a
	generation probability of 0 for all other elements.
	
	:param dslope: multiply all fluxes by E^dslope
	:param height: full height of the target cylinder in meters
	:param radius: radius of the target cylinder in meters
	
	.. note:: The return value of the GenerationProbability will be in units of :math:`GeV^{-1} sr^{-1} m^{-2}`
	"""
	# Ripped from dCORSIKA source (with RANPRI=2)
	mass_number = numpy.round([1.00797, 4.0026, 6.939, 9.0122, 10.811, 12.0112, 14.0067, 15.9994, 18.9984, 20.183, 22.9898, 24.312, 26.9815, 28.086, 30.984, 32.064, 35.453, 39.948, 39.102, 40.08, 44.956, 47.9, 50.942, 51.996, 54.938, 55.847])
	fluxes = numpy.array([0.0873, 0.0571, 0.00208, 0.000474, 0.000895, 0.0106, 0.00235, 0.0157, 0.000328, 0.0046, 0.000754, 0.00801, 0.00115, 0.00796, 0.00027, 0.00229, 0.000294, 0.000836, 0.000536, 0.00147, 0.000304, 0.00113, 0.000631, 0.00136, 0.00135, 0.0204])
	gammas = numpy.array([2.71, 2.64, 2.54, 2.75, 2.95, 2.66, 2.72, 2.68, 2.69, 2.64, 2.66, 2.64, 2.66, 2.75, 2.69, 2.55, 2.68, 2.64, 2.65, 2.7, 2.64, 2.61, 2.63, 2.67, 2.46, 2.59])
	
	# Integrate each power law (without a knee) to get the total flux
	# This calculation is done in TeV (it matters!)
	emin = emin*mass_number/I3Units.TeV
	# Generation spectrum includes the slope change
	gamma = gammas + dslope
	fluxsums = fluxes*(emin**(1-gamma))/(gamma-1)
	nshower = nevents*fluxsums/fluxsums.sum()
	
	components = []
	area = numpy.pi**2*radius*(radius+height)
	for pt in [getattr(ParticleType, p) for p in ('PPlus', 'He4Nucleus', 'N14Nucleus', 'Al27Nucleus', 'Fe56Nucleus')]:
		if pt < 100:
			z = 1
		else:
			z = pt % 100
		components.append(HoerandelComponent(z, gamma[z-1], emin[z-1]*I3Units.TeV, emax,
		    nevents=nshower[z-1], area=area, particle_type=pt))
	return GenerationProbabilityCollection(components).to_PDG()

class UprightCylinderAcceptance(object):
	"""
	differential acceptance (dA\Omega/d \cos\theta) of an upright cylinder
	"""
	def __init__(self, length, radius):
		self.length = length
		self.radius = radius
	def __call__(self, ct):
		l, r = self.length, self.radius
		return 4*numpy.pi**2*r*(r*abs(ct) + (2*l/numpy.pi)*numpy.sqrt(1-ct**2))

def NeutrinoGenerator(NEvents, FromEnergy, ToEnergy, GammaIndex, NeutrinoFlavor,
    ZenithMin=0, ZenithMax=numpy.pi, AzimuthMin=0, AzimuthMax=2*numpy.pi,
    InjectionMode='Circle', **kwargs):
	"""
	Construct a GenerationProbability appropriate for NeutrinoGenerator simulation. The arguments
	have the same meaning as those to the I3NeutrinoGenerator.
	
	.. warning:: The return value of the GenerationProbability will be in units of :math:`GeV^{-1} sr^{-1} m^{-2}`
	             rather than :math:`cm^{-2}`. Make sure that you use fluxes in the appropriate units!
	"""
	
	if InjectionMode == 'Circle':
		# Legacy NeutrinoGenerator samples on a cylindrical surface whose axis
		# passes through the origin and is parallel to the incoming neutrino.
		entendue = (numpy.cos(ZenithMin)-numpy.cos(ZenithMax))*(AzimuthMax-AzimuthMin)*numpy.pi*kwargs.get('InjectionRadius', 1200*I3Units.m)**2
	else:
		# Modern NeutrinoGenerator samples on an upright cylindrical surface
		l = kwargs.get('CylinderHeight', 1900*I3Units.m)
		r = kwargs.get('CylinderRadius', 950*I3Units.m)
		# differential acceptance (dA\Omega/d \cos\theta)
		entendue = UprightCylinderAcceptance(l, r)
	probs = []
	for p in (NeutrinoFlavor, NeutrinoFlavor+'Bar'):
		pt = getattr(ParticleType, p)
		# Half of the flux comes as neutrinos, half as anti-neutrinos
		probs.append(PowerLaw(-GammaIndex, FromEnergy, ToEnergy, (NEvents/2.), entendue, particle_type=pt))
	return GenerationProbabilityCollection(probs).to_PDG()

def _import_mysql():
	"Import the flavor of the month"
	import importlib
	for impl in 'MySQLdb', 'mysql.connector', 'pymysql':
		try:
			mysql = importlib.import_module(impl)
			return mysql
		except ImportError:
			pass

	raise ImportError('No MySQL bindings found!')

import ast
class SimprodFunction(ast.NodeTransformer):
	"""
	A callable version of simprod's $eval()
	"""
	ALLOWED_FUNCS = set(['int'])
	ALLOWED_NODE_TYPES = set(['Expression', 'Call', 'Name', 'Load', 'Num',
	    'BinOp', 'Add', 'Sub', 'Div', 'Mult', 'Mod'])
	
	def __init__(self, codestring, steering=dict()):
		super(SimprodFunction, self).__init__()
		# turn simprod syntax into valid Python
		codestring = codestring.replace('$', '').replace('::', '_')
		self.code = compile(self.visit(ast.parse(codestring, mode='eval')), '<AST>', 'eval')
		self.steering = dict([(k.replace('::', '_'), v) for k,v in steering.items()])
		
	def __call__(self, **kwargs):
		"""
		Evaluate the parsed expression. Keyword arguments not provided will
		be taken from the steering dictionary, if present.
		"""
		kw = dict(self.steering)
		kw.update(kwargs)
		return eval(self.code, kw)
	
	def visit_Call(self, node):
		if node.func.id == 'steering' or node.func.id == 'args':
			return ast.copy_location(ast.Name(node.args[0].id, ast.Load()), node)
		elif node.func.id == 'eval':
			return self.generic_visit(node.args[0])
		else:
			if node.func.id not in self.ALLOWED_FUNCS:
				raise RuntimeError("Invalid expression: %s() not allowed" % (node.func.id))
			return self.generic_visit(node)
		
	def generic_visit(self, node):
		nodetype = type(node).__name__
		if nodetype not in self.ALLOWED_NODE_TYPES:
			raise RuntimeError("Invalid expression: %s not allowed" % nodetype)
		return super(SimprodFunction, self).generic_visit(node)

def from_simprod(dataset_id, use_muongun=False):
	"""
	Extreme laziness: parse weighting scheme out of the simulation production database
	
	.. note:: This requires MySQL bindings (e.g. mysql-connector-python)
	
	:param dataset_id: the number of the SimProd dataset
	:returns: an instance of :py:class:`GenerationProbability`
	          that, when called, will return the number of particles injected into the simulation
	          per :math:`\textrm{GeV} \, \textrm{m}^2` \, \textrm{sr}`.
	"""
	import re
	mysql = _import_mysql()
	
	try:
		db = mysql.connect(host='dbs4.icecube.wisc.edu', user='i3simprod-ro', passwd='Twed9~Dret', db='i3simprod')
	except mysql.OperationalError, e:
		reason = e.args[1]
		reason += " This might happen if you tried to connect to the simprod database from many cluster jobs in parallel. Don't do that. Instead, query the generator for your dataset once, and pass it to your jobs in a file."
		raise mysql.OperationalError(e.args[0], reason)
	cursor = db.cursor()
	
	if isinstance(dataset_id, str):
		raise UnboundLocalError
	cursor.execute("SELECT COUNT(*) FROM dataset WHERE dataset_id=%s", (dataset_id,))
	if cursor.fetchone()[0] == 0:
		raise ValueError("Dataset %s does not exist in the simprod database" % repr(dataset_id))
	
	# In case this is a post-processed set, chase the chain back until we hit the real generated set
	while True:
		cursor.execute("SELECT description FROM dataset WHERE dataset_id=%s", (dataset_id,))
		match = re.match(r'.*(from|of) dataset (\d{4})', cursor.fetchone()[0], re.IGNORECASE)
		if match:
			dataset_id = int(match.group(2))
		else:
			break
	
	# query category and number of completed files
	cursor.execute("SELECT category FROM dataset JOIN simcat ON dataset.simcat_id=simcat.simcat_id and dataset.dataset_id=%s", (dataset_id,))
	row = cursor.fetchone()
	category = row[0]
	
	typemap=dict(string=str, int=int, double=float, float=float, bool=bool)
	def get_steering(cursor, dataset_id):
		cursor.execute("SELECT name, type, value FROM steering_parameter WHERE dataset_id=%s", (dataset_id,))
		steering = {}
		for name, typus, value in cursor.fetchall():
			try:
				steering[name] = typemap[typus](value)
			except ValueError:
				steering[name] = value
				pass
		return steering
	steering = get_steering(cursor, dataset_id)
	
	if category == 'Test':
		if steering['mctype'] == 'corsika':
			category = 'CORSIKA-in-ice'
		elif steering['mctype'].startswith('nugen'):
			category = 'neutrino-generator'
	
	def get_tray_parameter(dataset_id, key, klass=None):
		if klass is None:
			cursor.execute("SELECT cparameter_id, type, value FROM cparameter WHERE dataset_id=%s AND name=%s", (dataset_id, key))
		else:
			cursor.execute("SELECT cparameter_id, type, value FROM cparameter INNER JOIN (module_pivot, module) ON (module_pivot.module_id=module.module_id AND cparameter.module_pivot_id=module_pivot.module_pivot_id) WHERE module_pivot.dataset_id=%s AND cparameter.name=%s AND module.class=%s", (dataset_id, key, klass))
		row = cursor.fetchone()
		if not row:
			return None
		if row[1] in typemap:
			try:
				return typemap[row[1]](row[2])
			except ValueError:
				# not a literal, must be a function
				return SimprodFunction(row[2], get_steering(cursor, dataset_id))
		else:
			cursor.execute("SELECT value FROM carray_element WHERE cparameter_id=%s", (row[0],))
			return [float(v[0]) for v in cursor.fetchall()]
	
	def get_metaproject(dataset_id, tray_name, tray_index=None):
		"""
		Get metaproject version for a tray by name, or if that fails, by index
		"""
		cursor.execute("SELECT metaproject.name, metaproject.major_version, metaproject.minor_version, metaproject.patch_version FROM tray JOIN metaproject_pivot ON tray.tray_index=metaproject_pivot.tray_index AND tray.dataset_id=metaproject_pivot.dataset_id JOIN metaproject ON metaproject_pivot.metaproject_id=metaproject.metaproject_id WHERE tray.dataset_id=%s AND tray.name=%s", (dataset_id, tray_name))
		row = cursor.fetchone()
		if row is None and tray_index is not None:
			cursor.execute("SELECT metaproject.name, metaproject.major_version, metaproject.minor_version, metaproject.patch_version FROM tray JOIN metaproject_pivot ON tray.tray_index=metaproject_pivot.tray_index AND tray.dataset_id=metaproject_pivot.dataset_id JOIN metaproject ON metaproject_pivot.metaproject_id=metaproject.metaproject_id WHERE tray.dataset_id=%s AND tray.tray_index=%s", (dataset_id, tray_index))
			row = cursor.fetchone()
		return tuple([row[0]] + map(int, row[1:]))
		
	if category == 'neutrino-generator':
		if 'NUGEN::elogmin' in steering:
			emin, emax = 10**steering['NUGEN::elogmin'], 10**steering['NUGEN::elogmax']
		elif 'NUGEN::from_energy' in steering:
			emin, emax = steering['NUGEN::from_energy'], steering['NUGEN::to_energy']
		else:
			emin, emax = steering['NUGEN::emin'], steering['NUGEN::emax']
		nugen_kwargs = dict()
		if 'NUGEN::injectionradius' in steering:
			nugen_kwargs['InjectionRadius'] = steering['NUGEN::injectionradius']
		elif 'NUGEN::cylinder_length' in steering:
			nugen_kwargs['CylinderHeight'] = steering['NUGEN::cylinder_length']
			nugen_kwargs['CylinderRadius'] = steering['NUGEN::cylinder_radius']
		if get_metaproject(dataset_id, 'nugen', 0)[1:] >= (4,1,6):
			nugen_kwargs['InjectionMode'] = 'Cylinder'
		generator = NeutrinoGenerator(NEvents=steering['nevents'],
		    FromEnergy     =emin,
		    ToEnergy       =emax,
		    GammaIndex     =steering['NUGEN::gamma'],
		    NeutrinoFlavor =steering['NUGEN::flavor'],
		    ZenithMin      =steering['NUGEN::zenithmin']*I3Units.deg,
		    ZenithMax      =steering['NUGEN::zenithmax']*I3Units.deg,
		    **nugen_kwargs)
	elif category == 'CORSIKA-in-ice':
		if steering['composition'].startswith('5-component') or steering['composition'] == 'jcorsika':
			gamma = get_tray_parameter(dataset_id, "pgam")
			if gamma is None:
				gamma = [-2]*5
			else:
				gamma = [-abs(v) for v in gamma]
			norm = get_tray_parameter(dataset_id, "pnorm")
			if norm is None:
				norm = [10., 5., 3., 2., 1.]
			if get_tray_parameter(dataset_id, 'CutoffType') == "EnergyPerNucleon":
				LowerCutoffType = 'EnergyPerNucleon'
			else:
				LowerCutoffType = 'EnergyPerParticle'
			UpperCutoffType = get_tray_parameter(dataset_id, 'UpperCutoffType')
			#  Upper cutoff type appeared in IceSim 4, and defaults to the lower cutoff type
			if UpperCutoffType is None and get_metaproject(dataset_id, 'generate', 0)[1] >= 4:
				UpperCutoffType = LowerCutoffType
			elif UpperCutoffType == 'EnergyPerNucleon':
				UpperCutoffType = 'EnergyPerNucleon'
			else:
				UpperCutoffType = 'EnergyPerParticle'
			length = get_tray_parameter(dataset_id, 'length', "icecube.simprod.generators.CorsikaGenerator")
			if length is None:
				if 'CORSIKA::length' in steering:
					length = steering['CORSIKA::length']*I3Units.m
				else:
					length = 1600*I3Units.m
					warnings.warn("No target cylinder length for dataset {dataset_id}! Assuming {length:.0f} m".format(**locals()))
			radius = get_tray_parameter(dataset_id, 'radius', "icecube.simprod.generators.CorsikaGenerator")
			if radius is None:
				if 'CORSIKA::radius' in steering:
					radius = steering['CORSIKA::radius']*I3Units.m
				else:
					radius = 800*I3Units.m
					warnings.warn("No target cylinder length for dataset {dataset_id}! Assuming {radius:.0f} m".format(**locals()))
			if use_muongun:
				from icecube import MuonGun
				nevents = steering['CORSIKA::showers']
				if gamma == [-2.0]*5 and norm == [10., 5., 3., 2., 1.]:
					model = 'Standard5Comp'
				elif gamma == [-2.6]*5 and norm == [3., 2., 1., 1., 1.]:
					model = 'CascadeOptimized5Comp'
				else:
					raise ValueError("Unknown CORSIKA configuration!")
				generator = nevents*MuonGun.corsika_genprob(model)
			else:
				generator = FiveComponent(steering['CORSIKA::showers'],
				    emin=steering['CORSIKA::eprimarymin']*I3Units.GeV,
				    emax=steering['CORSIKA::eprimarymax']*I3Units.GeV,
				    normalization=norm, gamma=gamma,
				    LowerCutoffType=LowerCutoffType, UpperCutoffType=UpperCutoffType,
				    height=length, radius=radius)
		elif steering['composition'].startswith('polygonato') or steering['composition'].startswith('Hoerandel'):
			if use_muongun:
				from icecube import MuonGun
				length = steering['CORSIKA::length']*I3Units.m
				radius = steering['CORSIKA::radius']*I3Units.m
				area = numpy.pi**2*radius*(radius+length)
				areanorm = 0.131475115*area
				generator = (steering['CORSIKA::showers']/areanorm)*MuonGun.corsika_genprob('Hoerandel5')
			else:
				generator = Hoerandel(steering['CORSIKA::showers'],
				    emin=steering['CORSIKA::eprimarymin']*I3Units.GeV,
				    emax=steering['CORSIKA::eprimarymax']*I3Units.GeV,
				    dslope=steering['CORSIKA::dslope'],
				    height=steering['CORSIKA::length']*I3Units.m,
				    radius=steering['CORSIKA::radius']*I3Units.m)
	elif category == 'CORSIKA-ice-top':
		
		# get the parent (generator) dataset, as the generator parameters may
		# be buried several generations back
		substeering = steering
		while not ('CORSIKA::ebin' in substeering and 'CORSIKA::radius' in substeering):
			try:
				substeering = get_steering(cursor, substeering['inputdataset'])
			except KeyError:
				# sampling radius is in the topsimulator config
				radius = get_tray_parameter(dataset_id, 'r', "icecube.simprod.modules.IceTopShowerGenerator")
				break
		else:
			# sampling radius is a steering parameter
			radius = SimprodFunction(substeering['CORSIKA::radius'], substeering)
		
		# logarithmic energy bin is a function of the procnum
		ebin = SimprodFunction(substeering['CORSIKA::ebin'], substeering)
		
		# check that the energy steps are spaced like we expect
		dlogE = ebin(procnum=1) - ebin(procnum=0)
		assert dlogE > 0, "Subsequent procnums end up in different energy bins"
		eslope = substeering['CORSIKA::eslope']
		assert eslope == -1, "Weighting scheme only makes sense for E^-1 generation"
		
		try:
			oversampling = substeering['CORSIKA::oversampling']
		except KeyError:
			oversampling = get_tray_parameter(dataset_id, 'samples', "icecube.simprod.modules.IceTopShowerGenerator")
		
		ctmin = numpy.cos(numpy.radians(substeering['CORSIKA::cthmax']))
		ctmax = numpy.cos(numpy.radians(substeering['CORSIKA::cthmin']))
		# projected area x solid angle: pi^2 r^2 (ctmax^2 - ctmin^2)
		area_element = numpy.pi**2*(ctmax**2 - ctmin**2)
		
		emin = substeering['CORSIKA::ebin_first']
		emax = substeering['CORSIKA::ebin_last']
		num_ebins = int((emax - emin) / dlogE) + 1
		ebins = numpy.linspace(emin, emax, num_ebins)
		radii = numpy.array([(eb, radius(CORSIKA_ebin=eb)) for eb in ebins])
		
		# go up further levels if necessary
		while not 'CORSIKA::primary' in substeering:
			substeering = get_steering(cursor, substeering['inputdataset'])
		primary = substeering['PRIMARY::%s' % substeering['CORSIKA::primary']]
		
		probs = []
		for r, ebins in itertools.groupby(radii, lambda pair: pair[1]):
			area = r**2*area_element
			ebins = [pair[0] for pair in ebins]
			probs.append(PowerLaw(eslope, 10**ebins[0], 10**(ebins[-1]+dlogE), len(ebins), area, particle_type=ParticleType.values[primary]))
		
		# turn into a collection
		generator = GenerationProbabilityCollection(probs).to_PDG()
		# normalize to relative proportion in each bin
		generator /= sum([prob.nevents for prob in probs])
		# and scale to total number of showers with over-sampling
		generator *= substeering['CORSIKA::showers']*oversampling
	
	else:
		raise ValueError("No weighting scheme implemented for %s simulations" % (category))
	cursor.close()
	db.close()
	return generator

class EnergyWeight(object):
	def __init__(self, target_flux, generation_spectrum):
		self.target_flux = target_flux
		self.generation_spectrum = generation_spectrum
	def __call__(self, E, zenith):
		return self.target_flux(E)/self.generation_spectrum(E, zenith)

