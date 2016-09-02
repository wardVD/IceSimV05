#!/usr/bin/env python

from I3Tray import *
from icecube import icetray, dataio, dataclasses, earthmodel_service 
import math
import numpy as np

# get service

#
# for logging
#
icetray.I3Logger.global_logger = icetray.I3PrintfLogger()
icetray.set_log_level(icetray.I3LogLevel.LOG_INFO)
#icetray.set_log_level_for_unit("EarthModelService",icetray.I3LogLevel.LOG_TRACE)

#tray = I3Tray()

#tray.Add("I3EarthModelServiceFactory","earthmodel")
service = earthmodel_service.EarthModelService("EarthModelService","",
                                               [], [], "SimpleIceCap", 
                                               20.0*I3Units.degree, 1948*I3Units.m);

elog10 = 12
#elog10 = -0.3 #0.5GeV
#elog10 = -0.5 #0.3GeV
#elog10 = -0.7 #0.2GeV
energy = math.pow(10, elog10)
#isTau = True
isTau = False
isReverse = True
scale = 1

range = earthmodel_service.EarthModelCalculator.lepton_range(
                            energy, isTau,
                            earthmodel_service.EarthModelCalculator.default,
                            scale)   # scale

print("energy 1e%f range %f[mwe]" % (elog10, range))

# upgoing 

pos = dataclasses.I3Position(0, 0, 0)
dir = dataclasses.I3Direction(0, 0, 1)

print("set position (%f,%f,%f) and direction (%f,%f,%f)" % (pos.x, pos.y, pos.z, dir.x, dir.y, dir.z))

range_meter =  service.lepton_range_in_meter_to(
                    pos, 
                    dir,
                    energy,
                    isTau,
                    earthmodel_service.EarthModelCalculator.default,
                    scale)

cdep = earthmodel_service.EarthModelCalculator.mwe_to_column_depth_cgs(range)
dist = service.distance_for_column_depth_to_point(pos, dir, cdep)
print("upgoing to pos : energy 1e%f range %f[mwe], %f[m], cdep %f[g/cm2], dist(recal) %f[m]" % (elog10, range, range_meter, cdep, dist))

if (range_meter - dist > 1*I3Units.m) :
    log_error("result of lepton_range_in_meter_to and distance_for_column_depth_to_point doesn't match! range_meter %f, dist %f" %(range_meter, cdep)) 

pos = dataclasses.I3Position(0, 0, -2000)
ext = dataclasses.I3Position(0, 0, 0)

cdep =  service.column_depth_in_cgs(pos, ext)

print("cdep calculation from (%f,%f,%f) to (%f,%f,%f) = %f [g/cm2]" % (pos.x, pos.y, pos.z, ext.x, ext.y, ext.z, cdep))

pos = dataclasses.I3Position(0, 0, 1000)
ext = dataclasses.I3Position(0, 0, 0)

cdep =  service.column_depth_in_cgs(pos, ext)

print("cdep calculation from (%f,%f,%f) to (%f,%f,%f) = %f [g/cm2]" % (pos.x, pos.y, pos.z, ext.x, ext.y, ext.z, cdep))

pos = dataclasses.I3Position(0, 0, 4000)

cdep =  service.column_depth_in_cgs(pos,ext)

print("cdep calculation from (%f,%f,%f) to (%f,%f,%f) = %f [g/cm2]" % (pos.x, pos.y, pos.z, ext.x, ext.y, ext.z, cdep))


# holizontal

pos = dataclasses.I3Position(0, 0, 0)
dir = dataclasses.I3Direction(-1, 0, 0)

print("set position (%f,%f,%f) and direction (%f,%f,%f)" % (pos.x, pos.y, pos.z, dir.x, dir.y, dir.z))

range_meter =  service.lepton_range_in_meter_to(
                    pos, 
                    dir,
                    energy,
                    isTau,
                    earthmodel_service.EarthModelCalculator.default,
                    scale)

cdep = earthmodel_service.EarthModelCalculator.mwe_to_column_depth_cgs(range)
dist = service.distance_for_column_depth_to_point(pos, dir, cdep)

print("horizontal to pos : energy 1e%f range %f[mwe], %f[m], cdep %f[g/cm2], dist(recal) %f[m]" % (elog10, range, range_meter, cdep, dist))

pos = dataclasses.I3Position(2000, 0, 0)
ext = dataclasses.I3Position(0, 0, 0)

cdep =  service.column_depth_in_cgs(pos,ext)

print("cdep calculation from (%f,%f,%f) to (%f,%f,%f) = %f [g/cm2]" % (pos.x, pos.y, pos.z, ext.x, ext.y, ext.z, cdep))


print("search largest distance for muon range of neutrino with energy 1e%f ....." %  (elog10))

minzen = 0 
maxzen = 180
dist = 0
at_zen = minzen

pos = dataclasses.I3Position(0, 0, 0)

zenith = np.arange(minzen, maxzen, 0.01)
zenrad = np.array([math.radians(i) for i in zenith])

for z in zenrad:
    dir = dataclasses.I3Direction(z, 0)
    cdep = earthmodel_service.EarthModelCalculator.mwe_to_column_depth_cgs(range)
    curdist = service.distance_for_column_depth_to_point(pos, dir, cdep)
    '''
    curdist =  service.lepton_range_in_meter_to(
                    pos, 
                    dir,
                    energy,
                    isTau,
                    earthmodel_service.EarthModelCalculator.default,
                    scale)

    '''

    if (curdist > dist) :
        dist = curdist
        at_zen = math.degrees(z)


print("largest dist for energy 1e%f is %g [km] at %f [deg]" % (elog10, dist/I3Units.km, at_zen))







