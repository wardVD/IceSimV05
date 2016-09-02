#!/usr/bin/env python

from icecube import icetray, dataclasses, dataio, phys_services
from I3Tray import I3Tray
from os.path import expandvars

gcd = expandvars('$I3_TESTDATA/sim/GeoCalibDetectorStatus_IC80_DC6.54655.i3.gz')

import sys
outfile = sys.argv[1]

tray = I3Tray()

randomService = phys_services.I3SPRNGRandomService(1, 10000, 1)
tray.context['I3RandomService'] = randomService

from icecube.icetray import I3Units
from icecube.MuonGun import load_model, StaticSurfaceInjector, Cylinder, OffsetPowerLaw
from icecube.MuonGun.segments import GenerateBundles

# Use Hoerandel as a template for generating muons
model = load_model('Hoerandel5_atmod12_SIBYLL')
# Generate only single muons, no bundles
model.flux.max_multiplicity = 1
# Center the sampling surface on the barycenter of IC79 strings
surface = Cylinder(1600*I3Units.m, 800*I3Units.m, dataclasses.I3Position(31.25, 19.64, 0))
surface = Cylinder(1600*I3Units.m, 800*I3Units.m)
# Draw energies from an E^-2 power law broken at 1 TeV, from 10 TeV to 10 PeV
spectrum = OffsetPowerLaw(2, 1*I3Units.TeV, 10*I3Units.TeV, 10*I3Units.PeV)
# Set up the generator. This gets stored in a special frame for later reference
generator = StaticSurfaceInjector(surface, model.flux, spectrum, model.radius)

tray.AddSegment(GenerateBundles, 'MuonGenerator', Generator=generator, NEvents=10000, GCDFile=gcd)

icetray.logging.set_level_for_unit("I3PropagatorService", "TRACE")
icetray.logging.set_level_for_unit("I3PropagatorMMC", "TRACE")
icetray.logging.set_level_for_unit("I3PropagatorServiceMMC", "TRACE")

# The usual propagation foo
def make_propagators():
	from icecube.sim_services import I3ParticleTypePropagatorServiceMap
	from icecube.PROPOSAL import I3PropagatorServicePROPOSAL
	from icecube.cmc import I3CascadeMCService
	propagators = I3ParticleTypePropagatorServiceMap()
	muprop = I3PropagatorServicePROPOSAL(type=dataclasses.I3Particle.MuMinus, cylinderHeight=1200, cylinderRadius=700)
	cprop = I3CascadeMCService(phys_services.I3GSLRandomService(1)) # dummy RNG
	for pt in 'MuMinus', 'MuPlus':
		propagators[getattr(dataclasses.I3Particle.ParticleType, pt)] = muprop
	for pt in 'DeltaE', 'Brems', 'PairProd', 'NuclInt', 'Hadrons', 'EMinus', 'EPlus':
		propagators[getattr(dataclasses.I3Particle.ParticleType, pt)] = cprop
	return propagators
tray.AddModule('I3PropagatorModule', 'propagator', PropagatorServices=make_propagators(),
    RandomService=randomService, RNGStateName="RNGState")

tray.AddModule('I3Writer', 'writer',
    Streams=list(map(icetray.I3Frame.Stream, "SQP")),
    filename=outfile)

tray.AddModule('TrashCan', 'YesWeCan')
tray.Execute()
tray.Finish()
