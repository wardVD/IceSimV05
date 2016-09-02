#!/usr/bin/env python
#
# pybinding load test
#

from I3Tray import *
from icecube import icetray, dataclasses, phys_services, sim_services, dataio, earthmodel_service, neutrino_generator

#
# for logging
#
icetray.I3Logger.global_logger = icetray.I3PrintfLogger()
icetray.set_log_level(icetray.I3LogLevel.LOG_WARN)
#icetray.set_log_level_for_unit("EarthModelService",icetray.I3LogLevel.LOG_TRACE)
#icetray.set_log_level_for_unit("I3NuG",icetray.I3LogLevel.LOG_TRACE)
icetray.set_log_level_for_unit("I3NuG",icetray.I3LogLevel.LOG_INFO)


# generate random service
random = phys_services.I3GSLRandomService(42)


# generate earthmodel service
earthmodel = ["PREM_mmc"]
materialmodel = ["Standard"]
earth = earthmodel_service.EarthModelService("EarthModelService","",
                              earthmodel, materialmodel, 
                              "IceSheet",
                              20.*I3Units.degree, 1948*I3Units.m)

steer = neutrino_generator.Steering(earth)

# generate interaction service
interaction = neutrino_generator.I3NuGInteractionInfo(random, steer, "csms")
interaction.initialize()

inttypes = ["CC_Nu_iso","NC_Nu_iso","CC_NuBar_iso","NC_NuBar_iso","GR_NuBar"]

elogmin = 6.0
elogmax = 7.5
nx = 50

elogl = []
ccnul = []
ccnubarl = []
ncnul = []
ncnubarl = []
grnubarl = []

import numpy as np
for e in np.linspace(elogmin, elogmax, nx) :
    ene = 10**e
    elogl.append(e)
    ccnul.append(interaction.get_xsec_cgs("CC_Nu_iso", ene)) 
    ccnubarl.append(interaction.get_xsec_cgs("CC_NuBar_iso", ene))
    ncnul.append(interaction.get_xsec_cgs("NC_Nu_iso", ene))
    ncnubarl.append(interaction.get_xsec_cgs("NC_NuBar_iso", ene))
    grnubarl.append(interaction.get_xsec_cgs("GR_NuBar", ene))

import pylab as P
import math as m

elog = np.array(elogl)
ccnu = np.array(ccnul)
ccnubar = np.array(ccnubarl)
ncnu = np.array(ncnul)
ncnubar = np.array(ncnubarl)
grnubar = np.array(grnubarl)

fig1 = P.figure()
ax1 = fig1.add_subplot(1,2,1)

ax1.plot(elog, ccnu, '-', color="red", label="CC_Nu_iso", linewidth=1.5)
ax1.plot(elog, ccnubar, '--', color="red", label="CC_NuBar_iso", linewidth=1.5)
ax1.plot(elog, ncnu, '-', color="green", label="NC_Nu_iso", linewidth=1.5)
ax1.plot(elog, ncnubar, '--', color="green", label="NC_NuBar_iso", linewidth=1.5)
ax1.plot(elog, grnubar, '--', color="blue", label="GR_NuBar", linewidth=1.5)
ax1.set_yscale('log')
ax1.set_xlabel('log10(neutrino energy)')
ax1.legend(loc="best")

ax2 = fig1.add_subplot(1,2,2)
ax2.plot(elog, ccnubar/ccnubar, '--', color="red", label="CC_NuBar_iso", linewidth=1.5)
ax2.plot(elog, ncnubar/ccnubar, '--', color="green", label="NC_NuBar_iso", linewidth=1.5)
ax2.plot(elog, grnubar/ccnubar, '--', color="blue", label="GR_NuBar", linewidth=1.5)
ax2.set_yscale('log')
ax2.set_xlabel('log10(neutrino energy)')
ax2.set_ylabel('ratio (vs ccnubar)')
ax2.legend(loc="best")

P.show()
