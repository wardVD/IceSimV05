
from icecube import icetray, photonics_service
from icecube.photonics_service import Geometry, Parity
import numpy

nPhase = 1.3195; 
nGroup = 1.35634;
cVacuum = 0.299792458;

units = icetray.I3Units

def photonics_coords(pos, source, t=0., tref=0., geo=Geometry.SPHERICAL, parity=Parity.ODD):
	"""Transform from detector coordinates"""
	dx = pos.x - source.x
	dy = pos.y - source.y
	dz = pos.z - source.z
	l = dx*source.dirx + dy*source.diry + dz*source.dirz
	r = numpy.sqrt(dx*dx + dy*dy + dz*dz);	
	rho = numpy.sqrt(r*r - l*l);
	costheta = l/r;
	cosAzi = 0;
	
	if rho > 0:
		cosAzi = (dx*source.perpx + dy*source.perpy
		    + dz*source.perpz)/rho;
	if parity == Parity.ODD:
		cosAzi *= -1
		
	coords = numpy.zeros(6)
	# NB: PhotonicsService deals in radians, but the tables are in degrees.
	# print ('-'*10) + '>', cosAzi, 180*numpy.arccos(cosAzi)/numpy.pi
	coords[1] = 180*numpy.arccos(cosAzi)/numpy.pi
	coords[3] = t - tref - (r*nGroup/cVacuum)
	coords[5] = source.z

	if parity == Parity.ODD:
		coords[4] = 180 - source.zenith
	else:
		coords[4] = source.zenith
	
	if geo == Geometry.SPHERICAL:
		coords[0] = r
		coords[2] = costheta
	elif geo == Geometry.CYLINDRICAL:
		coords[0] = rho
		coords[2] = l
	
	return coords
	
def calc_jacobian(x, y, z, geo, ps):
	jac = photonics_service.getJacobian(x, y, z, ps, geo)
	cols = [0, 1, 2, 4, 5]
	return jac[cols,:][:,cols]
	
def tweak_source(source, vec):
	if len(vec) == 5:
		dx, dy, dz, dtheta, dphi = vec
	elif len(vec) == 6:
		dx, dy, dz = vec[:3]
		dtheta, dphi = vec[4:]
	# NB: PhotonicsSource needs degrees
	dtheta /= units.degree
	dphi /= units.degree
	phi = numpy.arctan2(source.diry, source.dirx)/units.degree - 180
	return photonics_service.PhotonicsSource(source.x+dx, source.y+dy,
	    source.z+dz, source.zenith+dtheta, phi+dphi, source.speed,
	    source.length, source.E, source.type)
	
def random_vertices(size=1, radius=500):
	rho = radius*numpy.sqrt(numpy.random.uniform(size=size))
	phi = numpy.random.uniform(high = 2*numpy.pi, size=size)
	x = rho*numpy.sin(phi)
	y = rho*numpy.cos(phi)
	z = numpy.random.uniform(low=-radius, high=radius, size=size)
	return (x, y, z)
	
def random_directions(size=1):
	theta = 180*numpy.arccos(numpy.random.uniform(low=0, high=1, size=size))/numpy.pi
	phi = 180*numpy.random.uniform(high = 2*numpy.pi, size=size)/numpy.pi
	return (theta, phi)

def random_sources(size=1):
	events = numpy.column_stack(random_vertices(size) + random_directions(size))
	for ev in events:
		yield photonics_service.PhotonicsSource(ev[0], ev[1], ev[2], 
		    ev[3], ev[4], 1, 0, 1e5, 1)

def finitediff_bsearch(low, high, get_gradient, test_gradient, tol=1e-12):
	"""
	hobo binary search for an epsilon that is small enough to reproduce
	the predicted gradient, but large enough to still produce a measureable
	difference in the function values.
	"""
	grad = numpy.nan
	eps = high
	success = False
	while high-low > tol:
		eps = low + (high-low)/2.0

		gradtmp = get_gradient(eps)

		if gradtmp == 0:
			if test_gradient(0):
				grad = gradtmp
				success = True
				break
			else:
				low = eps
		else:
			if test_gradient(gradtmp):
				grad = gradtmp
				success = True
				break
			else:
				grad = gradtmp
				high = eps
	return grad, eps, success

def report_badness(title, labels, predicted, measured, steps, good):
	gen_template = lambda tag, format, args: ('%12s' % (tag + ': ')) + (' '.join([format]*len(args)) % tuple(args))
	gen_label = lambda format, args: (' '*12) + (' '.join([format]*len(args)) % tuple(args))
	print('!!! ' + title)
	print(gen_template('step size', '% 8.4e', steps))
	print('-'*79)
	print(gen_label('%11s', labels))
	print(gen_template('predicted', '% 8.4e', predicted))
	print(gen_template('measured', '% 8.4e', measured))
	print('='*79)
	print(gen_template('ratio', '% 8.4e', predicted/measured))
	print(gen_label('%11s', [('^'*10, '')[bool(b)] for b in good]))
	print('='*79)

class OpenStruct(object):
	"""We like convenience."""
	def __init__(self, **kwargs):
		self.__dict__.update(kwargs)


