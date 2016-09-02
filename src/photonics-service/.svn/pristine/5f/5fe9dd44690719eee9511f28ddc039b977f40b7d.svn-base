from __future__ import print_function

import numpy as n
from icecube import icetray, photonics_service, dataclasses
from icecube.icetray import I3Units as units

import os

from icecube.photonics_service import Geometry, Parity

nPhase = 1.3195; 
nGroup = 1.35634;
cVacuum = 0.299792458;

def random_vertices(size=1):
	rho = 500*n.sqrt(n.random.uniform(size=size))
	phi = n.random.uniform(high = 2*n.pi, size=size)
	x = rho*n.sin(phi)
	y = rho*n.cos(phi)
	z = n.random.uniform(low=-500, high=500, size=size)
	return (x, y, z)
	
def random_directions(size=1):
	theta = 180*n.cos(n.random.uniform(low=-1, high=1, size=size))/n.pi
	phi = 180*n.random.uniform(high = 2*n.pi, size=size)/n.pi
	return (theta, phi)

def random_sources(size=1):
	events = n.column_stack(random_vertices(size) + random_directions(size))
	for ev in events:
		yield photonics_service.PhotonicsSource(ev[0], ev[1], ev[2], 
		    ev[3], ev[4], 1, 0, 1e5, 1)
		
def tweak_source(source, vec):
	if len(vec) == 5:
		dx, dy, dz, dtheta, dphi = vec
	elif len(vec) == 6:
		dx, dy, dz = vec[:3]
		dtheta, dphi = vec[4:]
	# NB: PhotonicsSource needs degrees
	dtheta /= units.degree
	dphi /= units.degree
	phi = n.arctan2(source.diry, source.dirx)/units.degree - 180
	return photonics_service.PhotonicsSource(source.x+dx, source.y+dy,
	    source.z+dz, source.zenith+dtheta, phi+dphi, 1, source.length,
	    source.E, source.type)
	
def tweak_source_(source, vec):
	if len(vec) == 6:
		dx, dy, dz, dtheta, dphi, dE = vec
	elif len(vec) == 7:
		dx, dy, dz = vec[:3]
		dtheta, dphi, dE = vec[4:]
	# NB: PhotonicsSource needs degrees
	dtheta /= units.degree
	dphi /= units.degree
	phi = n.arctan2(source.diry, source.dirx)/units.degree - 180
	return photonics_service.PhotonicsSource(source.x+dx, source.y+dy,
	    source.z+dz, source.zenith+dtheta, phi+dphi, source.length,
	    source.E+dE, source.type)
	
def random_oms(size=1):
	events = n.random_vertices(size=1)

def cos_azi(x, y, z, source):
	dx = x - source.x
	dy = y - source.y
	dz = z - source.z
	l = dx*source.dirx + dy*source.diry + dz*source.dirz
	r = n.sqrt(dx*dx + dy*dy + dz*dz);
	rho = n.sqrt(r*r - l*l);
	costheta = l/r;
	cosAzi = 0;
	
	if rho > 0:
		cosAzi = (dx*source.perpx + dy*source.perpy
		    + dz*source.perpz)/rho;
	return cosAzi

def photonics_coords(x, y, z, t, tref, source,
    geo=photonics_service.Geometry.SPHERICAL,
    parity=photonics_service.Parity.ODD):
	dx = x - source.x
	dy = y - source.y
	dz = z - source.z
	l = dx*source.dirx + dy*source.diry + dz*source.dirz
	r = n.sqrt(dx*dx + dy*dy + dz*dz);	
	rho = n.sqrt(r*r - l*l);
	costheta = l/r;
	cosAzi = 0;
	
	if rho > 0:
		cosAzi = (dx*source.perpx + dy*source.perpy
		    + dz*source.perpz)/rho;
	if parity == photonics_service.Parity.ODD:
		cosAzi *= -1
		
	coords = n.zeros(6)
	# NB: PhotonicsService deals in radians, but the tables are in degrees.
	coords[1] = 180*n.arccos(cosAzi)/n.pi
	coords[3] = t - tref - (r*nGroup/cVacuum)
	coords[5] = source.z

	if parity == photonics_service.Parity.ODD:
		coords[4] = 180 - source.zenith
	else:
		coords[4] = source.zenith
	
	if geo == photonics_service.Geometry.SPHERICAL:
		coords[0] = r
		coords[2] = costheta
	elif geo == photonics_service.Geometry.CYLINDRICAL:
		coords[0] = rho
		coords[2] = l
	
	return coords

