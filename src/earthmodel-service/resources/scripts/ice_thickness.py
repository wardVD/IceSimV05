#!/usr/bin/env python
#
# pybinding load test
#

from I3Tray import *
from icecube import icetray, dataclasses, earthmodel_service

import numpy as np
import math as m
import matplotlib as mpl
import matplotlib.pyplot as plt
import pylab as P

#
# for logging
#
icetray.I3Logger.global_logger = icetray.I3PrintfLogger()
icetray.set_log_level(icetray.I3LogLevel.LOG_WARN)

minzen = 80 
maxzen = 100

# 
# generate earthmodel service
#

earthmodel = ["PREM_mmc"]
materialmodel = ["Standard"]
earth1 = earthmodel_service.EarthModelService("EarthModelService","",
                              earthmodel, materialmodel, 
                              "IceSheet",
                              30.*I3Units.degree, 1948*I3Units.m)

earth2 = earthmodel_service.EarthModelService("EarthModelService","",
                              earthmodel, materialmodel, 
                              "SimpleIceCap",
                              30.*I3Units.degree, 1948*I3Units.m)

earth3 = earthmodel_service.EarthModelService("EarthModelService","",
                              earthmodel, materialmodel, 
                              "SimpleIceCap",
                              20.*I3Units.degree, 1948*I3Units.m)

earth4 = earthmodel_service.EarthModelService("EarthModelService","",
                              earthmodel, materialmodel, 
                              "SimpleIceCap",
                              10.*I3Units.degree, 1948*I3Units.m)

earth5 = earthmodel_service.EarthModelService("EarthModelService","",
                              earthmodel, materialmodel, 
                              "SimpleIceCap",
                              5.*I3Units.degree, 1948*I3Units.m)
 
#zenith = np.arange(minzen, maxzen, 0.5)
#zenith = np.arange(minzen, maxzen, 1.0)
zenith = np.arange(minzen, maxzen, 0.01)
zenrad = np.array([m.radians(i) for i in zenith])

def dist (earth, zen) :
    return earth.distance_from_earth_entrance_to_detector(zen)

def mwe (earth, zen) :
    distance = dist(earth, zen)
    p1 = dataclasses.I3Position(0, 0, 0)
    dir = dataclasses.I3Direction(zen, 0)
    p2 = -distance * dir
    #print("column depth from p2(%f,%f,%f) to p1(%f,%f%f)" % (p2.x, p2.y, p2.z, p1.x, p1.y, p1.z))
    cdep = earth.column_depth_in_cgs(p2, p1);
    return cdep / I3Units.m * I3Units.cm

dist1 = np.array([mwe(earth1, i) for i in zenrad])
dist2 = np.array([mwe(earth2, i) for i in zenrad])
dist3 = np.array([mwe(earth3, i) for i in zenrad])
dist4 = np.array([mwe(earth4, i) for i in zenrad])
dist5 = np.array([mwe(earth5, i) for i in zenrad])

# prepare figure
fig1 = P.figure()

ax1 = fig1.add_subplot(1,2,1)

ax1.plot(zenith, dist1, '-', color="red", label="IceSheet",linewidth=1.5)
ax1.plot(zenith, dist2, '-', color="blue", label="IceCap 30deg",linewidth=1.5)
ax1.plot(zenith, dist3, '-', color="orange", label="IceCap 20deg",linewidth=1.5)
ax1.plot(zenith, dist4, '-', color="green", label="IceCap 10deg",linewidth=1.5)
ax1.plot(zenith, dist5, '-', color="cyan", label="IceCap 5deg",linewidth=1.5)
ax1.set_yscale('log')
ax1.set_ylabel('Material Thickness [m.w.e]')
ax1.set_xlabel('track zenith angle[deg]')
#ax1.set_ylim([1e4, 1e8])
ax1.grid()
ax1.legend(loc=2, prop={'size':12})

ax2 = fig1.add_subplot(1,2,2)
ax2.plot(zenith, dist1/dist1, '-', color="red", label="IceSheet",linewidth=1.5)
ax2.plot(zenith, dist2/dist1, '-', color="blue", label="IceCap 30deg",linewidth=1.5)
ax2.plot(zenith, dist3/dist1, '-', color="orange", label="IceCap 20deg",linewidth=1.5)
ax2.plot(zenith, dist4/dist1, '-', color="green", label="IceCap 10deg",linewidth=1.5)
ax2.plot(zenith, dist5/dist1, '-', color="cyan", label="IceCap 5deg",linewidth=1.5)
ax2.set_ylim([0.92, 1.02])
ax2.set_ylabel('Material Thickness ratio')
ax2.set_xlabel('track zenith angle[deg]')
ax2.grid()
ax2.legend(loc=1, prop={'size':12})

P.show()





