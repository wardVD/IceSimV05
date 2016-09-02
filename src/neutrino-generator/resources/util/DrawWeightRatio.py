#!/usr/bin/env python
#
# pybinding load test
#

import sys
if sys.version_info[0] >= 3:
    import pickle
else:
    import cPickle as pickle

import numpy as np
import matplotlib as mpl
import matplotlib.pyplot as plt
import pylab as P
import math as m
from matplotlib.colors import BoundaryNorm
from matplotlib.ticker import MaxNLocator

mpl.rc('figure.subplot', hspace=0.5, wspace=0.5)


file1 = 'prem.pickle'
file2 = 'flat.pickle'

colormin = 0.8
colormax = 1.2
#colormin = 0
#colormax = 2

if(len(sys.argv) > 1 ):
   file1 = sys.argv[1]

if(len(sys.argv) > 2 ):
   file2 = sys.argv[2]

f = open(file1,'r')
prem = pickle.load(f)
f.close()

prem_zen = prem['zenith']
prem_ene = prem['energy']
prem_w = prem['weight']
prem_coszen = prem['coszenith']
prem_enecos = prem['energycos']
prem_wcos = prem['weightcos']

f = open(file2,'r')
flat = pickle.load(f)
f.close()

flat_zen = flat['zenith']
flat_ene = flat['energy']
flat_w = flat['weight']
flat_coszen = flat['coszenith']
flat_enecos = flat['energycos']
flat_wcos = flat['weightcos']

# zero protection
ped = 1e-10*np.ones(prem_w.shape)
pedcos = 1e-10*np.ones(prem_wcos.shape)

weight_ratio = (flat_w / (prem_w + ped))
weight_ratio_cos = (flat_wcos / (prem_wcos + pedcos))

fig1 = P.figure()
ax1 = fig1.add_subplot(1,2,1)
plt.pcolor(prem_zen, prem_ene, weight_ratio, vmin=colormin, vmax=colormax)
plt.colorbar()

ax1.set_xlabel('zenith [deg]')
ax1.set_ylabel('log10(Energy)')
plt.title('Neutrino Flux Ratio')

ax2 = fig1.add_subplot(1,2,2)
plt.pcolor(prem_coszen, prem_enecos, weight_ratio_cos, vmin=colormin, vmax=colormax)
plt.colorbar()

ax2.set_xlabel('cos(zenith)')
ax2.set_ylabel('log10(Energy)')
plt.title('Neutrino Flux Ratio')

P.show()