def calc_hessian(x, y, z, source, geo, parity):
	# worked out in gory detail by S. Panknin and A. Schoenwald
	# at the Brussels Collaboration Meeting
	
	hess = n.zeros((6,6,6))
	
	dx = x - source.x
	dy = y - source.y
	dz = z - source.z
	l = dx*source.dirx + dy*source.diry + dz*source.dirz
	r = n.sqrt(dx*dx + dy*dy + dz*dz);	
	rho = n.sqrt(r*r - l*l);
	costheta = l/r;
	cosAzi = 0;
	
	if rho > 0:
		cosAzi = (dx*source.perpx + dy*source.perpy
		    + dz*source.perpz)/rho;
	if parity == photonics_service.Parity.ODD:
		cosAzi *= -1
		
	sin2Azi = 1 - cosAzi**2
	sinAzi = n.sqrt(sin2Azi)
	
	# some handy derivatives of the sourceward vector
	dnx_dtheta = -source.costheta*source.cosphi
	dny_dtheta = -source.costheta*source.sinphi
	dnz_dtheta = source.sintheta
	
	dnx_dphi = -source.sintheta*source.sinphi
	dny_dphi = source.sintheta*source.cosphi
	# dnz_dphi == 0
	
	dnx_dtheta_dtheta = -source.sintheta*source.cosphi
	dny_dtheta_dtheta = -source.sintheta*source.sinphi
	dnz_dtheta_dtheta = -source.costheta
	
	dnx_dphi_dphi = -source.sintheta*source.cosphi
	dny_dphi_dphi = -source.sintheta*source.sinphi
	# dnz_dphi_dphi == 0
	
	dnx_dtheta_dphi = source.costheta*source.sinphi
	dny_dtheta_dphi = -source.costheta*source.cosphi
	# dnz_dtheta_dphi == 0
	
	# some handy projective derivatives
	dl_dtheta = -dx*source.costheta*source.cosphi \
	    - dy*source.costheta*source.sinphi + dz*source.sintheta
	dl_dphi = -dx*source.sintheta*source.sinphi \
	    + dy*source.sintheta*source.cosphi
	
	dl_dtheta_dtheta = dx*dnx_dtheta_dtheta + dy*dny_dtheta_dtheta + dz*dnz_dtheta_dtheta
	dl_dtheta_dphi = dx*dnx_dtheta_dphi + dy*dny_dtheta_dphi
	dl_dphi_dphi = dx*dnx_dphi_dphi + dy*dny_dphi_dphi
	
	drho_dx = (-dx + l*source.dirx)/rho
	drho_dy = (-dy + l*source.diry)/rho
	drho_dz = (-dz + l*source.dirz)/rho
	
	drho_dtheta = -l*dl_dtheta/rho
	drho_dphi = -l*dl_dphi/rho
	
	drho_dx_dx = (1 - source.dirx**2 - drho_dx**2)/rho # drho / dx dx
	drho_dy_dy = (1 - source.diry**2 - drho_dy**2)/rho # drho / dy dy
	drho_dz_dz = (1 - source.dirz**2 - drho_dz**2)/rho # drho / dz dz

	drho_dx_dy = (-source.dirx*source.diry - drho_dx*drho_dy)/rho # drho / dx dy
	drho_dy_dz = (-source.diry*source.dirz - drho_dy*drho_dz)/rho # drho / dy dz
	drho_dx_dz = (-source.dirx*source.dirz - drho_dx*drho_dz)/rho # drho / dx dz
	
	drho_dx_dtheta = -(l*source.dirx - dx)*drho_dtheta/(rho*rho) + (source.dirx*dl_dtheta + l*dnx_dtheta)/rho
	drho_dy_dtheta = -(l*source.diry - dy)*drho_dtheta/(rho*rho) + (source.diry*dl_dtheta + l*dny_dtheta)/rho
	drho_dz_dtheta = -(l*source.dirz - dz)*drho_dtheta/(rho*rho) + (source.dirz*dl_dtheta + l*dnz_dtheta)/rho
	
	drho_dx_dphi = -(l*source.dirx - dx)*drho_dphi/(rho*rho) + (source.dirx*dl_dphi + l*dnx_dphi)/rho
	drho_dy_dphi = -(l*source.diry - dy)*drho_dphi/(rho*rho) + (source.diry*dl_dphi + l*dny_dphi)/rho
	drho_dz_dphi = -(l*source.dirz - dz)*drho_dphi/(rho*rho) + (source.dirz*dl_dphi)/rho
	
	drho_dtheta_dtheta = l*dl_dtheta*drho_dtheta/(rho*rho) - (dl_dtheta*dl_dtheta + l*dl_dtheta_dtheta)/rho
	drho_dtheta_dphi = l*dl_dtheta*drho_dphi/(rho*rho) - (dl_dtheta*dl_dphi + l*dl_dtheta_dphi)/rho
	drho_dphi_dphi = l*dl_dphi*drho_dphi/(rho*rho) - (dl_dphi*dl_dphi + l*dl_dphi_dphi)/rho
	
	# \vec{x_om - x_s} \cdot \vec{p}
	xp = dx*source.perpx + dy*source.perpy + dz*source.perpz
	
	jac = photonics_service.getJacobian(x, y, z, source, geo, parity)
	
	if geo == photonics_service.Geometry.SPHERICAL:
		# r
		hess[0][0][0] = 1/r - dx**2/r**3 # dr / dx dx
		hess[1][1][0] = 1/r - dy**2/r**3 # dr / dy dy
		hess[2][2][0] = 1/r - dz**2/r**3 # dr / dz dz
	
		hess[0][1][0] = -dx*dy/r**3 # dr / dx dy
		hess[1][2][0] = -dy*dz/r**3 # dr / dy dz
		hess[0][2][0] = -dx*dz/r**3 # dr / dx dz
		
		# NB: remaining dr / da db terms are zero.
	else:
		# rho
		hess[0][0][0] = drho_dx_dx
		hess[1][1][0] = drho_dy_dy
		hess[2][2][0] = drho_dz_dz
	                    
		hess[0][1][0] = drho_dx_dy
		hess[1][2][0] = drho_dy_dz
		hess[0][2][0] = drho_dx_dz	
	
		# and so on in that fashion until all are eaten...

		hess[0][4][0] = drho_dx_dtheta # drho / dx dtheta	
		hess[1][4][0] = drho_dy_dtheta # drho / dy dtheta
		hess[2][4][0] = drho_dz_dtheta # drho / dz dtheta
		
		hess[0][5][0] = drho_dx_dphi # drho / dx dphi
		hess[1][5][0] = drho_dy_dphi # drho / dy dphi
		hess[2][5][0] = drho_dz_dphi # drho / dz dphi
		
		hess[4][4][0] = drho_dtheta_dtheta # drho / dtheta dtheta
		hess[4][5][0] = drho_dtheta_dphi   # drho / dtheta dphi
		hess[5][5][0] = drho_dphi_dphi     # drho / dphi dphi
					
	# ====
	# phi
	# ====
	
	dphi_dx = jac[0][1]
	dphi_dy = jac[1][1]
	dphi_dz = jac[2][1]
	
	dphi_dtheta = jac[4][1]
	dphi_dphi = jac[5][1]
	
	if parity == Parity.EVEN:
		sign = 1
	else:
		sign = -1
	
	# NB: this will blow the hell up at phi = 0,180
	# FIXME: add numerically stable verion for cosAzi =~ +-1
	
	sr2d = (sinAzi*rho**2*units.degree)
	
	dpx_dtheta = -source.sintheta*source.cosphi
	dpy_dtheta = -source.sintheta*source.sinphi
	dpz_dtheta = -source.costheta
	
	dpx_dphi = source.costheta*source.sinphi
	dpy_dphi = -source.costheta*source.cosphi
	# dpz_dphi == 0
	
	dxp_dtheta = dx*dpx_dtheta + dy*dpy_dtheta + dz*dpz_dtheta
	dxp_dphi = dx*dpx_dphi + dy*dpy_dphi
	dxp_dphi_dphi = -dx*source.perpx - dy*source.perpy
	
	a = -(cosAzi/sin2Azi)*(source.perpx/rho + xp*drho_dx/(rho*rho))
	
	# dphi / dx dx
	hess[0][0][1] = sign*(dphi_dx*a + \
	    (xp*drho_dx_dx - 2*xp*drho_dx*drho_dx/rho - 2*source.perpx*drho_dx)/sr2d)
	# dphi / dx dy
	hess[0][1][1] = sign*(dphi_dy*a + \
	    (xp*drho_dx_dy - 2*xp*drho_dx*drho_dy/rho - source.perpx*drho_dy - source.perpy*drho_dx)/sr2d)
	# dphi / dx dz
	hess[0][2][1] = sign*(dphi_dz*a + \
	    (xp*drho_dx_dz - 2*xp*drho_dx*drho_dz/rho - source.perpx*drho_dz - source.perpz*drho_dx)/sr2d)
	# dphi / dx dtheta
	hess[0][4][1] = sign*(dphi_dtheta*a + \
	    (rho*dpx_dtheta - source.perpx*drho_dtheta - 2*xp*drho_dx*drho_dtheta/rho + \
	    drho_dx*dxp_dtheta + xp*drho_dx_dtheta)/sr2d)
	# dphi / dx dphi
	hess[0][5][1] = sign*(dphi_dphi*a + \
	    (rho*dpx_dphi - source.perpx*drho_dphi - 2*xp*drho_dx*drho_dphi/rho + \
	    drho_dx*dxp_dphi + xp*drho_dx_dphi)/sr2d)
	
	a = -(cosAzi/sin2Azi)*(source.perpy/rho + xp*drho_dy/rho**2)
	
	# dphi / dy dy
	hess[1][1][1] = sign*(dphi_dy*a + \
	    (xp*drho_dy_dy - 2*xp*drho_dy*drho_dy/rho - 2*source.perpy*drho_dy)/sr2d)
	# dphi / dy dz
	hess[1][2][1] = sign*(dphi_dz*a + \
	    (xp*drho_dy_dz - 2*xp*drho_dy*drho_dz/rho - source.perpy*drho_dz - source.perpz*drho_dy)/sr2d)
	# dphi / dy dtheta
	hess[1][4][1] = sign*(dphi_dtheta*a + \
	    (rho*dpy_dtheta - source.perpy*drho_dtheta - 2*xp*drho_dy*drho_dtheta/rho + \
	    drho_dy*dxp_dtheta + xp*drho_dy_dtheta)/sr2d)
	# dphi / dy dphi
	hess[1][5][1] = sign*(dphi_dphi*a + \
	    (rho*dpy_dphi - source.perpy*drho_dphi - 2*xp*drho_dy*drho_dphi/rho + \
	    drho_dy*dxp_dphi + xp*drho_dy_dphi)/sr2d)
	
	a = -(cosAzi/sin2Azi)*(source.perpz/rho + xp*drho_dz/rho**2)
	
	# dphi / dz dz
	hess[2][2][1] = sign*(dphi_dz*a + \
	    (xp*drho_dz_dz - 2*xp*drho_dz*drho_dz/rho - 2*source.perpz*drho_dz)/sr2d)
	# dphi / dz dtheta
	hess[2][4][1] = sign*(dphi_dtheta*a + \
	    (rho*dpz_dtheta - source.perpz*drho_dtheta - 2*xp*drho_dz*drho_dtheta/rho + \
	    drho_dz*dxp_dtheta + xp*drho_dz_dtheta)/sr2d)
	# dphi / dz dphi
	hess[2][5][1] = sign*(dphi_dphi*a + \
	    (-source.perpz*drho_dphi - 2*xp*drho_dz*drho_dphi/rho + \
	    drho_dz*dxp_dphi + xp*drho_dz_dphi)/sr2d)
	
	a = -(cosAzi/sin2Azi)*(l/rho + xp*drho_dtheta/rho**2)
	
	# dphi / dtheta dtheta
	hess[4][4][1] = sign*(dphi_dtheta*a + \
	    (rho*dl_dtheta - 2*xp*drho_dtheta*drho_dtheta/rho -\
	    l*drho_dtheta + drho_dtheta*dxp_dtheta + xp*drho_dtheta_dtheta)/sr2d)
	# dphi / dtheta phi
	hess[4][5][1] = sign*(dphi_dphi*a + \
	    (rho*dl_dphi - 2*xp*drho_dtheta*drho_dphi/rho -\
	    l*drho_dphi + drho_dtheta*dxp_dphi + xp*drho_dtheta_dphi)/sr2d)
	
	a = -(cosAzi/sin2Azi)*(-dxp_dphi/rho + xp*drho_dphi/rho**2)
	# dphi / dphi phi
	hess[5][5][1] = sign*(dphi_dphi*a + \
	    (dxp_dphi*drho_dphi - 2*xp*drho_dphi*drho_dphi/rho - \
	    rho*dxp_dphi_dphi + dxp_dphi*drho_dphi + xp*drho_dphi_dphi)/sr2d)
	
	if geo == photonics_service.Geometry.SPHERICAL: # cos(polar angle)
		
		a = (source.dirx - 3*l*dx/(r*r))/(r*r)
		
		hess[0][0][2] = -dx*a/r - (source.dirx*dx + l)/(r*r*r) # dc / dx dx
		hess[0][1][2] = -dy*a/r - (source.diry*dx)/(r*r*r)     # dc / dx dy
		hess[0][2][2] = -dz*a/r - (source.dirz*dx)/(r*r*r)     # dc / dx dz
		
		a = (source.diry - 3*l*dy/(r*r))/(r*r)
		
		hess[1][1][2] = -dy*a/r - (source.diry*dy + l)/(r*r*r) # dc / dy dy
		hess[1][2][2] = -dz*a/r - (source.dirz*dy)/(r*r*r)     # dc / dy dz
		
		a = (source.dirz - 3*l*dz/(r*r))/(r*r)
		
		hess[2][2][2] = -dz*a/r - (source.dirz*dz + l)/(r*r*r) # dc / dz dz
				
		hess[0][4][2] = -dnx_dtheta/r + dx*dl_dtheta/(r*r*r) # dc / dx dtheta
		hess[0][5][2] = -dnx_dphi/r   + dx*dl_dphi  /(r*r*r) # dc / dx dphi
		hess[1][4][2] = -dny_dtheta/r + dy*dl_dtheta/(r*r*r) # dc / dy dtheta
		hess[1][5][2] = -dny_dphi/r   + dy*dl_dphi  /(r*r*r) # dc / dy dphi
		hess[2][4][2] = -dnz_dtheta/r + dz*dl_dtheta/(r*r*r) # dc / dz dtheta
		hess[2][5][2] = dz*dl_dphi/(r*r*r)                   # dc / dz dphi
		
		hess[4][4][2] = dl_dtheta_dtheta/r # dc / dtheta dtheta
		hess[4][5][2] = dl_dtheta_dphi/r   # dc / dtheta dphi
		hess[5][5][2] = dl_dphi_dphi/r     # dc / dphi dphi
		
	else: # l
	
		# NB: l is linear in the spatial coordinates
		
		hess[0][4][2] = -dnx_dtheta
		hess[1][4][2] = -dny_dtheta
		hess[2][4][2] = -dnz_dtheta
		
		hess[0][5][2] = -dnx_dphi
		hess[1][5][2] = -dny_dphi
		# hess[2][5][2] = -dnz_dphi == 0
		
		hess[4][4][2] = dl_dtheta_dtheta
		hess[4][5][2] = dl_dtheta_dphi
		hess[5][5][2] = dl_dphi_dphi
	
	# ===========	
	# delay time 
	# ===========
	
	timescale = nGroup / (r*cVacuum)
	
	hess[0][0][3] = timescale*(dx*dx/(r*r) - 1)  # dt / dx dx
	hess[0][1][3] = timescale*(dx*dy/(r*r))      # dt / dx dy
	hess[0][2][3] = timescale*(dx*dz/(r*r))      # dt / dx dz
	
	hess[1][1][3] = timescale*(dy*dy/(r*r) - 1)  # dt / dy dy
	hess[1][2][3] = timescale*(dy*dz/(r*r))      # dt / dy dz
	
	hess[2][2][3] = timescale*(dz*dz/(r*r) - 1)  # dt / dz dz
	
	# NB: remaining coords depend linearly or not at all on source coords
	
	return hess
	
