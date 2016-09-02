#!/usr/bin/env python

try:
	from icecube import icetray, photonics_service
	from icecube.photonics_service import Geometry, Parity

	from coordinates import random_sources, photonics_coords, tweak_source, OpenStruct, report_badness, finitediff_bsearch
	import numpy

	units = icetray.I3Units
	
	def check_jacobian(pos, source, tol = 1e-2, geo=Geometry.SPHERICAL, parity=Parity.ODD):
		jacobian = photonics_service.getJacobian(pos.x, pos.y, pos.z, source, geo, parity)
		base_coords = photonics_coords(pos, source, geo=geo, parity=parity)
		if geo is Geometry.SPHERICAL:
			pcoords = ['r', 'azi', 'cos(p)', 't', 'zen', 'z'] 
		elif geo is Geometry.CYLINDRICAL:
			pcoords = ['rho', 'azi', 'l', 't', 'zen', 'z'] 
		labels = ['x_s', 'y_s', 'z_s', 't_s', 'zen_s', 'azi_s']
		good = numpy.ones((6,6), dtype=numpy.bool)
	
		def tweakme(source, tweak):
			tweaked_source = tweak_source(source, tweak)
			tweaked_coords = photonics_coords(pos, tweaked_source, tref=tweak[3], geo=geo, parity=parity)
			return tweaked_coords

		for j in range(6):
			scales = [10.0, 10.0, 10.0, 10.0, numpy.pi/10, numpy.pi/5]
			grad = numpy.zeros(6)
			steps = numpy.zeros(6)
			predicted_grad = jacobian[j]

			for i in range(6):

				def get_gradient(eps):
					tweak = numpy.zeros(6)
					tweak[j] = eps/2.0

					tweak_plus = tweakme(source, tweak)[i]
					tweak_minus = tweakme(source, -tweak)[i]

					grad = (tweak_plus - tweak_minus)/eps
					
					return grad

				def test_gradient(grad):
					
					if predicted_grad[i] != 0:
						err = (grad - predicted_grad[i])/predicted_grad[i]
					else:
						err = grad
					return abs(err) <= tol

				grad[i], steps[i], good[j][i] = finitediff_bsearch(0, float(scales[i]), get_gradient, test_gradient)
		
			if not good[j].all():
				cosAzi = numpy.cos(base_coords[1]*units.degree)
				if geo == Geometry.SPHERICAL:
					rho = base_coords[0]*numpy.sqrt(1-base_coords[2]**2)
				else:
					rho = base_coords[0]
				dlabels = ['d%s/d%s' % (l, labels[j]) for l in pcoords]
				title = 'Screwiness in d/d%s (rho = %e, cos(azi) = %e)' % (labels[j], rho, cosAzi)
				report_badness(title, dlabels, predicted_grad, grad, steps, good[j])
	
		return good
		
	if __name__ == "__main__":
		import sys
		tol = 1e-7

		print('Checking detector/photonics Jacobian by finite differences...')
		for geo in (Geometry.SPHERICAL, Geometry.CYLINDRICAL):
			for parity in (Parity.ODD, Parity.EVEN):
				nruns = 1000
				nfail = numpy.zeros((6,6), dtype=int)
				for source in random_sources(nruns):
					pos = OpenStruct(x=0., y=0., z=0.)
					good = check_jacobian(pos, source, tol=tol, geo=geo, parity=parity)
					nfail += numpy.logical_not(good)
					
				print('==> %s geometry/%s parity: %d/%d failures (tol: %.2e)' % (geo, parity, nfail.max(), nruns, tol))
				print('==> by coordinate:')
				print(nfail)
				# Under normal circumstances 
				if nfail.max() > 10:
					sys.exit(1)
		try:
			# Cubic geometries are not supported, so this should fail
			pos = OpenStruct(x=0., y=0., z=0.)
			check_jacobian(pos, source, tol=1e-4, geo=Geometry.CUBIC, parity=Parity.ODD)
			print('FAIL: If cubic geometries are supported, then we need a unit test!')
			sys.exit(1)
		except:
			pass

except ImportError:
	import sys
	print("%s: skipping tests..." % sys.exc_info()[1])

