#!/usr/bin/env python

import os
import sys

if(os.getenv("I3_BUILD") == None):
    print("I3_BUILD not set.")
    sys.exit()

from I3Tray import *
from os.path import expandvars

if(len(sys.argv)!=6):
    print("Use: run.py [corsika/nugen/flasher] [gpu] [seed] [infile/num of flasher events] [outfile]")
    sys.exit()

mode =     sys.argv[1]
gpu  = int(sys.argv[2])
seed = int(sys.argv[3])
infile  =  sys.argv[4]
outfile =  sys.argv[5]

detector = False

gcdfile = "/data/ana/IC86/ppc/in/gcd/" #expandvars("$I3_TESTDATA/sim/")
os.putenv("PPCTABLESDIR", expandvars("$I3_BUILD/ppc/resources/ice/lea"))
os.putenv("OGPU", "1") # makes sure only GPUs are used (with OpenCL version)

from icecube import icetray, dataclasses, dataio, phys_services, sim_services

mjd = 55380
gcdfile += "GeoCalibDetectorStatus_IC86.55380_corrected.i3.gz"

tray = I3Tray()

tray.AddService("I3SPRNGRandomServiceFactory", "random")(
    ("Seed", seed),
    ("NStreams", 2),
    ("StreamNum", 1)
    )

if(mode != "nugen"):
    tray.AddModule("I3InfiniteSource", "muxer")(
        ("Prefix", gcdfile),
        ("Stream", icetray.I3Frame.DAQ)
        )

    def makeDrivingTime(frame): # needed by I3GlobalTriggerSim
        frame["DrivingTime"] = frame["I3EventHeader"].start_time
    tray.AddModule(makeDrivingTime, "makeDrivingTime", Streams=[icetray.I3Frame.DAQ])

if(mode == "corsika"):
    load("libucr-icetray")

    SHOWERS = 25000000
    FLUXSUM = 0.131475115
    LENGTH  = 1600
    RADIUS  = 800
    DEPTH   = 1950
    HEIGHT  = 2834

    ucrprogram = expandvars("$I3_BUILD/bin/ucr-icetray-ucr")  # ("$HOME/bin/ucr")

    ucropts = ucrprogram + " " + infile
    ucropts += " -oms -tr=2 -rmusr -over=1"
    ucropts += " -SHOWERS="+str(SHOWERS)
    ucropts += " -FLUXSUM="+str(FLUXSUM)
    ucropts += " -LENGTH="+str(LENGTH)
    ucropts += " -RADIUS="+str(RADIUS)
    ucropts += " -DEPTH="+str(DEPTH)
    ucropts += " -HEIGHT="+str(HEIGHT)
    ucropts += " -EARTHR=6.4e6 "
    ucropts += " -cutfe=273. -curved=4 -cutth=85 -rphi"  # -trigw=21

    tray.AddModule("I3GeneratorUCR", "generator")(
        ("EventsToIssue", SHOWERS),
        ("UCROpts", ucropts)
        )

    tray.AddModule("I3CorsikaWeightModule", "corsikaweight")(
        ("spectrumtype", 2),
        ("OverSampling", 1),
        ("nevents", SHOWERS),
        ("fluxsum", FLUXSUM),
        ("ThetaMin", 0.*I3Units.degree),
        ("ThetaMax", 89.99*I3Units.degree),
        ("cylinderlength", LENGTH*I3Units.meter),
        ("cylinderradius", RADIUS*I3Units.meter),
        ("energyprimarymin", 600.*I3Units.GeV),
        ("energyprimarymax", 1.e11*I3Units.GeV)
        )

if(mode == "nugen"):
    load("libppc-pick")

    LENGTH = 1700
    RADIUS = 1200

    tray.AddModule("I3Reader", "gcd")(
        ("FileNameList", [gcdfile, infile]),
        )

    tray.AddModule("I3ModifyEventID", "newmjd")(
        ("mjd", mjd),
        ("ModifyEventID", True)
        )

    tray.AddModule("I3IcePickModule<I3EpriFilt>","emax")(
        ("DiscardEvents", True),
        ("MaxEpri", 1.e11*I3Units.GeV)
        )

#load("libcudart")
#load("libOpenCL")