def calc_jacobian(x, y, z, geo, ps):
	jac = photonics_service.getJacobian(x, y, z, ps, geo)
	cols = [0, 1, 2, 4, 5]
	return jac[cols,:][:,cols]

class Factory:
	args = []
	kwargs = {}
	def __init__(self, ctor, *args, **kwargs):
		self.args = args
		self.kwargs = kwargs
		self.ctor = ctor
		self.instance = None

	def __call__(self):
		if self.instance is None:
			self.instance = self.ctor(*self.args, **self.kwargs)
		return self.instance

def spline_config(fname, sigma=0):
	templates = ['%s.%s.fits', '%s.%s.pspl.fits']
	for temp in templates:
		abs = temp % (fname, 'abs')
		prob = temp % (fname, 'prob')
		if os.path.exists(abs):
			return Factory(photonics_service.I3PhotoSplineService,
			    abs, prob, sigma)
	raise ValueError("whups, no sort of table with that name")

splines = spline_config(os.path.expandvars('$I3_BUILD/../src/photospline/ems_spice1_z100_a10'))

def amp_table_ctor(path):
	table = photonics_service.I3PhotoSplineTable()
	table.SetupTable(path, -1)
	return table

amp_table = Factory(amp_table_ctor, splines.args[0])
timing_table = Factory(amp_table_ctor, splines.args[1])

