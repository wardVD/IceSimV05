#!/usr/bin/env python
#
# Draw InEarth propagation weight(.pickles)
#

import glob
import sys
if sys.version_info[0] >= 3:
    import pickle
else:
    import cPickle as pickle

import matplotlib.pyplot as plt
import pylab as P

infilebase = sys.argv[1]

f = open(infilebase+".pickles", 'r')
tables = pickle.load(f)
f.close()

zenith    = tables['zenith']
energy    = tables['energy']
weight    = tables['weight']
coszenith = tables['coszenith']
energycos = tables['energycos']
weightcos = tables['weightcos']

fig1 = P.figure()
ax1 = fig1.add_subplot(1,2,1)

plt.pcolor(zenith, energy, weight)
plt.colorbar()

ax1.set_xlabel('zenith [deg]')
ax1.set_ylabel('log10(Energy)')
plt.title('Event Ratio (NInIce/NPrimary)')

ax2 = fig1.add_subplot(1,2,2)
plt.pcolor(coszenith, energycos, weightcos)
plt.colorbar()

ax2.set_xlabel('Cos(zenith)')
ax2.set_ylabel('log10(Energy)')
plt.title('Event Ratio (NInIce/NPrimary)')

P.show()
