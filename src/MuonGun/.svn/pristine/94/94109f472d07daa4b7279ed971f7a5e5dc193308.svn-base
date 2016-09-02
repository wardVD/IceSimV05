#!/usr/bin/env python

from icecube import icetray, dataclasses, dataio
from icecube import phys_services, sim_services, simclasses, MuonGun
from I3Tray import I3Tray

tray = I3Tray()

tray.AddModule('I3InfiniteSource', 'driver')
tray.AddService('I3GSLRandomServiceFactory', 'rng', Seed=1337)

surface  = MuonGun.Cylinder(1600, 800)
flux     = MuonGun.BMSSFlux()
flux.min_multiplicity = 1
flux.max_multiplicity = 1
energies = MuonGun.BMSSEnergyDistribution()
radii    = MuonGun.BMSSRadialDistribution()
generator = 10*MuonGun.StaticSurfaceInjector(surface, flux, MuonGun.OffsetPowerLaw(2, 500., 50, 1e6), radii)

tray.AddModule('I3MuonGun::GeneratorModule', 'generator', Generator=generator)
tray.AddModule('I3MuonGun::WeightCalculatorModule', 'weight',
    Model=MuonGun.BundleModel(flux, radii, energies),
    Generator=generator)

def check_weight(frame):
    weight = frame['weight'].value
    assert weight > 0
tray.Add(check_weight, Streams=[icetray.I3Frame.DAQ])

tray.AddModule('TrashCan', 'YesWeCan')
tray.Execute()
tray.Finish()