def test_amplitude_gradient(size=10):
	ps = splines()
	xom, yom, zom = random_vertices(size)
	dx_om, dy_om, dz_om = n.random.uniform(size=size*3, low=-100, high = 100).reshape((3,size))
	failures = 0
	elements = 0
	for source, dx, dy, dz in zip(random_sources(size), dx_om, dy_om, dz_om):
		if not check_jacobian(source.x+dx, source.y+dy, source.z+dz, source, tol=1e-4):
			print('!!! Jacobian screwy, skipping!')
			continue
		else:
			pass
			# print 'Jacobian appears to be correct.'
			# print photonics_coords(source.x+dx, source.y+dy, source.z+dz, photonics_service.Geometry.SPHERICAL, source)
		
		ps.SelectModuleCoordinates(source.x+dx, source.y+dy, source.z+dz)
		meanamp, distance, geotime = ps.SelectSource(source)
		# print meanamp, 'pe at', distance, 'm'
		
		predicted_grad = ps.GetMeanAmplitudeGradient()[:-1] # don't consider energy for now
		labels = ['x_s', 'y_s', 'z_s', 'theta_s', 'phi_s']
		grad = n.zeros(5)
		for i in range(5):
			# tweak each coordinate
			tweak = n.zeros(5)
			tweak[i] = 1e-2
			tweaked_source = tweak_source(source, tweak/2)
			tweakamp_plus = ps.SelectSource(tweaked_source)[0]
			tweaked_source = tweak_source(source, -tweak/2)
			tweakamp_minus = ps.SelectSource(tweaked_source)[0]
			# print tweak
			# print tweakamp_plus, tweakamp_minus
			grad[i] = (tweakamp_plus - tweakamp_minus)/tweak[i]
			
		err = n.zeros(5)
		mask = grad != 0
		imask = grad == 0
		err[mask] = (predicted_grad[mask] - grad[mask])/grad[mask]
		err[imask] = grad[imask]
		baddies = abs(err) > 1e-1
		if baddies.any():
			coords = photonics_coords(source.x+dx, source.y+dy, source.z+dz,
			    geotime, 0, source)
			print('!!! Amplitude gradient is screwy! (coords: %s)' % coords)
			print(predicted_grad)
			print(grad)
			print(predicted_grad/grad)
			failures += baddies.sum()
		elements += baddies.size
	print('------> %d/%d failed' % (failures,elements))
	return float(failures)/elements


