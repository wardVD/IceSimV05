#!/usr/bin/env python
import numpy

from math import pi as PI

# this is simply 2pi * alpha
FACTOR = 2.0 * PI * 1.0/137.0

# these are the constants from the wavelength
# dependent index of refraction where lambda
# is required to be in units of microns
# n(lambda) = /Sigma_{i} C_i * lambda^i
C_0 = 1.55749  # unitless
C_1 = -1.57988 # units of micron ^-1
C_2 = 3.99993  # units of micron ^-2
C_3 = -4.68271 # units of micron ^-3
C_4 = 2.09354  # units of micron ^-4

c = numpy.array([C_0,C_1,C_2,C_3,C_4])

def integrand(lmbda):
    a = numpy.array([lmbda**i for i in range(5)])
    f = 1.0 - 1.0/(sum(c * a))**2
    return FACTOR * f / lmbda**2

# lambda in units of microns
dlmbda = 0.000001
lmbda_grid = numpy.arange(0.3,0.6,dlmbda)
integral = 0
for l in lmbda_grid:
    integral += integrand(l) * dlmbda

print(integral * 1e6) # print in units of 
