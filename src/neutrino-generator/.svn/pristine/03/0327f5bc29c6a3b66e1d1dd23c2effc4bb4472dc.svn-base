#!/usr/bin/env python
#
# pybinding load test
#

from I3Tray import *
from icecube import icetray, dataclasses, phys_services, sim_services, dataio, earthmodel_service, neutrino_generator

import numpy as np
import matplotlib as mpl
import matplotlib.pyplot as plt
import pylab as P
import os
import sys
from os.path import expandvars

CS_PATH = None
if os.environ['I3_DATA'] and os.path.exists(expandvars("$I3_PATH/neutrino-generator/cross_section_data")) :
    CS_PATH = os.path.exists(expandvars("$I3_PATH/neutrino-generator/cross_section_data"))
elif os.path.exists(expandvars("$I3_BUILD/neutrino-generator/resources/cross_section_data")) :
    CS_PATH = os.path.exists(expandvars("$I3_BUILD/neutrino-generator/resources/cross_section_data"))
else :
    print "ERROR : Can't find cross section data. "
    sys.exit()


# prepare data for line plotting
#
def format (yval, binedges ) :
    smallnum = 0.5
    x = []
    y = []
    for index, value in enumerate(yval) :
        print "y = %f, binedge_low = %f, binedge_hi = %f" % (value, binedges[index],binedges[index+1])
        x.append(binedges[index])
        x.append(binedges[index+1])
        if (value == 0) :
            value = smallnum
        y.append(value)
        y.append(value)

    # append zeros to close histos
    x.insert(0, x[0])
    y.insert(0, smallnum)
    x.append(x[-1])
    y.append(smallnum)

    return np.array(x), np.array(y)


#
# for logging
#
icetray.I3Logger.global_logger = icetray.I3PrintfLogger()
icetray.set_log_level(icetray.I3LogLevel.LOG_WARN)
#icetray.set_log_level_for_unit("EarthModelService",icetray.I3LogLevel.LOG_TRACE)
#icetray.set_log_level_for_unit("I3NuG",icetray.I3LogLevel.LOG_TRACE)
#icetray.set_log_level_for_unit("I3NuG",icetray.I3LogLevel.LOG_INFO)
icetray.set_log_level_for_unit("I3NuG",icetray.I3LogLevel.LOG_ERROR)


# generate random service
random = phys_services.I3SPRNGRandomService(1, 10000, 1)


# generate earthmodel service
earthmodel = ["PREM_mmc"]
materialmodel = ["Standard"]
earth = earthmodel_service.EarthModelService("EarthModelService","",
                              earthmodel, materialmodel, 
                              "IceSheet",
                              30.*I3Units.degree, 1948*I3Units.m)

steer = neutrino_generator.Steering(earth)

icetray.logging.log_info("default steer injection mode is %s" % (steer.injection_mode), "steering")
icetray.logging.log_info("default steer vtc genmode is %s" % (steer.vtx_gen_mode), "steering")

# generate interaction CC
nuCC = neutrino_generator.InteractionCC(random, steer)
nuCC.initialize_crossection_table(CS_PATH + "/csms/total_nu_CC_iso_NLO_HERAPDF1.5NLO_EIG.dat")
nuCC.initialize_finalstate_table(CS_PATH + "/csms/xy_nu_CC_iso_NLO_HERAPDF1.5NLO_EIG_xy.dat")

nbins = 20
xmin = -1
xmax = 1

nevts = 1000000
elogmin = 1
elogmax = 8

# muon mass in GeV
M_mu = 0.105658389 

import math
ne0 = 0
ne1 = 0
def get_costheta_simple (interaction, elog, ethres):
    xy = interaction.select_xy(elog)
    out = (1.0 - xy[1]) * (10.0**elog)
    costheta = interaction.calc_outgoing_costheta_simple(10.0**elog, xy[0], xy[1])
    if (math.fabs(costheta) > 1.0) :
        global ne0
        ne0 += 1
        if (out < ethres) :
            costheta = random.uniform(-1.0, 1.0)
        else :
            global ne1
            ne1 += 1
            icetray.logging.log_notice("elog = %e, out = %f, x = %e, y = %e, costheta = %e" % 
                (elog, out,  xy[0], xy[1], costheta))
            costheta = get_costheta_simple(interaction, elog, ethres)
        icetray.logging.log_notice("select costheta = %f" % (costheta))
    return costheta