def test_quantile_gradient(size=10):
	ps = splines()
	xom, yom, zom = random_vertices(size)
	dx_om, dy_om, dz_om = n.random.uniform(size=size*3, low=-100, high = 100).reshape((3,size))
	failures = 0
	elements = 0
	for source, dx, dy, dz in zip(random_sources(size), dx_om, dy_om, dz_om):
		if not check_jacobian(source.x+dx, source.y+dy, source.z+dz, source, tol=1e-4):
			print('!!! Jacobian screwy, skipping!')
			continue
		else:
			pass
			# print 'Jacobian appears to be correct.'
			# print photonics_coords(source.x+dx, source.y+dy, source.z+dz, photonics_service.Geometry.SPHERICAL, source)

		ps.SelectModuleCoordinates(source.x+dx, source.y+dy, source.z+dz)
		meanamp, distance, geotime = ps.SelectSource(source)
		t_0 = geotime
		edges = geotime + 10 + n.linspace(0,n.sqrt(500),5)**2
		# print 'delaytime', edges - geotime
		
		# predicted_grad = ps.GetProbabilityQuantileGradients(edges, t_0)
		values, predicted_grad, hessians = ps.GetProbabilityQuantileHessians(edges, t_0)
		
		grad = n.zeros(predicted_grad.shape)
		# print grad.shape
		
		eps = 1e-3
		for i in range(6):
			# tweak each coordinate
			if i == 3: # time
				ps.SelectSource(source)
				tweaked_plus = ps.GetProbabilityQuantiles(edges, t_0+eps/2, 0)
				tweaked_minus = ps.GetProbabilityQuantiles(edges, t_0-eps/2, 0)
			else:
				tweak = n.zeros(6)
				tweak[i] = eps
				tweaked_source = tweak_source(source, tweak/2)
				# need to keep the source time constant, so we adjust the reference time
				tref = ps.SelectSource(tweaked_source)[2]
				tweaked_plus = ps.GetProbabilityQuantiles(edges, tref, 0)
				tweaked_source = tweak_source(source, -tweak/2)
				tref = ps.SelectSource(tweaked_source)[2]
				tweaked_minus = ps.GetProbabilityQuantiles(edges, tref, 0)
			grad[:,i] = (tweaked_plus - tweaked_minus)/eps
		
		err = (grad-predicted_grad)/predicted_grad
		baddies = abs(err) > 0.1
		if baddies.any():
			coords = photonics_coords(source.x+dx, source.y+dy, source.z+dz,
			    geotime, 0, source)
			print('!!! Quantile gradient is screwy! (coords: %s)' % coords)
			print('predicted:')
			print(predicted_grad)
			print('measured:')
			print(grad)
			print('ratio:')
			print(grad/predicted_grad)
			failures += baddies.sum()
		elements += baddies.size
	if (elements > 0):
		print('------> %d/%d failed' % (failures,elements))
		return float(failures)/elements
	else:
		return 0.0
	

def check_jacobian(x, y, z, ps, t=0, tref=0, tol = 1e-2,
    geo=photonics_service.Geometry.SPHERICAL,
    parity=photonics_service.Parity.ODD):
	jacobian = photonics_service.getJacobian(x, y, z, ps, geo, parity)
	base_coords = photonics_coords(x, y, z, t, tref, ps, geo, parity)
	labels = ['x_s', 'y_s', 'z_s', 't_s', 'theta_s', 'phi_s']
	good = True
	
	def tweakme(ps, tweak):
		tweaked_source = tweak_source(ps, tweak)
		tweaked_coords = photonics_coords(x, y, z, t, tref+tweak[3], tweaked_source, geo, parity)
		return tweaked_coords
	
	eps = 1e-6
	for i in range(6):
		# tweak each coordinate
		tweak = n.zeros(6)
		tweak[i] = eps
		tweaked_coords = tweakme(ps, tweak)
		row = (tweaked_coords - base_coords)/tweak[i]
		jac_row = jacobian[i]
		err = n.zeros(6)
		ratio = n.zeros(6)
		mask = jac_row != 0
		imask = jac_row == 0
		err[mask] = (jac_row[mask] - row[mask])/jac_row[mask]
		err[imask] = row[imask]
		ratio[mask] = row[mask]/jac_row[mask]
		
		# print i, (tweaked_coords - base_coords)[2]
		
		if (abs(err) > tol).any():
			good = False
			coord = tweaked_coords
			cosAzi = n.cos(coord[1]*units.degree)
			if geo == photonics_service.Geometry.SPHERICAL:
				rho = coord[0]*n.sqrt(1-coord[2]**2)
			else:
				rho = coord[0]
			print('!!! Screwiness in d/d%s (rho = %e, cos(azi) = %e)' % (labels[i], rho, cosAzi))
			print('predicted:')
			print(jac_row)
			print('measured:')
			print(row)
			print('ratio:')
			print(ratio)
			good = False
	return good
	
