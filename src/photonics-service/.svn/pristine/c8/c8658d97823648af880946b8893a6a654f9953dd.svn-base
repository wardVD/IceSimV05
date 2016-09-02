#!/usr/bin/env python

import os
import math
import numpy as np
from I3Tray import *
from icecube import icetray,dataclasses,phys_services,photonics_service
from icecube.simprod.segments.PropagateMuons import PropagateMuons

CylinderRadius = 800.
ZenithMin = 0*I3Units.degree
ZenithMax = 180*I3Units.degree
MuonEnergy = 100 * I3Units.TeV
mu_type =dataclasses.I3Particle.MuPlus

gcdfile = os.path.join(os.environ['I3_TESTDATA'],"sim",
                       "GeoCalibDetectorStatus_2012.56063_V0.i3.gz")
TablePath = "/cvmfs/icecube.opensciencegrid.org/data/photon-tables/splines"
amplitudetable = TablePath+'/ems_mie_z20_a10.abs.fits'
timingtable = TablePath+'/ems_mie_z20_a10.prob.fits'

if not os.path.exists(TablePath):
	print "tables not found, skipping test"
	sys.exit(0)

def muon_injector(frame):
    zenith  = randomService.uniform(ZenithMin,ZenithMax)
    azimuth = randomService.uniform(0,360*I3Units.degree)
    x = CylinderRadius*math.cos(azimuth)
    y = CylinderRadius*math.sin(azimuth)
    z = CylinderRadius*math.cos(zenith)

    global mu_type
    if mu_type==dataclasses.I3Particle.MuMinus:
        mu_type=dataclasses.I3Particle.MuPlus
    else:
        mu_type = dataclasses.I3Particle.MuMinus
    
    mu        = dataclasses.I3Particle()
    mu.type   = mu_type
    mu.pos    = dataclasses.I3Position(x,y,z)
    mu.dir    = dataclasses.I3Direction(zenith,azimuth)
    mu.energy = MuonEnergy
    mu.time   = 0 * I3Units.ns
    mu.length = NaN
    mu.location_type = dataclasses.I3Particle.InIce

    mctree = dataclasses.I3MCTree()
    mctree.add_primary(mu)
    frame["I3MCTree_preMuonProp"]=mctree

def test(frame):
    residuals = []
    geo = frame['I3Geometry'].omgeo
    primary = dataclasses.I3MCTree.get_primaries(frame['I3MCTree_preMuonProp'])[0]
    for om,pes in frame['I3MCPESeriesMap']:
        pos = geo[om].position
        residuals += [phys_services.I3Calculator.time_residual(primary,pos,p.time) for p in pes]

    l = len(residuals)
    assert ( l > 900 and l <6000)
    m  = np.mean(residuals)
    assert ( m >200 and m <500)
    s = np.std(residuals)
    assert (s >300 and s < 600)

randomService = phys_services.I3SPRNGRandomService(
    seed = 12345,
    nstreams = 10000,
    streamnum = 1)

cascade_service = photonics_service.I3PhotoSplineService(
        amplitudetable=amplitudetable,
        timingtable=timingtable,
        timingSigma=0.)

tray = I3Tray()

tray.AddModule("I3InfiniteSource", "source",
               prefix = gcdfile)

tray.AddModule(muon_injector,
               Streams=[icetray.I3Frame.DAQ])

tray.AddSegment(PropagateMuons, 
                RandomService = randomService
                )

tray.AddModule("I3MCTreeHybridSimulationSplitter")

tray.AddModule("I3PhotonicsHitMaker", 
               CascadeService = cascade_service,
               TrackService = None, 
               Input = "I3MCTreeCascades",
               RandomService = randomService)

tray.AddModule(test,Streams=[icetray.I3Frame.DAQ])

tray.Execute(13)
tray.Finish()

