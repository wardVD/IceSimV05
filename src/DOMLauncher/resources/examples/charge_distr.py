#!/usr/bin/env python

from optparse import OptionParser
from os.path import expandvars
import sys
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

usage = "usage: %prog [options] outputfile"
parser = OptionParser(usage)
parser.add_option("-s", "--seed",type="int",default=12345,
                  dest="SEED", help="Initial seed for the random number generator")
parser.add_option("-n", "--numevents", type="int", default=10,
                  dest="NUMEVENTS", help="The number of events per run")
parser.add_option("--comparePulses", dest="comparePulses", default=False, action="store_true")
parser.add_option("-g","--gcd", dest="gcd", type="string",default=GCDFN)

# parse cmd line args, bail out if anything is not understood
(options, args) = parser.parse_args()



tray = I3Tray()

tray.AddService("I3GSLRandomServiceFactory","Random",
		   Seed = options.SEED,
		   InstallServiceAs = "Random"
)

tray.AddModule("I3InfiniteSource","streams",
               Stream=icetray.I3Frame.DAQ,
               prefix=options.gcd,
               )


tray.AddModule(lambda frame: frame.Put("I3EventHeader", dataclasses.I3EventHeader()), "eventheader-gen",
                Streams=[icetray.I3Frame.DAQ])

def makeHits(frame):
        hits = simclasses.I3MCPESeriesMap()
        geo = frame["I3Geometry"]
        doms = list(geo.omgeo.keys())
        for dom in doms:
            hits[dom] = simclasses.I3MCPESeries()
            for i in range(0,10):
                    hit = simclasses.I3MCPE()
                    hit.npe=1
                    hit.time=29000*i
                    hits[dom].append(hit)
        
            #dom = list(geo.omgeo.keys())[3]
            #hits[dom] = simclasses.I3MCPESeries()
            #for i in range(0,10):
                    #hit = simclasses.I3MCPE()
                    #hit.npe=1
                    #hit.time=29000*i
                    #hits[dom].append(hit)
            #dom = list(geo.omgeo.keys())[4]
            #hits[dom] = simclasses.I3MCPESeries()
            #for i in range(0,10):
                    #hit = simclasses.I3MCPE()
                    #hit.npe=1
                    #hit.time=29000*i
                    #hits[dom].append(hit)
            #dom = list(geo.omgeo.keys())[5]
            #hits[dom] = simclasses.I3MCPESeries()
            #for i in range(0,10):
                    #hit = simclasses.I3MCPE()
                    #hit.npe=1
                    #hit.time=29000*i
                    #hits[dom].append(hit)
        frame["MCPESeriesMap"] = hits

tray.AddModule(makeHits,"hitterer",Streams=[icetray.I3Frame.DAQ])

tray.AddModule("PMTResponseSimulator", "Rosencrantz",
        Input = "MCPESeriesMap",
        Output = "ProcessedMCPulses",
        #UseSPEDistribution = False
        PrePulseProbability = 0,
        LatePulseProbability = 0,
        AfterPulseProbability = 0,
        RandomServiceName = "Random"
        )

tray.AddModule("DOMLauncher", "Guildenstern",
        Input = "ProcessedMCPulses",
        Output = "InIceRawData",
        RandomServiceName = "Random"
        )

tray.AddModule('I3WaveCalibrator', 'domcal')
tray.AddModule('I3Wavedeform', 'deform')

mc_charge = []
rec_charge = []
launches = []
def summarize(frame):

        hits = frame['ProcessedMCPulses']

        pulses = frame['WavedeformPulses']
        launches = frame['InIceRawData']

        global mc_charge
        for om in hits.keys():
                for hit in hits[om]:
				if(hit.charge>0.8):
				    if(om not in launches):
					print("Charge",hit.charge," cause no launch")
				    elif(om not in pulses):
					print("Charge",hit.charge," was not extracted")
                                mc_charge += [hit.charge]

        global rec_charge
        for om in pulses.keys():
                for pulse in pulses[om]:
                        rec_charge += [pulse.charge]



tray.AddModule(summarize,'sum',Streams=[icetray.I3Frame.DAQ])

tray.AddModule('I3Writer','writer',Filename="roundtrip.i3",Streams=[icetray.I3Frame.DAQ])

tray.AddModule('TrashCan','~')
tray.Execute(options.NUMEVENTS+3)
tray.Finish()
print("Number of simulated hits: ",len(mc_charge))
print("Number of extracted hits: ",len(rec_charge))
# Charge distribution function taken from PMTResponseSimulator.
# For stability reasons sigma is defined as 1/std^2
def chargDistr(x, mean, sigma, norm):
    e = (x - mean)#/(sigma)
    #norm = 0.58713646069439362
    amp = 1.6
    return norm*(np.exp(-x/0.5057) + amp*np.exp(-0.5*e*e*sigma))

gaussian = stats.norm

plt.figure()

# window for the fit
x_min = 0.5
x_max = 1.5
x = np.arange(x_min,x_max,0.01)

mc_charge_hist, binEdges = np.histogram(mc_charge, bins=150,range=(0,3))
bincenters = 0.5*(binEdges[1:]+binEdges[:-1])
mc_charge_Std = np.sqrt(mc_charge_hist)

mc_charge = np.array(mc_charge)
mask = (mc_charge > x_min)*(mc_charge < x_max)
#mean, std = gaussian.fit(mc_charge[mask])
bin_mask = (bincenters>x_min)*(bincenters<x_max)

popt, pcov = curve_fit(chargDistr,
                       bincenters[bin_mask],
                       mc_charge_hist[bin_mask],
                       p0=(1,1.82,100),
                       sigma = mc_charge_Std[bin_mask]
                       )
print(popt)
plt.plot(x,chargDistr(x,popt[0],popt[1],popt[2]),'b-')
plt.plot(bincenters,mc_charge_hist,'bo',label="MCPulse charge: \n mean: %.4f"%(popt[0])+"\n sigma: %.4f"%(1/np.sqrt(popt[1])))
plt.errorbar(bincenters,mc_charge_hist, yerr= mc_charge_Std,fmt='bo')



rec_charge = np.array(rec_charge)
mask = (rec_charge > x_min)*(rec_charge < x_max)
print("charge ratio rec/input", sum(rec_charge[rec_charge>0.3])/sum(mc_charge[mc_charge>0.3]))
print(len(rec_charge))
print(len(rec_charge[mask]))
#mean, std = gaussian.fit(rec_charge[mask])

rec_charge_hist, binEdges = np.histogram(rec_charge, bins=150,range=(0,3))

rec_charge_Std = np.sqrt(rec_charge_hist)


popt, pcov = curve_fit(chargDistr,
                       bincenters[bin_mask],
                       rec_charge_hist[bin_mask],
                       p0=(1,1.82,5.06083307e+03),
                       sigma = rec_charge_Std[bin_mask]
                       )
print(popt)
plt.plot(x,chargDistr(x,popt[0],popt[1],popt[2]),'r-')
plt.plot(bincenters,rec_charge_hist,'ro',label="Extracted charge: \n mean: %.4f"%popt[0]+"\n sigma: %.4f"%(1/np.sqrt(popt[1])))
plt.errorbar(bincenters,rec_charge_hist, yerr= rec_charge_Std,fmt='ro')
plt.xlabel("Charge [PE]")
plt.ylabel("Frequency [#]")
plt.legend()
plt.show()