def check_hessian_tensor(x, y, z, ps, t=0, tref=0, tol = 1e-2,
    geo=photonics_service.Geometry.SPHERICAL,
    parity=photonics_service.Parity.ODD, generator=photonics_service.getHessian):
	hessian = generator(x, y, z, ps, geo, parity)
	base_coords = photonics_coords(x, y, z, t, tref, ps, geo, parity)
	labels = ['x_s', 'y_s', 'z_s', 't_s', 'theta_s', 'phi_s']
	good = True
	
	eps = 1e-3
	def tweakme(ps, tweak):
		tweaked_source = tweak_source(ps, tweak)
		tweaked_coords = photonics_coords(x, y, z, t, tref+tweak[3], tweaked_source, geo, parity)
		return tweaked_coords
	def squaredance(ps, i, signi, j, signj):
		tweak = n.zeros(6)
		tweak[i] = signi*eps
		tweak[j] = signj*eps
		return tweakme(ps, tweak)
	for i in range(6):
		for j in range(i, 6):
			# print i,j
			# tweak each coordinate
			if i == j:
				# diagonal term
				tweak = n.zeros(6)
				tweak[i] = eps
				plus = tweakme(ps, tweak)
				tweak = n.zeros(6)
				tweak[i] = -eps
				minus = tweakme(ps, tweak)
				# print plus
				# print minus
				# print base_coords
				row = (plus + minus - 2*base_coords)/eps**2
			else:
				# a cross term
				num = squaredance(ps, i, +1, j, +1) + squaredance(ps, i, -1, j, -1) \
				    - squaredance(ps, i, +1, j, -1) - squaredance(ps, i, -1, j, +1)
				row = num/(4*eps**2)
				
			
			jac_row = hessian[i,j]
			err = n.zeros(6)
			mask = jac_row != 0
			imask = jac_row == 0
			err[mask] = (jac_row[mask] - row[mask])/jac_row[mask]
			err[imask] = row[imask]

			if (abs(err) > tol).any():
				good = False
				coord = base_coords
				cosAzi = n.cos(coord[1]*units.degree)
				if geo == photonics_service.Geometry.SPHERICAL:
					rho = coord[0]*n.sqrt(1-coord[2]**2)
				else:
					rho = coord[0]
				ratio = n.zeros(jac_row.shape)
				ratio[mask] = row[mask]/jac_row[mask]
				print('!!! Screwiness in d/d%s d%s (rho = %e, cos(azi) = %e)' % (labels[i], labels[j], rho, cosAzi))
				print('pred.', jac_row)
				print('meas.', row)
				print('ratio', ratio)
				print('error', err)
				good = False
	return good

def check_amplitude_hessian(pxs, x, y, z, ps, t=0, tref=0, tol = 1e-2,
    geo=photonics_service.Geometry.SPHERICAL,
    parity=photonics_service.Parity.ODD):
	# hessian = calc_hessian(x, y, z, ps, geo, parity)
	base_coords = photonics_coords(x, y, z, t, tref, ps, geo, parity)
	labels = ['x_s', 'y_s', 'z_s', 't_s', 'theta_s', 'phi_s']
	good = True

	pxs.SelectModuleCoordinates(x, y, z)
	amp_0 = pxs.SelectSource(ps)[0]
	
	print('amp: %.2e coords: %s' % (amp_0, base_coords))
	
	gradient, hessian = pxs.GetMeanAmplitudeHessian()
	
	print(gradient)
	print(hessian)	

	eps = n.array([2, 2, 2, 1e-2, 1e-2, 1e0])
	def tweakme(ps, tweak):
		tweaked_source = tweak_source_(ps, tweak)
		amp = pxs.SelectSource(tweaked_source)[0]
		return amp
	def squaredance(ps, i, signi, j, signj):
		tweak = n.zeros(6)
		tweak[i] = signi*eps[i]
		tweak[j] = signj*eps[j]
		return tweakme(ps, tweak)
	hessm = n.zeros((6,6))
	for i in range(6):
		for j in range(0, i+1):
			# print i,j
			# tweak each coordinate
			if i == j:
				# diagonal term
				tweak = n.zeros(6)
				tweak[i] = eps[i]
				plus = tweakme(ps, tweak)
				tweak = n.zeros(6)
				tweak[i] = -eps[i]
				minus = tweakme(ps, tweak)
				# print plus
				# print minus
				# print base_coords
				hessm[i][j] = (plus + minus - 2*amp_0)/eps[i]**2
			else:
				# a cross term
				num = squaredance(ps, i, +1, j, +1) + squaredance(ps, i, -1, j, -1) \
				    - squaredance(ps, i, +1, j, -1) - squaredance(ps, i, -1, j, +1)
				hessm[i][j] = num/(4*eps[i]*eps[j])
	
	print(hessian.shape)
	err = n.zeros((6,6))
	mask = n.tril(n.ones((6,6))).astype(bool) & (hessian != 0)
	err[mask] = (hessm[mask] - hessian[mask])/hessian[mask]

	if (abs(err) > 1e-2).any():
		good = False
		coord = base_coords
		cosAzi = n.cos(coord[1]*units.degree)
		if geo == photonics_service.Geometry.SPHERICAL:
			rho = coord[0]*n.sqrt(1-coord[2]**2)
		else:
			rho = coord[0]
		ratio = n.zeros(hessian.shape)
		ratio[mask] = hessm[mask]/hessian[mask]
		print('!!! Amplitude Hessian is screwy! (rho = %e, cos(azi) = %e)' % (rho, cosAzi))
		print('predicted')
		print(hessian)
		print('measured')
		print(hessm)
		print('ratio')
		print(ratio)
		good = False
	return good

