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


file1 = 'prem.pickle'

if(len(sys.argv) > 1 ):
   file1 = sys.argv[1]

f = open(file1,'r')
prem = pickle.load(f)
f.close()

prem_zen = prem['zenith']
prem_ene = prem['energy']
prem_w = prem['weight']
prem_coszen = prem['coszenith']
prem_enecos = prem['energycos']
prem_wcos = prem['weightcos']
prem_primary_w = prem['primary_w']
prem_inicenu_w = prem['inice_w']

print prem_primary_w
print prem_inicenu_w
print prem_inicenu_w / prem_primary_w

#exit(0)

# draw ratio plots

fig1 = P.figure()

fig1ax1 = fig1.add_subplot(2,1,1)
plt.pcolor(prem_zen, prem_ene, prem_w)
plt.colorbar()
fig1ax1.set_xlabel('zenith [deg]')
fig1ax1.set_ylabel('log10(Energy)')
fig1ax1.set_title(file1 + " ratio")

fig1ax3 = fig1.add_subplot(2,1,2)
plt.pcolor(prem_coszen, prem_enecos, prem_wcos)
plt.colorbar()
fig1ax3.set_xlabel('cos(zenith)')
fig1ax3.set_ylabel('log10(Energy)')
fig1ax3.set_title(file1 + " ratio")

#
# draw primary and inice counts
# in zenith-energy 
#

prem_prim_count = prem['primary']
prem_inice_count = prem['inice']

fig2 = P.figure()
fig2ax1 = fig2.add_subplot(2,1,1)

plt.pcolor(prem_zen, prem_ene, prem_prim_count)
plt.colorbar()
fig2ax1.set_xlabel('zenith [deg]')
fig2ax1.set_ylabel('log10(Energy)')
fig2ax1.set_title(file1 + " primary count")

fig2ax2 = fig2.add_subplot(2,1,2)
plt.pcolor(prem_zen, prem_ene, prem_inice_count)
plt.colorbar()
fig2ax2.set_xlabel('zenith [deg]')
fig2ax2.set_ylabel('log10(Energy)')
fig2ax2.set_title(file1 + " inice count")

#
# draw primary and inice counts
# in coszenith-energy 
#
prem_prim_count_cos = prem['primarycos']
prem_inice_count_cos = prem['inicecos']

fig3 = P.figure()
fig3ax1 = fig3.add_subplot(2,1,1)

plt.pcolor(prem_coszen, prem_enecos, prem_prim_count_cos)
plt.colorbar()
fig3ax1.set_xlabel('cos(zenith)')
fig3ax1.set_ylabel('log10(Energy)')
fig3ax1.set_title(file1 + " primary count_cos")

fig3ax2 = fig3.add_subplot(2,1,2)
plt.pcolor(prem_coszen, prem_enecos, prem_inice_count_cos)
plt.colorbar()
fig3ax2.set_xlabel('cos(zenith)')
fig3ax2.set_ylabel('log10(Energy)')
fig3ax2.set_title(file1 + " inice count_cos")

P.show()

