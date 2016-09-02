#!/usr/bin/env python

from I3Tray import *
from icecube import icetray, dataclasses, dataio, earthmodel_service 

import numpy as np
import matplotlib.pyplot as plt

def draw_density(service) :

    nx = 200
    x = np.linspace(0.0, service.atmo_radius, nx)
    print(x)
    y = [service.earth_density_in_cgs(dataclasses.I3Position(0.,0.,v)) for v in x]

    for index, item in enumerate(x):
        print("rad %f, dens %f" % (x[index], y[index]))

    plt.plot(x/1000, y, 'r.-')
    plt.xlabel('radius[km]')
    plt.ylabel('density [g/cm3]')
    plt.show()
    
# get service
service = earthmodel_service.EarthModelService("EarthModelService","",
                                               [], [], "SimpleIceCap", 
                                               20.0*I3Units.degree, 1948*I3Units.m);

print service.print_earth_params()
draw_density(service)