def check_quantile_hessian(pxs, x, y, z, ps, t=0, tref=0, tol = 1e-2,
    geo=photonics_service.Geometry.SPHERICAL,
    parity=photonics_service.Parity.ODD):
	# hessian = calc_hessian(x, y, z, ps, geo, parity)
	base_coords = photonics_coords(x, y, z, t, tref, ps, geo, parity)
	labels = ['x_s', 'y_s', 'z_s', 't_s', 'theta_s', 'phi_s']
	good = True

	# tensor = photonics_service.getHessian(x, y, z, ps, geo, parity)
	# print tensor

	pxs.SelectModuleCoordinates(x, y, z)
	meanamp, distance, geotime = pxs.SelectSource(ps)

	t_0 = geotime
	# edges = geotime + 10 + n.linspace(0,n.sqrt(500),5)**2
	# nbins = edges.size - 1
	edges = geotime + n.array([100., 200.])
	nbins = 1
	
	print('amp: %.2e coords: %s' % (meanamp, base_coords))

	values, gradients, hessians = pxs.GetProbabilityQuantileHessians(edges, t_0)
	
	base_quantiles = pxs.GetProbabilityQuantiles(edges, t_0)

	# print gradients
	# print hessians	

	eps = n.array([2, 2, 2, 2, 1e-2, 1e-2])
	def tweakme(ps, tweak):
		tweaked_source = tweak_source(ps, tweak)
		# need to keep source time constant, so adjust reference time
		tref = pxs.SelectSource(tweaked_source)[2]
		quants = pxs.GetProbabilityQuantiles(edges, tref+tweak[3], 0)
		return quants
	def squaredance(ps, i, signi, j, signj):
		tweak = n.zeros(6)
		tweak[i] = signi*eps[i]
		tweak[j] = signj*eps[j]
		return tweakme(ps, tweak)
	hessm = n.zeros((nbins,6,6))
	for i in range(6):
		for j in range(0, i+1):
			# print i,j
			# tweak each coordinate
			if i == j:
				# diagonal term
				tweak = n.zeros(6)
				tweak[i] = eps[i]
				plus = tweakme(ps, tweak)
				tweak = n.zeros(6)
				tweak[i] = -eps[i]
				minus = tweakme(ps, tweak)
				# print plus
				# print minus
				# print base_coords
				hessm[:,i,j] = (plus + minus - 2*base_quantiles)/eps[i]**2
			else:
				# a cross term
				num = squaredance(ps, i, +1, j, +1) + squaredance(ps, i, -1, j, -1) \
				    - squaredance(ps, i, +1, j, -1) - squaredance(ps, i, -1, j, +1)
				hessm[:,i,j] = num/(4*eps[i]*eps[j])

	err = n.zeros((nbins, 6,6))
	mask = n.tril(n.ones((6,6))).reshape((1,6,6)).repeat(nbins, axis=0).astype(bool) & (hessians != 0)
	# mask = [slice(None)]*(hessm.ndim)
	err[mask] = (hessm[mask] - hessians[mask])/hessians[mask]

	if (abs(err) > tol).any():
		good = False
		coord = base_coords
		cosAzi = n.cos(coord[1]*units.degree)
		if geo == photonics_service.Geometry.SPHERICAL:
			rho = coord[0]*n.sqrt(1-coord[2]**2)
		else:
			rho = coord[0]
		ratio = n.zeros(hessians.shape)
		ratio[mask] = hessm[mask]/hessians[mask]
		print('!!! Quantile Hessian is screwy! (rho = %e, cos(azi) = %e)' % (rho, cosAzi))
		print('predicted')
		print(hessians)
		print('measured')
		print(hessm)
		print('ratio')
		print(ratio)
		good = False
	return abs(err) > tol



def check_amplitude_hessian_raw(table, base_coords):
	
	amp_0 = table.Eval(base_coords)

	hessian = table.EvalHessian(base_coords)
	hessian = hessian[:,:5][:5,:]
	
	eps = n.array([1.0, 1.0, 1e-2, 1.0, 1.0])
	def tweakme(coords, tweak):
		return table.Eval(coords+tweak)
	def squaredance(coords, i, signi, j, signj):
		tweak = n.zeros(5)
		tweak[i] = signi*eps[i]
		tweak[j] = signj*eps[j]
		return tweakme(coords, tweak)
	hessm = n.zeros((5,5))
	for i in range(5):
		for j in range(0, i+1):
			# print i,j
			# tweak each coordinate
			if i == j:
				# diagonal term
				tweak = n.zeros(5)
				tweak[i] = eps[i]
				plus = tweakme(base_coords, tweak)
				tweak = n.zeros(5)
				tweak[i] = -eps[i]
				minus = tweakme(base_coords, tweak)
				# print plus
				# print minus
				# print base_coords
				hessm[i][j] = (plus + minus - 2*amp_0)/eps[i]**2
			else:
				# a cross term
				num = squaredance(base_coords, i, +1, j, +1) + squaredance(base_coords, i, -1, j, -1) \
				    - squaredance(base_coords, i, +1, j, -1) - squaredance(base_coords, i, -1, j, +1)
				hessm[i][j] = num/(4*eps[i]*eps[j])

	err = n.zeros((5,5))
	mask = n.tril(n.ones((5,5))).astype(bool) & (hessian != 0)
	err[mask] = (hessm[mask] - hessian[mask])/hessian[mask]

	if (abs(err) > 1e-2).any():
		good = False
		ratio = n.zeros(hessian.shape)
		ratio[mask] = hessm[mask]/hessian[mask]
		print('!!! Raw amplitude Hessian is screwy! (coords: %s)' % (base_coords))
		print('predicted')
		print(hessian)
		print('measured')
		print(hessm)
		print('ratio')
		print(ratio)
		good = False
	return good
	
