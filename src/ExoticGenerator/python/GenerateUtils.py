#!/usr/bin/env python 

from math import cos, sin, acos, sqrt, pi
from I3Tray import I3Units
from icecube import dataclasses, icetray, dataio

class GenerateUtils:
    def RandomizeDirection(self,random,zenithMin=None,zenithMax=None,azimuthMin=None,azimuthMax=None):
        cosZenith = random.uniform(cos(zenithMax/I3Units.radian), cos(zenithMin/I3Units.radian))
        zenith    = acos(cosZenith)
        azimuth   = random.uniform(azimuthMin/I3Units.radian, azimuthMax/I3Units.radian)
        direction = dataclasses.I3Direction(zenith,azimuth)

        return direction

    def RandomizePosition(self,random,direction=None,diskDist=None,diskRad=None):
        z = diskDist/I3Units.m
        
        r = sqrt(random.uniform(0,diskRad**2))
        theta = random.uniform(0,2*pi)
        
        x = r*cos(theta)
        y = r*sin(theta)

        p = dataclasses.I3Position(x*I3Units.m,y*I3Units.m,z*I3Units.m)
        p.rotate_y(direction.zenith)
        p.rotate_z(direction.azimuth)
        
        return p
