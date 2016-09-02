#!/usr/bin/python
from icecube.topsimulator import ParticleUnthinner, ExtendedI3Particle, intersect_cylinder
from icecube.dataclasses import I3Position, I3Direction, I3Particle
from icecube.icetray import I3Units, i3logging
from icecube.phys_services import I3GSLRandomService
from matplotlib import pyplot as plt, cm
from matplotlib.patches import Circle
import numpy as np
import sys
import random
from scipy.stats import chi2

seed = int(sys.argv[1])
np.random.seed(seed)
rng = I3GSLRandomService(seed) 

# i3logging.set_level_for_unit('ParticleUnthinner', 'TRACE')
# i3logging.set_level_for_unit('ParticleUnthinner', 'DEBUG')

pu = ParticleUnthinner(30, 10, 8)

def add_sta(xt, yt):
    pu.add_station(
        I3Position(xt, yt - 10, -2.5),
        I3Position(xt, yt + 10, -2.5),
        5.0, 5.0)

add_sta(-10, 0)
add_sta(+10, 0)

for zenith in (0.0, 30.0 * I3Units.degree):
    part = I3Particle()
    part.pos = I3Position(0, 0, 0)
    part.dir = I3Direction(zenith, zenith)
    part.time = 10 * I3Units.ns
    pu.set_primary(part)

    npart = 100000
    ext = pu.grid_rmax / np.cos(zenith)
    xp = ext * (2 * np.random.rand(npart) - 1)
    yp = ext * (2 * np.random.rand(npart) - 1)
    particles = []
    hitmask = []
    stations = pu.stations
    dummy = I3Particle()
    weight = 1.0
    for x, y in zip(xp, yp):
        p = ExtendedI3Particle()
        p.type = I3Particle.MuMinus
        p.pos = I3Position(x, y, 0)
        p.dir = I3Direction(zenith, zenith)
        p.energy = 10 * I3Units.GeV
        p.time = 10 * I3Units.ns
        particles.append(p)
        pu.add_particle(p, weight)
        tank_hit = False
        for sta in stations:
            for pos in sta.pos:
                tank_hit |= intersect_cylinder(pos, sta.height, sta.radius, p, dummy)
        hitmask.append(tank_hit)
    hitmask = np.array(hitmask)

    shape = pu.grid_shape
    for i in xrange(shape[0]):
        for j in xrange(shape[1]):
            gnode = pu.grid_node(i, j)
            print i, j, gnode.statistics["muon"]

    xclones = []
    yclones = []
    for p in particles:
        clones = pu.sample(p, weight, rng)
        for c in clones:
            xclones.append(c.pos.x)
            yclones.append(c.pos.y)

    fig, ax = plt.subplots(1, 2, figsize=(11, 5.5))
    plt.subplots_adjust(left=0.1, right=0.95, wspace=0.3, top=0.85)
    plt.figtext(0.5, 0.99,
        "zenith, azimuth = %.0f deg" % (zenith / I3Units.degree),
        ha="center", va="top")
    for sta in pu.stations:
        for pos in sta.pos:
            ax[0].add_artist(Circle((pos.x, pos.y), sta.radius, facecolor="None", zorder=10))
    plt.sca(ax[0])
    plt.plot(xp, yp, ".k", alpha=0.01)
    plt.plot(xp[hitmask], yp[hitmask], '.r', alpha=0.1)
    plt.plot(xclones, yclones, '.b', alpha=0.1)
    plt.xlabel("x")
    plt.ylabel("y")

    w1, xe = np.histogram(xp[hitmask], bins=20)
    w2 = np.histogram(xclones, bins=len(w1), range=(xe[0], xe[-1]))[0]
    dx = xe[1:] - xe[:-1]
    cx = 0.5 * (xe[1:] + xe[:-1])
    plt.sca(ax[1])
    m = w1 > 0
    plt.errorbar(cx[m], w1[m], w1[m] ** 0.5, fmt="or", label="original")
    m = w2 > 0
    plt.errorbar(cx[m], w2[m], w2[m] ** 0.5, fmt="Db", label="unthinned")
    m = (w1 > 0) & (w2 > 0)
    wchi2 = np.sum((w1[m] - w2[m]) ** 2 / (w1[m] + w2[m]))
    plt.title("chance prob. = %0.2f" % (1 - chi2(np.sum(m)).cdf(wchi2)))
    plt.xlabel("x")

plt.show()
