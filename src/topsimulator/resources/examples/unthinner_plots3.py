#!/usr/bin/python
from icecube.topsimulator import ParticleUnthinner, ExtendedI3Particle, intersect_cylinder
from icecube.dataclasses import I3Position, I3Direction, I3Particle
from icecube.icetray import I3Units, i3logging
from icecube.phys_services import I3GSLRandomService
from matplotlib import pyplot as plt, cm
from matplotlib.patches import Circle, Polygon
import numpy as np
import sys
import random
from scipy.stats import poisson

np.random.seed(1)
rng = I3GSLRandomService(1)

pu = ParticleUnthinner(20, 10, 16)

pu.add_station(
    I3Position(100, -1.5, -2.5),
    I3Position(100,  1.5,  2.5),
    1, 1)

part = I3Particle()
part.pos = I3Position(0, 0, 0)
part.dir = I3Direction(0, 0)
part.time = 10 * I3Units.ns
pu.set_primary(part)

show_scenario = False
if show_scenario:
    plt.figure()
    # draw tanks and sampling regions
    x = []
    y = []
    for sta in pu.stations:
        for pos in sta.pos:
            c = Circle((pos.x, pos.y), sta.radius)
            plt.gca().add_artist(c)
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
        plt.gca().add_artist(Polygon(np.transpose((xpol, ypol)),
                                 closed=True,
                                 facecolor="r",
                                 edgecolor="None",
                                 alpha=0.5,
                                 zorder=2))
    plt.xlim(90, 110)
    plt.ylim(-20, 20)

count_0 = np.zeros((2, 2), dtype=int)
count_1 = np.zeros((2, 2), dtype=int)

ring_r1, ring_r2 = pu.stations[0].ring_radius
ring_p1, ring_p2 = pu.stations[0].ring_phi

xp_all = []
yp_all = []
nevent = 10000
for ievent in xrange(nevent):
    navg = 20
    weight = 10
    npart = poisson(navg).rvs(1)
    ext = pu.grid_rmax
    z1 = np.random.rand(npart)
    z2 = np.random.rand(npart)
    rp = ring_r1 * (1.0 - z1) + ring_r2 * z1
    pp = ring_p1 * (1.0 - z2) + ring_p2 * z2
    xp = rp * np.cos(pp)
    yp = rp * np.sin(pp)
    xp_all.extend(xp)
    yp_all.extend(yp)
    particles = []
    stations = pu.stations
    dummy = I3Particle()
    hit_count = np.zeros((len(stations), 2), dtype=int)
    for x, y in zip(xp, yp):
        p = ExtendedI3Particle()
        p.type = I3Particle.MuMinus
        p.pos = I3Position(x, y, 0)
        p.dir = I3Direction(0, 0)
        p.energy = 10 * I3Units.GeV
        p.time = 10 * I3Units.ns
        particles.append(p)
        pu.add_particle(p, 1)
        for ista, sta in enumerate(stations):
            for itank, pos in enumerate(sta.pos):
                hit_count[ista][itank] += \
    intersect_cylinder(pos, sta.height, sta.radius, p, dummy)

    for ista in xrange(hit_count.shape[0]):
        count_0[int(hit_count[ista][0] > 0), 
                int(hit_count[ista][1] > 0)] += 1

    nweighted = poisson(navg / weight).rvs(1)
    hit_count = np.zeros((len(stations), 2), dtype=int)
    nclones = 0
    for p in particles[:nweighted]:
        clones = pu.sample(p, weight, rng)
        nclones += len(clones)
        for c in clones:
            for ista, sta in enumerate(stations):
                for itank, pos in enumerate(sta.pos):
                    hit_count[ista][itank] += \
        intersect_cylinder(pos, sta.height, sta.radius, c, dummy)

    for ista in xrange(hit_count.shape[0]):
        count_1[int(hit_count[ista][0] > 0), 
                int(hit_count[ista][1] > 0)] += 1

if show_scenario:
    plt.plot(xp_all, yp_all, "k.", mec="None", alpha=0.01)

plt.figure()
for i, count in enumerate((count_0, count_1)):
    label = ["no thin", " unthin"][i]
    x = np.arange(4, dtype=float)
    x += 0.1 * (2 * i - 1)
    y = count[0, 0], count[1, 0], count[0, 1], count[1, 1]
    ye = [_ ** 0.5 for _ in y]
    plt.errorbar(x, y, ye,
                 fmt=("ob", "sr")[i],
                 ms=10, mec="None", label=label)
    one_tank_hit = count[0, 1] + count[1, 0]
    print label + ": N[both hit]/N[one hit]", \
        float(count[1,1]) / (one_tank_hit + 1e-300)

plt.semilogy()
plt.legend()
plt.xticks(np.arange(4), ("no hit", "A hit", "B hit", "A \& B hit"))
plt.show()
