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

compareWithRosencrantz = False

GCDFN = expandvars("$I3_TESTDATA/sim/GeoCalibDetectorStatus_IC86.55697_corrected_V2.i3.gz")

usage = "usage: %prog [options] outputfile"
parser = OptionParser(usage)
parser.add_option("-s", "--seed",type="int",default=12345,
                  dest="SEED", help="Initial seed for the random number generator")
parser.add_option("-n", "--numevents", type="int", default=100,
                  dest="NUMEVENTS", help="The number of events per run")
parser.add_option("--comparePulses", dest="comparePulses", default=False, action="store_true")
parser.add_option("-g","--gcd", dest="gcd", type="string",default=GCDFN)

# parse cmd line args, bail out if anything is not understood
(options,args) = parser.parse_args()

tray = I3Tray()

tray.AddService("I3GSLRandomServiceFactory","Random")

tray.AddModule("I3InfiniteSource","streams",
               Stream=icetray.I3Frame.DAQ,
               prefix=options.gcd,
               )

def makeHits(frame):
        hits = simclasses.I3MCPESeriesMap()
        geo = frame["I3Geometry"]
        dom = list(geo.omgeo.keys())[2]
        hits[dom] = simclasses.I3MCPESeries()
        for i in range(0,10):
                hit = simclasses.I3MCPE()
                hit.npe=10
                hit.time=10*i
                hits[dom].append(hit)
        dom = list(geo.omgeo.keys())[3]
        hits[dom] = simclasses.I3MCPESeries()
        for i in range(0,10):
                hit = simclasses.I3MCPE()
                hit.npe=10
                hit.time=10*i
                hits[dom].append(hit)
        frame["MCPESeriesMap"] = hits
        frame["I3EventHeader"] = dataclasses.I3EventHeader()

tray.AddModule(makeHits,"hitterer",Streams=[icetray.I3Frame.DAQ])
#tray.AddModule("Dump","dumper")
tray.AddModule("PMTResponseSimulator", "Rosencrantz",
        Input = "MCPESeriesMap",
        Output = "ProcessedMCPulses",
        #UseSPEDistribution = False
        )
#tray.AddModule("Dump","dumper2")
tray.AddModule("DOMLauncher", "Guildenstern",
        Input = "ProcessedMCPulses",
        Output = "InIceRawData",
        )

tray.AddModule('I3WaveCalibrator', 'domcal')
tray.AddModule('I3Wavedeform', 'deform')

n = 0
avg = 0.0

def summarize(frame):
        if(options.comparePulses):
                hits=frame['MCPESeriesMap']
        else:
                hits=frame['ProcessedMCPulses']
        pulses=frame['WavedeformPulses']

        nHits=0.0
        for om in hits.keys():
                for hit in hits[om]:
                        if(options.comparePulses):
                                nHits += hit.npe
                        else:
                                nHits += hit.charge
        charge=0.0
        for om in pulses.keys():
                for pulse in pulses[om]:
                        charge += pulse.charge

        global n
        n += 1
        global avg
        avg += charge/nHits


tray.AddModule(summarize,'sum',Streams=[icetray.I3Frame.DAQ])

tray.AddModule('I3Writer','writer',Filename="roundtrip.i3",Streams=[icetray.I3Frame.DAQ])

tray.AddModule('TrashCan','~')
tray.Execute(options.NUMEVENTS+3)
tray.Finish()

print(avg/n)
