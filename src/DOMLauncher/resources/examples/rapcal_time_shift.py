#!/usr/bin/env python
import sys
from os.path import expandvars
from I3Tray import *
from icecube import icetray, dataclasses, dataio, simclasses
from icecube import phys_services
from icecube import DOMLauncher
from icecube import WaveCalibrator
load('wavedeform')
import numpy as np
import scipy
from scipy import stats
from scipy.optimize import curve_fit
import matplotlib.pyplot as plt
import matplotlib


GCDFN = expandvars("$I3_TESTDATA/sim/GeoCalibDetectorStatus_IC86.55697_corrected_V2.i3.gz")


tray = I3Tray()

#tray.AddService("I3GSLRandomServiceFactory","Random",
                   #Seed = 1
#)

tray.AddModule("I3InfiniteSource","streams",
               Stream=icetray.I3Frame.DAQ,
               prefix=GCDFN ,
               )


tray.AddModule(lambda frame: frame.Put("I3EventHeader", dataclasses.I3EventHeader()), "eventheader-gen",
                Streams=[icetray.I3Frame.DAQ])


n_hits = 1000
N = 10
hit_times = []
def dom_simulation(frame, time_shift=10, nhits = n_hits):
	global hit_times
        random_service = phys_services.I3GSLRandomService(2)
        geo = frame["I3Geometry"].omgeo
        cal = frame["I3Calibration"].dom_cal
        stat = frame["I3DetectorStatus"].dom_status
        #DOMLauncher.I3DOM.merge_pulses = True
        #DOMLauncher.I3DOM.use_tabulation(True)
        omKey1 = icetray.OMKey(47,2)
        dom1 = DOMLauncher.I3InIceDOM(random_service,omKey1)
        dom1.configure(cal[omKey1],stat[omKey1])

        omKey2 = icetray.OMKey(47,3)
        dom2 = DOMLauncher.I3InIceDOM(random_service,omKey2)
        dom2.configure(cal[omKey2],stat[omKey2])
        dom1.rapcal_time_shift = 0
        dom2.rapcal_time_shift = time_shift
        #making the doms neigbors
        dom2.get_neighbors().append(dom1)
        dom1.get_neighbors().append(dom2)

        pulses = simclasses.I3MCPulseSeries()
        pulse = simclasses.I3MCPulse()
        pulse.charge = 1
        pulse.time = 0
        for i in xrange(nhits):
            pulses.append(pulse)
            hit_times +=[pulse.time]
            pulse.time += 29e3


        triggers = DOMLauncher.DCStream()
        print("simulating discriminator")
        dom1.discriminator(pulses,triggers)
        dom2.discriminator(pulses,triggers)

        triggers = sorted(triggers, key = lambda t: t.time)
        i = 0
        for trigg in triggers:
            if(i%100==0):
                print(trigg.DOM, trigg.time)
	    i +=1
            if(trigg.DOM == omKey1):
                dom1.add_trigger(trigg)
            elif(trigg.DOM == omKey2):
                dom2.add_trigger(trigg)
        print("ending simulation")
        dom1.trigger_launch(False)
        dom2.trigger_launch(False)
        launch_map = dataclasses.I3DOMLaunchSeriesMap()
        launch_map[dom1.get_omkey()] = dom1.get_domlaunches()
        launch_map[dom2.get_omkey()] = dom2.get_domlaunches()
        frame["InIceRawData"] = launch_map

tray.AddModule(dom_simulation,"simulation",Streams=[icetray.I3Frame.DAQ])
tray.AddModule('I3WaveCalibrator', 'domcal')
tray.AddModule('I3Wavedeform', 'deform')

hits1 = []
hits2 = []
def summarize(frame):
    global hits1, hits2
    pulses = frame['WavedeformPulses']
    for hit in pulses[icetray.OMKey(47,2)]:
        hits1 +=[hit.time]
    for hit in pulses[icetray.OMKey(47,3)]:
        hits2 +=[hit.time]


tray.AddModule(summarize,'sum',Streams=[icetray.I3Frame.DAQ])
tray.AddModule('TrashCan','~')
tray.Execute(N+3)
tray.Finish()


hit_times = np.array(hit_times)

non_shifted, binEdges = np.histogram(np.array(hits1)-hit_times, bins=400,range=(-5,15))
#print max(np.array(hits1)-hit_times)
print hit_times
print np.array(hits1)
#print min(np.array(hits1)-hit_times)
bincenters = 0.5*(binEdges[1:]+binEdges[:-1])
plt.plot(bincenters, non_shifted, 'bo',label="Non-shifted launch time")
shifted, binEdges = np.histogram(np.array(hits2)-hit_times, bins=400,range=(-5,15))
plt.plot(bincenters, shifted, 'ro',label="Shifted launch time")
plt.xlabel("$t_{pulse}-t_{reco}$")
plt.ylabel("Number of entries")
plt.legend()
plt.show()