def check_quantile_hessian_raw(table, base_coords):
	t0 = base_coords
	t1 = base_coords.copy()
	t1[3] += 100
	
	amp_0 = table.Eval(t1) - table.Eval(t0)
	print('prob: %.2f coords: %s' % (amp_0, base_coords))

	hessian = table.EvalHessian(t1) - table.EvalHessian(t0)
	# hessian = hessian[:,:5][:5,:]

	eps = 1e-1*n.array([1, 1, 1e-2, 1, 1, 1])
	def tweakme(tweak):
		# print coords.shape, tweak.shape
		amp = table.Eval(t1 + tweak) - table.Eval(t0 + tweak)
		return amp
	def squaredance(i, signi, j, signj):
		tweak = n.zeros(6)
		tweak[i] = signi*eps[i]
		tweak[j] = signj*eps[j]
		return tweakme(tweak)
	hessm = n.zeros((6,6))
	for i in range(6):
		for j in range(0, i+1):
			# print i,j
			# tweak each coordinate
			if i == j:
				# diagonal term
				tweak = n.zeros(6)
				tweak[i] = eps[i]
				plus = tweakme(tweak)
				tweak = n.zeros(6)
				tweak[i] = -eps[i]
				minus = tweakme(tweak)
				# print plus
				# print minus
				# print base_coords
				hessm[i][j] = (plus + minus - 2*amp_0)/eps[i]**2
			else:
				# a cross term
				num = squaredance(i, +1, j, +1) + squaredance(i, -1, j, -1) \
				    - squaredance(i, +1, j, -1) - squaredance(i, -1, j, +1)
				hessm[i][j] = num/(4*eps[i]*eps[j])

	err = n.zeros((6,6))
	mask = n.tril(n.ones((6,6))).astype(bool) & (hessian != 0)
	err[mask] = (hessm[mask] - hessian[mask])/hessian[mask]

	if (abs(err) > 1e-2).any():
		good = False
		ratio = n.zeros(hessian.shape)
		ratio[mask] = hessm[mask]/hessian[mask]
		print('!!! Raw quantile Hessian is screwy! (coords: %s)' % (base_coords))
		print('predicted')
		print(hessian)
		print('measured')
		print(hessm)
		print('ratio')
		print(ratio)
		good = False
	return good





n.set_printoptions(precision=8, suppress=True, linewidth=200)
def test_jacobian(size=100, geo=photonics_service.Geometry.SPHERICAL,
    parity=photonics_service.Parity.ODD):
	xom, yom, zom = random_vertices(size)
	tref = 0
	t = 100
	for ps, x, y, z in zip(random_sources(size), xom, yom, zom):
		check_jacobian(x, y, z, ps, t=t, tref=tref, tol=1e-2, geo=geo, parity=parity)
		
def test_hessian_tensor(size=100, geo=photonics_service.Geometry.SPHERICAL,
    parity=photonics_service.Parity.ODD, generator=photonics_service.getHessian):
	xom, yom, zom = random_vertices(size)
	tref = 0
	t = 100
	for ps, x, y, z in zip(random_sources(size), xom, yom, zom):
		check_hessian_tensor(x, y, z, ps, t=t, tref=tref, tol=1e-2, geo=geo, parity=parity, generator=generator)

def test_amplitude_hessian_raw(size=100):
	table = amp_table()
	coords = n.random.uniform(size=size*5).reshape((size,5))
	coords[:,0] *= 500
	coords[:,1] *= 180 
	coords[:,2]  = 2*coords[:,2] - 1
	coords[:,3] *= 180
	coords[:,4]  = 1000*coords[:,4] - 500
	
	for base_coords in coords:
		check_amplitude_hessian_raw(table, base_coords)
		
def test_quantile_hessian_raw(size=100):
	table = timing_table()
	coords = n.random.uniform(size=size*6).reshape((size,6))
	coords[:,0] *= 500
	coords[:,1] *= 180 
	coords[:,2]  = 2*coords[:,2] - 1
	coords[:,3] *= 300
	coords[:,4] *= 180
	coords[:,5]  = 1000*coords[:,5] - 500

	for base_coords in coords:
		check_quantile_hessian_raw(table, base_coords)
		
def test_amplitude_hessian(size=100, geo=photonics_service.Geometry.SPHERICAL,
    parity=photonics_service.Parity.ODD):
	# xom, yom, zom = random_vertices(size)
	# xom, yom, zom = random_vertices(size)
	pxs = splines()
	dx_om, dy_om, dz_om = n.random.uniform(size=size*3, low=-100, high = 100).reshape((3,size))
	tref = 0
	t = 100
	for ps, dx, dy, dz in zip(random_sources(size), dx_om, dy_om, dz_om):
		if not check_hessian_tensor(ps.x+dx, ps.y+dy, ps.z+dz, ps, t=t, tref=tref, tol=1e-3):
			print("Hessian tensor is screwy, skipping!")
			continue
		check_amplitude_hessian(pxs, ps.x+dx, ps.y+dy, ps.z+dz, ps, t=t, tref=tref, tol=1e-2, geo=geo, parity=parity)
		
def test_quantile_hessian(size=100, geo=photonics_service.Geometry.SPHERICAL,
    parity=photonics_service.Parity.ODD):
	# xom, yom, zom = random_vertices(size)
	# xom, yom, zom = random_vertices(size)
	pxs = splines()
	dx_om, dy_om, dz_om = n.random.uniform(size=size*3, low=-100, high = 100).reshape((3,size))
	tref = 0
	t = 100
	failures = None
	trials = 0
	for ps, dx, dy, dz in zip(random_sources(size), dx_om, dy_om, dz_om):
		if not check_hessian_tensor(ps.x+dx, ps.y+dy, ps.z+dz, ps, t=t, tref=tref, tol=1e-3):
			print("Hessian tensor is screwy, skipping!")
			continue
		if not check_jacobian(ps.x+dx, ps.y+dy, ps.z+dz, ps, t=t, tref=tref, tol=1e-3):
			print("Jacobian is screwy, skipping!")
			continue
		# check_amplitude_hessian(pxs, ps.x+dx, ps.y+dy, ps.z+dz, ps, t=t, tref=tref, tol=1e-2, geo=geo, parity=parity)
		fail = check_quantile_hessian(pxs, ps.x+dx, ps.y+dy, ps.z+dz, ps, t=t, tref=tref, tol=1e-1, geo=geo, parity=parity)
		if failures is None:
			failures = fail.astype(float)
		else:
			failures += fail.astype(float)
		trials += 1
	print('Failures:')
	print(failures.astype(float)/trials)
			
if __name__ == "__main__":
	print('### Checking photonics <=> detector jacobian')
	test_jacobian(size=100)
	print('### Checking amplitude gradients')
	afail = test_amplitude_gradient(size=100)
	print('### Checking quantile gradients')
	qfail = test_quantile_gradient(size=100)
	print('Amplitude failures: %d%% Quantile failures: %d%%' % (afail*100, qfail*100))
	pass
