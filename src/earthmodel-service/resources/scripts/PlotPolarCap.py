#!/usr/bin/env python

from I3Tray import *
from icecube import icetray, dataio, dataclasses, earthmodel_service 

import math
import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D

minthetadeg = float(0)
maxthetadeg = float(30.) 
earthmodel = 'PREM_mmc'
capangle = float(30.0 / 180. * math.pi)

service = earthmodel_service.EarthModelService("EarthModelService","",
                                               [earthmodel], [], "SimpleIceCap", 
                                               capangle, 1948*I3Units.m);

fig = plt.figure()
#ax = fig.add_subplot(111, projection='3D')
ax = fig.add_subplot(111)

n = 10000
mintheta = minthetadeg / 180. * math.pi
maxtheta = maxthetadeg / 180. * math.pi
rmin     = service.moho_boundary
rmax     = service.ice_air_boundary
print("rmin = %f, rmax = %f" % (rmin, rmax))

def randrange(n, vmin, vmax):
    return (vmax-vmin)*np.random.rand(n) + vmin

service.ice_cap_type_string = 'SimpleIceCap'
service.ice_cap_simple_angle = capangle

ar  = randrange(n, rmin, rmax)
ath = randrange(n, mintheta, maxtheta)
aph = randrange(n, 0, 2*math.pi)

x2 = []
y2 = []
z2 = []

for i, item in enumerate(ar) :
    x = ar[i]*np.sin(ath[i])*np.cos(aph[i])
    y = ar[i]*np.sin(ath[i])*np.sin(aph[i])
    z = ar[i]*np.cos(ath[i])
    p = dataclasses.I3Position(x, y, z)
    medtype = service.medium(p)
    if medtype == earthmodel_service.rock :
        x2.append(x)
        y2.append(y)
        z2.append(z)

ax.scatter(x2, y2, z2, c='b', marker='o')

ax.set_xlabel('X [m]')
ax.set_ylabel('Y [m]')
#ax.set_zlabel('Z [m]')

plt.show()