ne2 = 0
def get_costheta (interaction, elog, ethres, lepton_mass):
    xy = interaction.select_xy(elog)
    out = (1.0 - xy[1]) * (10.0**elog)
    costheta = interaction.calc_outgoing_costheta(10.0**elog, lepton_mass, xy[0], xy[1])
    if (math.fabs(costheta) > 1.0) :
        if (out < ethres) :
            costheta = random.uniform(-1.0, 1.0)
        else :
            global ne2
            ne2 += 1
            icetray.logging.log_notice("elog = %e, out = %f, x = %e, y = %e, costheta = %e" % 
                (elog, out,  xy[0], xy[1], costheta))
            costheta = get_costheta(interaction, elog, ethres, lepton_mass)
        icetray.logging.log_notice("select costheta = %f" % (costheta))
    return costheta

data = np.random.uniform(elogmin, elogmax, nevts)
costheta_simple = [get_costheta_simple(nuCC, i, -1) for i in data]
costheta = [get_costheta(nuCC, i, -1, M_mu) for i in data]

print ("Number of events (fabs(costheta)) > 1.0) = %d" % (ne0))
print ("Number of events (resampled) for simple version = %d" % (ne1))
print ("Number of events (resampled) for new version = %d" % (ne2))

ne0 = 0;
ne1 = 0;
ne2 = 0;
costheta_simple_random = [get_costheta_simple(nuCC, i, 0.2) for i in data]
costheta_random = [get_costheta(nuCC, i, 0.2, M_mu) for i in data]

print ("Number of events (fabs(costheta)) > 1.0) = %d" % (ne0))
print ("Number of events (resampled) for simple version = %d" % (ne1))
print ("Number of events (resampled) for new version = %d" % (ne2))

# prepare figure
fig1 = P.figure()

ax1 = fig1.add_subplot(1,2,1)

yval, binEdges = np.histogram(costheta_simple, nbins, range=[xmin, xmax])
x1 = 0.5*(binEdges[1:]+binEdges[:-1])
y1 = np.array(1.0*yval)
y1err = np.sqrt(y1)
ax1.errorbar(x1, y1, yerr=y1err, color="red", label="costheta simple",linewidth=1.5)

yval, binEdges = np.histogram(costheta_simple_random, nbins, range=[xmin, xmax])
x2 = 0.5*(binEdges[1:]+binEdges[:-1])
y2 = np.array(1.0*yval)
y2err = np.sqrt(y2)
ax1.errorbar(x2, y2, yerr=y2err, color="magenta", label="costheta simple + random",linewidth=1.5)

yval, binEdges = np.histogram(costheta, nbins, range=[xmin, xmax])
x3 = 0.5*(binEdges[1:]+binEdges[:-1])
y3 = np.array(1.0*yval)
y3err = np.sqrt(y3)
ax1.errorbar(x3, y3, yerr=y3err, color="blue", label="costheta new",linewidth=1.5)

yval, binEdges = np.histogram(costheta, nbins, range=[xmin, xmax])
x4 = 0.5*(binEdges[1:]+binEdges[:-1])
y4 = np.array(1.0*yval)
y4err = np.sqrt(y4)
ax1.errorbar(x4, y4, yerr=y4err, color="cyan", label="costheta new + random",linewidth=1.5)

ax1.set_yscale('log')
ax1.legend()

ax2 = fig1.add_subplot(1,2,2)

ax2.plot(x1, y1/y1, '-', color="red", label="costheta simple",linewidth=1.5)
ax2.plot(x1, y2/y1, '-', color="magenta", label="costheta simple + random",linewidth=1.5)
ax2.plot(x1, y3/y1, '-', color="blue", label="costheta new",linewidth=1.5)
ax2.plot(x1, y4/y1, '-', color="cyan", label="costheta new + random",linewidth=1.5)
#ax2.set_yscale('log')
ax2.legend()

P.show()





