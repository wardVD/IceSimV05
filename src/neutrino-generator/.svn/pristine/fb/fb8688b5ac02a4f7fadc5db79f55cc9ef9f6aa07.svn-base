#!/usr/bin/env python
from I3Tray import *
import sys
from icecube.neutrino_generator.I3Test import *

from icecube import icetray, dataclasses

class TestAngularDist(icetray.I3Module):

    def __init__(self, context):
        icetray.I3Module.__init__(self, context)
        self.AddOutBox("OutBox")

        self.AddParameter("ZenithMin","minimum zenith angle", 0. * I3Units.degree)
        self.AddParameter("ZenithMax","maximum zenith angle", 180.* I3Units.degree)
        self.AddParameter("AzimuthMin","minimum azimuth angle", 0.* I3Units.degree)
        self.AddParameter("AzimuthMax","maximum azimuth angle", 360.* I3Units.degree)

        self.zenith_l = list()
        self.azimuth_l = list()

    def ENSURE(self.cond, msg):
        if not cond :
            print(msg)
            print("FAIL")
            sys.exit(1)

    def ENSURE_EQUAL(self.x,y, msg):
        if not x == y :
            print(msg)
            print("FAIL")
            sys.exit(1)

    def ENSURE_DISTANCE(self.x,y, d, msg):
        if abs(x - y) > d :
            print(msg)
            print("FAIL")
            sys.exit(1)

    def ENSURE_FLAT(self.test_values_l, msg):
        try:
            import numpy

            self.ENSURE(len(test_values_l) > 0, "this list is empty\n" + msg)
                    
            n, bins = numpy.histogram(test_values_l)
            # statistical errors
            sigma = numpy.sqrt(n)
             
            # the bin expectation value for a flat distribution
            bin_expectation = float(len(test_values_l))/(len(bins))
            # number of degrees of freedom
            NDOF = float( len(bins) - 1 )
            # reduced chi**2
            red_chi2 = sum([ ((v - bin_expectation)/sig)**2 for v,sig in zip(n,sigma) if sig > 0])/NDOF
            self.ENSURE( red_chi2 < 2, "reduced chi^2 = %f\n%s" % (red_chi2,msg) )

        except ImportError:
            self.ENSURE(False, "numpy not installed")
        
    def Configure(self):
        self.zenithMin = self.GetParameter("ZenithMin")
        self.zenithMax = self.GetParameter("ZenithMax")
        self.azimuthMin = self.GetParameter("AzimuthMin")
        self.azimuthMax = self.GetParameter("AzimuthMax")
        
    def DAQ(self, frame):
    
       tree = frame.Get("I3MCTree")

       for p in tree.primaries:
           self.ENSURE(p.dir.zenith >= self.zenithMin, "zenith > zenithMin, zenith = %f  zenithMin = %f" % \
                  (p.dir.zenith/I3Units.degree, self.zenithMin/I3Units.degree) )
           self.ENSURE(p.dir.zenith <= self.zenithMax, "zenith > zenithMax, zenith = %f  zenithMax = %f" % \
                  (p.dir.zenith/I3Units.degree, self.zenithMax/I3Units.degree) )
           self.ENSURE(p.dir.azimuth >= self.azimuthMin, "azimuth > azimuthMin, azimuth = %f  azimuthMin = %f" % \
                  (p.dir.azimuth/I3Units.degree, self.azimuthMin/I3Units.degree) )
           self.ENSURE(p.dir.azimuth <= self.azimuthMax, "azimuth > azimuthMax, azimuth = %f  azimuthMax = %f" % \
                  (p.dir.azimuth/I3Units.degree, self.azimuthMax/I3Units.degree) )

           self.zenith_l.append(p.dir.zenith)
           self.azimuth_l.append(p.dir.azimuth)
       

    def Finish(self):
        from math import cos
        cos_zenith_l = [cos(zen/I3Units.radian) for zen in self.zenith_l]
        
        self.ENSURE_FLAT(cos_zenith_l, "cos(zenith) is not flat")
        self.ENSURE_FLAT(self.azimuth_l, "azimuth is not flat")

#-----------
# start test
#-----------

from I3Tray import *
from icecube import icetray, dataclasses, phys_services, sim_services, dataio, earthmodel_service, neutrino_generator

tray = I3Tray()
random = phys_services.I3GSLRandomService(42)
tray.context['I3RandomService'] = random

tray.AddModule("I3InfiniteSource", "source",
               prefix = i3_testdata + "/sim/GeoCalibDetectorStatus_IC86.55380_corrected.i3.gz")

tray.AddModule("I3MCEventHeaderGenerator","ev")
tray.AddService("I3EarthModelServiceFactory", "EarthModelService")

tray.AddService("I3NuGSteeringFactory", "steering",
                NEvents = 1000,
                EarthModelName = "EarthModelService",
                InjectionMode = "Surface",
                )

tray.AddService("I3NuGInjectorFactory", "injector",
                RandomService = random,
                SteeringName = "steering")

tray.AddService("I3NuGInteractionInfoFactory", "interaction",
                RandomService = random,
                SteeringName = "steering")

tray.AddModule("I3NeutrinoGenerator","generator",
                RandomService = random,
                SteeringName = "steering",
                InjectorName = "injector",
                InteractionInfoName = "interaction"
              )

#tray.AddModule("Dump", "dump")
tray.AddModule("TrashCan", "the can")

tray.Execute()
tray.Finish()


