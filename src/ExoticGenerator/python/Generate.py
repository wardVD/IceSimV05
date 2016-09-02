#!/usr/bin/env python
import time, math, os, random
from I3Tray import I3Tray, I3Units, NaN
from icecube import icetray, dataio, dataclasses, phys_services
from icecube.exoticgenerator.GenerateUtils import GenerateUtils
from random import uniform

utils = GenerateUtils()

class Generate(icetray.I3Module):
    def __init__(self,ctx):
        icetray.I3Module.__init__(self,ctx)
        self.AddParameter('Type','Type of the particle you want to generate',[dataclasses.I3Particle.STauMinus,dataclasses.I3Particle.STauPlus])
        self.AddParameter('Mass','Mass in GeV of the particle you want to generate',100/I3Units.GeV)
        self.AddParameter('XYZPosition', 'Vector of initial X,Y,Z position', [0,0,0])
        self.AddParameter('Direction', 'Vector of initial direction', [math.pi/2.2,0])
        self.AddParameter('Energy', 'Initial energy (I3Units)', 1000*I3Units.TeV)
        self.AddParameter('Time', 'Initial time (I3Units)', 0*I3Units.ns)
        self.AddParameter('TreeName', 'Name of the tree', "I3MCTree")
        self.AddParameter("Disk_dist","Distance of disk from center",1000.*I3Units.m)
        self.AddParameter("Disk_rad","Radius of Generation Disk",800.*I3Units.m)
        self.AddParameter("Rand_dir","Randomize Direction",True)
        self.AddParameter("Zenith","Stau zenith",NaN)
        self.AddParameter("Azimuth","Stau azimuth",NaN)
        self.AddParameter("ZenithMin","Lower zenith bound",0.*I3Units.deg)
        self.AddParameter("ZenithMax","Upper zenith bound",180.*I3Units.deg)
        self.AddParameter("AzimuthMin","Lower azimuth bound",0.*I3Units.deg)
        self.AddParameter("AzimuthMax","Upper azimuth bound",360.*I3Units.deg)

        self.AddOutBox("OutBox")

    def Geometry(self,frame):
        assert 'I3Geometry' in frame, "Input a GCD!"
        self.PushFrame(frame)

    def Configure(self):
        self.Type        = self.GetParameter('Type')
        self.Mass        = self.GetParameter('Mass')
        self.XYZPosition = self.GetParameter('XYZPosition')
        self.Direction   = self.GetParameter('Direction')
        self.Energy      = self.GetParameter('Energy')
        self.Time        = self.GetParameter('Time')
        self.Disk_dist   = self.GetParameter('Disk_dist')
        self.Disk_rad    = self.GetParameter('Disk_rad')
        self.Rand_dir    = self.GetParameter('Rand_dir')
        self.Zenith      = self.GetParameter('Zenith')
        self.Azimuth     = self.GetParameter('Azimuth')
        self.ZenithMin   = self.GetParameter('ZenithMin')
        self.ZenithMax   = self.GetParameter('ZenithMax')
        self.AzimuthMin  = self.GetParameter('AzimuthMin')
        self.AzimuthMax  = self.GetParameter('AzimuthMax')

        assert (self.Disk_dist > 0 or self.Disk_dist < 5.*I3Units.km), "Disk_dist (%f m) out of range" %(self.Disk_dist/I3Units.m)
        assert self.Disk_rad > 0, "Disk_rad (%f m) out of range" %(self.Disk_rad/I3Units.m)
        if self.Rand_dir: 
            assert not (math.isnan(self.ZenithMin) or math.isnan(self.ZenithMax) or math.isnan(self.AzimuthMin) \
                        or math.isnan(self.AzimuthMax)), "Please set angle min/max correctly"
        else: 
            assert not (math.isnan(self.Zenith) or math.isnan(self.Azimuth)), "Please set angles correctly"
            assert (self.Zenith/I3Units.deg >= 0. and self.Zenith/I3Units.deg <= 180.), "Set Zenith correctly"
            assert (self.Azimuth/I3Units.deg >= 0. and self.Azimuth/I3Units.deg <= 360.), "Set Azimuth correctly"
        assert (self.ZenithMin/I3Units.deg >= 0. and self.ZenithMin/I3Units.deg <= 180.), "Set ZenithMin correctly"
        assert (self.AzimuthMin/I3Units.deg >= 0. and self.AzimuthMin/I3Units.deg <= 360.), "Set AzimuthMin correctly"
        assert (self.ZenithMax/I3Units.deg >= 0. and self.ZenithMax/I3Units.deg <= 180.), "Set ZenithMin correctly"
        assert (self.AzimuthMax/I3Units.deg >= 0. and self.AzimuthMax/I3Units.deg <= 360.), "Set AzimuthMin correctly"
        assert (self.ZenithMin/I3Units.deg < self.ZenithMax/I3Units.deg), "ZenithMin should be smaller than ZenithMax"
        assert (self.AzimuthMin/I3Units.deg < self.AzimuthMax/I3Units.deg), "AzimuthMin should be smaller than AzimuthMax"

    def DAQ(self, frame):
        randomserv = phys_services.I3GSLRandomService(int(uniform(0,1000)))

        StauDir = utils.RandomizeDirection(randomserv,zenithMin = self.ZenithMin, zenithMax = self.ZenithMax, azimuthMin =self.AzimuthMin , azimuthMax = self.AzimuthMax) if self.Rand_dir \
            else dataclasses.I3Direction(self.Zenith,self.Azimuth) 

        StauPos = utils.RandomizePosition(randomserv, direction = StauDir, diskDist = self.Disk_dist, diskRad = self.Disk_rad)
            
        particle = dataclasses.I3Particle()
        particle.type = random.sample(self.Type,1)[0]
        #particle.pos = dataclasses.I3Position(self.XYZPosition[0],self.XYZPosition[1],self.XYZPosition[2])
        #particle.dir = dataclasses.I3Direction(self.Direction[0],self.Direction[1])
        particle.pos = StauPos
        particle.dir = StauDir
        particle.energy = self.Energy
        particle.time = self.Time

        frame['Primary'] = particle
        #STau particle cannot have mass
        frame['PrimaryMass'] = dataclasses.I3Double(self.Mass)

        self.PushFrame(frame, 'OutBox')

        return 1
