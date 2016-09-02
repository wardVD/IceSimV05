#!/usr/bin/python
from __future__ import print_function
from icecube.topsimulator import ParticleUnthinner
from icecube.dataclasses import I3Position, I3Direction, I3Particle
from icecube.icetray import I3Units, i3logging
from icecube.phys_services import I3GSLRandomService
from matplotlib import pyplot as plt, cm
from matplotlib.patches import Circle, Polygon
import numpy as np
import sys
import random

np.random.seed(1)
rng = I3GSLRandomService(1) 

# i3logging.set_level_for_unit('ParticleUnthinner', 'TRACE')
# i3logging.set_level_for_unit('ParticleUnthinner', 'DEBUG')

pu = ParticleUnthinner(800, 30, 16)

print('shape', pu.grid_shape, np.prod(pu.grid_shape))

def add_sta(xt, yt):
    pu.add_station(
        I3Position(xt - 5, yt - 5, 0),
        I3Position(xt + 5, yt + 5, 0),
        1.82,
        1.30)

for xt in xrange(-400, 1001, 200):
    add_sta(xt, 0)

for yt in xrange(-400, 1001, 200):
    add_sta(0, yt)

# check coordinate system transform and binning
xl = np.arange(-1000, 1001, 10)
yl = xl

for zenith in (0.0, 60.0 * I3Units.degree):
    part = I3Particle()
    part.pos = I3Position(0, 0, 0)
    part.dir = I3Direction(zenith, zenith)
    pu.set_primary(part)

    zl = np.empty(xl.shape + yl.shape + (2,))
    for i,x in enumerate(xl):
        for j,y in enumerate(yl):
            zl[i, j] = pu.to_index(I3Position(x, y, 0))

    fig, ax = plt.subplots(1, 2, figsize=(12, 5))
    plt.subplots_adjust(left=0.1, right=0.95, wspace=0.3)
    plt.figtext(0.5, 0.99, 
                (r'zenith = ${:.0f}^\circ$, '
                  'azimuth = ${:.0f}^\circ$')
                 .format(part.dir.zenith / I3Units.degree,
                         part.dir.azimuth / I3Units.degree),
                ha="center", va="top")

    for k in (0, 1):
        plt.sca(ax[k])
        plt.pcolormesh(xl, yl, zl[...,k].T,
                       cmap = cm.get_cmap("jet", pu.grid_shape[k]))
        plt.colorbar()
        plt.arrow(part.pos.x, part.pos.y,
                  -500 * part.dir.x, -500 * part.dir.y,
                  color="k")
        plt.clim(0, pu.grid_shape[k])
        plt.xlabel('$x$')
        plt.ylabel('$y$')


    fig, ax = plt.subplots(1, 2, figsize=(11, 5))
    plt.subplots_adjust(left=0.1, right=0.95, wspace=0.5)
    for k in (0, 1):
        plt.sca(ax[k])
        plt.xlim(-1200, 1200)
        plt.ylim(-1200, 1200)

    plt.sca(ax[0])
    x = []
    y = []
    for sta in pu.stations:
        for pos in sta.pos:
            x.append(pos.x)
            y.append(pos.y)
    plt.plot(x, y, "ow")

    plt.sca(ax[1])
    # draw tanks and sampling regions
    x = []
    y = []
    for sta in pu.stations:
        for pos in sta.pos:
            r, phi, z = pu.to_shower_front_cs(pos)
            x.append(r * np.cos(phi))
            y.append(r * np.sin(phi))
        r1, r2 = sta.ring_radius
        phi1, phi2 = sta.ring_phi
        xpol = []
        ypol = []
        ndeg = int(np.degrees(phi2 - phi1))
        for phi in np.linspace(phi1, phi2, ndeg):
            xpol.append(r1 * np.cos(phi))
            ypol.append(r1 * np.sin(phi))
        for phi in np.linspace(phi2, phi1, ndeg):
            xpol.append(r2 * np.cos(phi))
            ypol.append(r2 * np.sin(phi))
        ax[1].add_artist(Polygon(np.transpose((xpol, ypol)),
                                 closed=True,
                                 facecolor="r",
                                 edgecolor="None",
                                 alpha=0.5,
                                 zorder=2))
    plt.plot(x, y, "ow", zorder=3)

    # draw grid
    for i in xrange(pu.grid_shape[0]):
        r2 = pu.grid_segment(i, 0)[1]
        ax[1].add_patch(Circle((0, 0), r2,
                               edgecolor="b",
                               facecolor="None",
                               zorder=1))
    for j in xrange(pu.grid_shape[1]):
        p = pu.grid_segment(0, j)[2]
        r = pu.grid_rmax
        plt.plot([0, r * np.cos(p)], [0, r * np.sin(p)],
                 "-b", zorder=1)

    for i in xrange(pu.grid_shape[0]):
        for j in xrange(pu.grid_shape[1]):
            gnode = pu.grid_node(i, j)
            r1, r2, p1, p2 = pu.grid_segment(i, j) 
            sta = gnode.stations
            r = 0.5 * (r1 + r2)
            p = 0.5 * (p1 + p2)
            if sta:
                plt.text(r * np.cos(p), r * np.sin(p),
                         ",".join([str(k) for k in sta]),
                         ha="center", va="center")

plt.show()