if(mode == "flasher"):
    num=int(infile)  # how many events

    fstr=63   # flashing string
    fdom=20   # flashing DOM
    bri=127  # flasher brightness (same setting as in flasher xml driver file)
    wid=124  # flasher width (same setting as in flasher xml driver file)
    ori=0    # azimuthal direction (in degrees) of the first LED
    mask=0b000000000001 # bitmask controlling which LEDs are turned on, 0 for off, 1 for on

    nled=bin(mask).count("1")

    # the nph setting below is the nominal value from the Flashers wiki
    # or is the average flasher photon yield derived as part of SPIE Lea.
    if False:           # nominal value from Flashers wiki page
        nph=8.e9        # per LED at maximum bri=127 and wid=127
    else:               # total number of real photons per LED from SPICE Lea fit
        nph=2.75*2.5e9  # fitted p_y in SPICE Lea * photon bunch
        nph/=0.1315     # DOM acceptance
        nph/=0.9*0.85   # shadowing * disc. threshold loss
        nph/=6*0.977316 # number of LEDs * correction for bri=127 wid=124 (as used in SPICE)
    # do not modify the above lines unless you think they contain an error!

    nph*=(0.0006753+0.00005593*bri)*(wid+13.9-57.5/(1+bri/34.4))

    if True: # whether to use simprod flasher segment
        from icecube.simprod import flashers
        tray.AddModule(flashers.GenerateFlashers,"GenerateFlashers",Streams=[icetray.I3Frame.DAQ],
		FlashString=fstr,
		FlashDOM=fdom,
		FlashBrightness=bri,
		FlashWidth=wid,
		FlashMask=mask)
    else:
        def fakeTree(frame): # needed by I3TimeShifter
            frame["I3MCTree"] = dataclasses.I3MCTree()
        tray.AddModule(fakeTree, "cascade", Streams=[icetray.I3Frame.DAQ])

    # Set FLDR=x+(n-1)*360, where 0<=x<360 and n>0 to simulate n LEDs in a
    # symmetrical n-fold pattern, with first LED centered in the direction x.
    # Negative or unset FLDR simulates a symmetric in azimuth pattern of light.

    fldr=ori+(nled-1)*360    # set fldr=-1 for azimuthally-symmetric emission
    
    os.putenv("FLDR", str(fldr))   # direction of the first flasher LED
    os.putenv("WFLA", "405")  # flasher wavelength; set to 337 for standard candles

    nph*=nled   # number of LEDs
    nph*=0.1315 # DOM acceptance at 405 nm. For SC use eff(337)=0.0354
    nph/=25     # DOM oversize factor squared; must match the value in cfg.txt
    nph*=0.9    # shadowing losses. Set efficiency correction in cfg.txt to 1.

    load("libxppc")
    load("libppc")

    if (int(mask)<64):
	fstr=-fstr

    tray.AddModule("i3ppc", "ppc")(
        ("gpu", gpu),
        ("nph", nph),
        ("wid", wid*0.5*I3Units.ns),
        ("fla", OMKey(fstr, fdom)) # set fstr=-fstr for tilted flashers, fstr=0 and fdom=1,2 for SC1 and 2
        )

else:
    num=0
    load("libmmc-icetray")

    tray.AddService("I3JavaVMFactory", "java")(
        ("Options", [expandvars("-Djava.class.path=$I3_BUILD/lib/mmc.jar")])
        )

    mmcopts = "-seed="+str(seed)
    mmcopts += " -radius="+str(RADIUS)
    mmcopts += " -length="+str(LENGTH)

    tray.AddModule("I3PropagatorMMC", "mmc")(
        ("rerr", "/dev/null"),
        ("mode", -1),
        ("opts", mmcopts)
        )

    load("libxppc")
    load("libppc")

    tray.AddModule("i3ppc", "ppc")(
        ("gpu", gpu),
        )

if(detector):
    load("libnoise-generator")
    load("libpmt-simulator")
    load("libDOMsimulator")
    load("libtrigger-sim")

    tray.AddModule("I3NoiseGeneratorModule", "noise")(
        ("StartWindow", 10.*I3Units.microsecond),
        ("EndWindow", 10.*I3Units.microsecond),
        ("Rate", 500.*I3Units.hertz),
        ("IndividualRates", True),
        ("Icetop", False),
        ("InIce", True)
        )
    tray.AddModule("I3PMTSimulator", "pmt")
    tray.AddModule("I3DOMsimulator", "dom")
    tray.AddModule("SimpleMajorityTrigger", "smt")
    tray.AddModule("I3GlobalTriggerSim", "trigger")
    tray.AddModule("I3Pruner", "pruner")
    tray.AddModule("I3TimeShifter", "shifter")

tray.AddModule("I3Writer", "writer")(
    ("streams", [icetray.I3Frame.DAQ]),
    ("SkipKeys", ["MCPMTResponseMap"]),
    ("filename", outfile)
    )

tray.AddModule("TrashCan", "the can")

if(num>0):
    tray.Execute(num+3)
else:
    tray.Execute()

tray.Finish()
