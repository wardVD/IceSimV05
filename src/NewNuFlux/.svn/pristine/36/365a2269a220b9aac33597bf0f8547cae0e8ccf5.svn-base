#!/usr/bin/env python

import math, numpy
from icecube import dataclasses,NewNuFlux

#print out all of the available models
NewNuFlux.printModels()

#get the Honda 2006 model
flux=NewNuFlux.makeFlux("honda2006")

#add the effects of the CR spectrum
flux.knee_reweighting_model="gaisserH3a_elbert"

#increase the contribution from Kaons by 10%
flux.relative_kaon_contribution=1.1

#print out a simple table of flux values for different energies at a fixed zenith angle
#Results are similar to the first column of Table XXII in arXiv:astro-ph/0611418
cz=1.0 #cosine of zenith angle
print ""
for le in numpy.arange(1,4.1,.1):
	en=math.pow(10,le)
	f=flux.getFlux(dataclasses.I3Particle.NuMu,en,cz)
	print en, f