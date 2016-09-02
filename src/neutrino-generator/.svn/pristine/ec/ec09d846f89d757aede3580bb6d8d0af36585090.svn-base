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
import math as m
from matplotlib.colors import BoundaryNorm
from matplotlib.ticker import MaxNLocator

mpl.rc('figure.subplot', hspace=0.5, wspace=0.5, bottom=0.2, top=0.8)
mpl.rcParams['figure.figsize'] = "11,5"
mpl.rcParams['font.size'] = 18
mpl.rcParams['font.family'] = 'fantasy'
mpl.rcParams['lines.linewidth'] = 1.5

file1 = 'prem.pickle'
file2 = ''
nrows = 1 
ncols = 2
ndiv = nrows * ncols

colormin = 0.
colormax = 1.

if(len(sys.argv) > 1 ):
    file1 = sys.argv[1]

if(len(sys.argv) > 2 ):
    file2 = sys.argv[2]
    nrows = 2
    ncols = 2
    ndiv = nrows * ncols

f = open(file1,'r')
prem = pickle.load(f)
f.close()

prem_zen = prem['zenith']
prem_ene = prem['energy']
prem_w = prem['weight']
prem_coszen = prem['coszenith']
prem_enecos = prem['energycos']
prem_wcos = prem['weightcos']

# draw plots

from matplotlib.ticker import *

def draw_canvas(fig, canvasid, x, y, w, xlabel, ylabel, title, colormin=-1, colormax=-1) :
    ax1 = fig.add_subplot(nrows,ncols,canvasid)
    if colormin >-1 :
        c1  = ax1.pcolormesh(x, y, w, vmin=colormin, vmax=colormax )
    else :
        c1  = ax1.pcolormesh(x, y, w)
    plt.colorbar(c1)
    plt.locator_params(axis='x',tight=True, nbins=5)
    
    ax1.set_xlabel(xlabel)
    ax1.set_ylabel(ylabel)
    ax1.set_title(title)


fig1 = plt.figure()
draw_canvas(fig1, 1, prem_zen, prem_ene, prem_w, 'zenith [deg]', 'log10(Energy)', "file1 inice_w/prim_w", colormin, colormax)
draw_canvas(fig1, 2, prem_coszen, prem_enecos, prem_wcos, 'cos(zenith)', 'log10(Energy)', "file1 inice_w/prim_w", colormin, colormax)

#
# draw primary and inice counts
# in zenith-energy 
#

prem_prim_count = prem['primary']
prem_inice_count = prem['inice']

fig2 = plt.figure()
draw_canvas(fig2, 1, prem_zen, prem_ene, prem_prim_count, 'zenith [deg]', 'log10(Energy)', "file1 primary count")
draw_canvas(fig2, 2, prem_zen, prem_ene, prem_inice_count, 'zenith [deg]', 'log10(Energy)', "file1 inice count")

#
# draw primary and inice counts
# in coszenith-energy 
#

prem_prim_count_cos = prem['primarycos']
prem_inice_count_cos = prem['inicecos']

fig3 = plt.figure()
draw_canvas(fig3, 1, prem_coszen, prem_enecos, prem_prim_count_cos, 'cos(zenith)', 'log10(Energy)', "file1 primary count")
draw_canvas(fig3, 2, prem_coszen, prem_enecos, prem_inice_count_cos, 'cos(zenith)', 'log10(Energy)', "file1 inice count")

#
# draw primary and inice weights
# in zenith-energy 
#

prem_prim_w = prem['primary_w']
prem_inice_w = prem['inice_w']

fig4 = plt.figure()
draw_canvas(fig4, 1, prem_zen, prem_ene, prem_prim_w, 'zenith [deg]', 'log10(Energy)', "file1 primary weight")
draw_canvas(fig4, 2, prem_zen, prem_ene, prem_inice_w, 'zenith [deg]', 'log10(Energy)', "file1 inice weight")

#
# draw primary and inice weights
# in coszenith-energy 
#

prem_prim_w_cos = prem['primarycos_w']
prem_inice_w_cos = prem['inicecos_w']

fig5 = plt.figure()
draw_canvas(fig5, 1, prem_coszen, prem_enecos, prem_prim_w_cos, 'cos(zenith)', 'log10(Energy)', "file1 primary weight")
draw_canvas(fig5, 2, prem_coszen, prem_enecos, prem_inice_w_cos, 'cos(zenith)', 'log10(Energy)', "file1 inice weight")

#--------------------------
# draw second file if exist
#--------------------------

if file2 != '' :

    f = open(file2,'r')
    flat = pickle.load(f)
    f.close()

    flat_zen = flat['zenith']
    flat_ene = flat['energy']
    flat_w = flat['weight']
    flat_coszen = flat['coszenith']
    flat_enecos = flat['energycos']
    flat_wcos = flat['weightcos']

    fig1 = plt.figure(1)
    draw_canvas(fig1, 3, flat_zen, flat_ene, flat_w, 'zenith [deg]', 'log10(Energy)', "file2 inice_w/prim_w")
    draw_canvas(fig1, 4, flat_coszen, flat_enecos, flat_wcos, 'cos(zenith)', 'log10(Energy)', "file2 inice_w/prim_w")

    flat_prim_count = flat['primary']
    flat_inice_count = flat['inice']

    fig2 = plt.figure(2)
    draw_canvas(fig2, 3, flat_zen, flat_ene, flat_prim_count, 'zenith [deg]', 'log10(Energy)', "file2 primary count")
    draw_canvas(fig2, 4, flat_zen, flat_ene, flat_inice_count, 'zenith [deg]', 'log10(Energy)', "file2 inice count")

    flat_prim_count_cos = flat['primarycos']
    flat_inice_count_cos = flat['inicecos']

    fig3 = plt.figure(3)
    draw_canvas(fig3, 3, flat_coszen, flat_enecos, flat_prim_count_cos, 'cos(zenith)', 'log10(Energy)', "file2 primary count")
    draw_canvas(fig3, 4, flat_coszen, flat_enecos, flat_inice_count_cos, 'cos(zenith)', 'log10(Energy)', "file2 inice count")

    flat_prim_w = flat['primary_w']
    flat_inice_w = flat['inice_w']

    fig4 = plt.figure(4)
    draw_canvas(fig4, 3, flat_zen, flat_ene, flat_prim_w, 'zenith [deg]', 'log10(Energy)', "file2 primary weight")
    draw_canvas(fig4, 4, flat_zen, flat_ene, flat_inice_w, 'zenith [deg]', 'log10(Energy)', "file2 inice weight")

    flat_prim_w_cos = flat['primarycos_w']
    flat_inice_w_cos = flat['inicecos_w']

    fig5 = plt.figure(5)
    draw_canvas(fig5, 3, flat_coszen, flat_enecos, flat_prim_w_cos, 'cos(zenith)', 'log10(Energy)', "file2 primary weight")
    draw_canvas(fig5, 4, flat_coszen, flat_enecos, flat_inice_w_cos, 'cos(zenith)', 'log10(Energy)', "file2 inice weight")

plt.figure(1)
plt.savefig("fig1.png")
plt.figure(2)
plt.savefig("fig2.png")
plt.figure(3)
plt.savefig("fig3.png")
plt.figure(4)
plt.savefig("fig4.png")
plt.figure(5)
plt.savefig("fig5.png")

plt.show()

