#!/usr/bin/env python

from icecube import icetray
from icecube import dataclasses
from icecube import dataio
from icecube import DOMLauncher


import pylab
import matplotlib.pyplot as plt
from math import log10
import numpy as np

from os.path import expandvars
PATH = expandvars("$I3_TESTDATA/sim")
GCDFN = PATH + "/GeoCalibDetectorStatus_IC86.55697_corrected_V2.i3.gz"

f = dataio.I3File(GCDFN)

fr = f.pop_frame()
while "I3Calibration" not in fr :
    fr = f.pop_frame()

domcal = fr["I3Calibration"].dom_cal


#Ploting residual of discriminator pulse template |interpolated-non_interpolated|
disc_pt = DOMLauncher.InterpolatedSPETemplate(domcal[icetray.OMKey(1,45)].discriminator_pulse_template(True))

times = np.arange(0,150000,0.1)

amps = np.array([disc_pt(t) for t in times])
st = 0
lin_chp = 80
quad_chp = 6400
disc_pt.tabulate(5000, 1000, st, lin_chp, quad_chp, 1e9)

amps_int = np.array([disc_pt(t) for t in times])

#pylab.axvspan(6400,1e9,facecolor='r', alpha=0.1)
print("Disc: Sum of absolute value of residuals: ",sum(abs(amps-amps_int)))

plt.loglog(times, abs(amps-amps_int))
plt.axvspan(0.1, lin_chp, facecolor='b', alpha=0.1)
plt.axvspan(lin_chp, quad_chp, facecolor='g', alpha=0.1)
plt.xlabel("time [ns]")
plt.ylabel("amplitude residual")
plt.figure()

#FADC plot
fadc_pt = DOMLauncher.InterpolatedSPETemplate(domcal[icetray.OMKey(1,45)].fadc_pulse_template(True))

amps = np.array([fadc_pt(t) for t in times])
st = -10
lin_chp = 400
quad_chp = 10000

fadc_pt.tabulate(3000, 2000, st, lin_chp, quad_chp, 1e9)

amps_int = np.array([fadc_pt(t) for t in times])

#pylab.axvspan(6400,1e9,facecolor='r', alpha=0.1)
print("FADC: Sum of absolute value of residuals: ",sum(abs(amps-amps_int)))

plt.loglog(times, abs(amps-amps_int))
plt.axvspan(0.1, lin_chp,facecolor='b', alpha=0.1)
plt.axvspan(lin_chp, quad_chp, facecolor='g', alpha=0.1)
plt.xlabel("time [ns]")
plt.ylabel("amplitude residual")
plt.figure()

#ATWD plot
shift = -25
atwd_pt = DOMLauncher.InterpolatedSPETemplate(domcal[icetray.OMKey(1,45)].atwd_pulse_template(1,True))
amps = np.array([atwd_pt(t + shift) for t in times])

st = -10
lin_chp = 80
quad_chp = 6400

atwd_pt.tabulate(5000, 1000, st, lin_chp, quad_chp, 1e9)

amps_int = np.array([atwd_pt(t+shift) for t in times])

#pylab.axvspan(6400,1e9,facecolor='r', alpha=0.1)

plt.loglog(times, abs(amps-amps_int))
plt.axvspan(0.1-shift+st, lin_chp,facecolor='b', alpha=0.1)
plt.axvspan(lin_chp, quad_chp, facecolor='g', alpha=0.1)
print("ATWD1: Sum of absolute value of residuals: ",sum(abs(amps-amps_int)))

atwd_pt = DOMLauncher.InterpolatedSPETemplate(domcal[icetray.OMKey(1,45)].atwd_pulse_template(0,True))
amps = np.array([atwd_pt(t+shift) for t in times])

atwd_pt.tabulate(5000, 1000, st, lin_chp, quad_chp, 1e9)

amps_int = np.array([atwd_pt(t+shift) for t in times])
plt.loglog(times, abs(amps-amps_int))
plt.xlabel("time - 25 [ns]")
plt.ylabel("amplitude residual")
print("ATWD0: Sum of absolute value of residuals: ",sum(abs(amps-amps_int)))

plt.figure()

for omkey, dc in domcal :

    spetemp = dc.discriminator_pulse_template(True)

    times = list(range(60))
    amps = [spetemp(t) for t in times]

#    t = len(amps) - 1
#    amp = amps[t]
#    while amp < 0 :
#        t += 1
#        amp = spetemp(t)
#    print "time at which template becomes positive = ",t
###
### becomes positive at around 7.8-8.0 ms
###

    if max(amps) > 0.07 and dc.toroid_type != dataclasses.I3DOMCalibration.OLD_TOROID:
        print(max(amps),"  ",dc.toroid_type, omkey)
    else:
        if max(amps) < 0.07 and dc.toroid_type != dataclasses.I3DOMCalibration.NEW_TOROID:
            print(max(amps),"  ",dc.toroid_type, omkey)

    pylab.plot(times,amps)

pylab.savefig("spe_templates.png")
pylab.show()

