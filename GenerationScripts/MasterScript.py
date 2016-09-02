#!/usr/bin/env python
#
#############################################################################
from GenerationExotic import GenerationTray,PropagationTray
from PhotonSimulation import PhotonTray
from optparse import OptionParser

from icecube import dataio,dataclasses,icetray

from I3Tray import I3Tray, I3Units
import os

class RunParameters:
    ### General parameters
    n_events    = 0
    detector    = ""
    gcd         = ""
    outputdir   = ""
    outputfile  = ""
    keep_all = False
    ### Generator parameters
    energy   = 0.*I3Units.TeV
    mass     = 0.*I3Units.GeV
    zenithmin = 0.*I3Units.deg
    zenithmax = 0.*I3Units.deg
    iteration = 0.

def parseOptions(parser,params):
    parser.add_option("-n", "--n_events", type=int, dest = 'n_events', default=3, help="Number of P-frames to produce")
    parser.add_option("-d", "--detector",default="IC86-2",dest="DETECTOR", help="What detector configuration? I.e. 'IC86-1', 'IC86-2',... default IC86-2.'")
    parser.add_option("-g", "--gcd",default=None,dest="GCDFILE", help="Read geometry from GCDFILE (.i3{.gz} format). Setting this to \"auto\" (the default) will select a GCD file according to the --detector parameter from $I3_TESTDATA")
    parser.add_option("-e", "--energy", type=float, default=10.,dest="ENERGY", help="Energy of particle in TeV (default 10)")
    parser.add_option("-m", "--mass", type=float,default=150.,dest="MASS", help="Mass of particle in GeV (default 150)")
    parser.add_option("--zenithmin", type=float,default=86.,dest="ZENITHMIN", help="Minimal Zenith angle in degrees (default 86)")
    parser.add_option("--zenithmax", type=float,default=180.,dest="ZENITHMAX", help="Maximal Zenith angle in degrees (default 180)")
    parser.add_option("-k", "--keep_all",default=False,dest="KEEPALL", help="Keep all information? Warning: files can become huge!")
    parser.add_option("-i", "--iteration", type=int, dest = 'ITERATION', default=0, help="Iteration number")
    parser.add_option("-o", "--output",default=None,dest="OUTPUT", help="What directory to save the output")

    (options,args) = parser.parse_args()

    if options.GCDFILE == None:
        if   options.DETECTOR == "IC86-3": options.GCDFILE = "/data/sim/sim-new/downloads/GCD/GeoCalibDetectorStatus_2013.56429_V1.i3.gz"
        elif options.DETECTOR == "IC86-2": options.GCDFILE = "/data/sim/sim-new/downloads/GCD/GeoCalibDetectorStatus_2012.56063_V1.i3.gz"
        elif options.DETECTOR == "IC86-1": options.GCDFILE = "/data/sim/sim-new/downloads/GCD/GeoCalibDetectorStatus_IC86.55697_corrected_V2.i3.gz"  
        else: raise SyntaxError("Define your detector configuration as you should.")

    assert options.OUTPUT != None, "Output directory is not defined. Exiting."
    assert os.path.exists(options.GCDFILE)," - GCD file %s not found!"%options.GCDFILE
    assert os.path.exists(options.OUTPUT)," - Output %s not found!"%options.OUTPUT

    params.n_events = options.n_events
    params.detector = options.DETECTOR
    params.gcd = options.GCDFILE
    params.keep_all = options.KEEPALL
    params.energy = options.ENERGY*I3Units.TeV
    params.mass = options.MASS*I3Units.GeV
    params.zenithmin = options.ZENITHMIN*I3Units.deg
    params.zenithmax = options.ZENITHMAX*I3Units.deg
    params.iteration = options.ITERATION
    params.outputdir = options.OUTPUT
    params.outputfile = "_".join([params.outputdir+"/Signal","Energy",str(params.energy)+"TeV","Mass",str(params.mass)+"GeV","n_Events",str(params.n_events),"Iteration",str(params.iteration)+".i3.bz2"])
    #params. = options.

def main(params,tray):
    ### Generate the exotics
    tray = GenerationTray(params,tray)
    ### Propagate the exotics
    tray = PropagationTray(params,tray)
    ### Simulate light propagation
    #tray = PhotonTray(params,tray)
    
    ### Write out/execute and finish
    tray.Add('I3Writer','writer',
             Filename = params.outputfile,
             streams = [icetray.I3Frame.Geometry,icetray.I3Frame.DAQ,icetray.I3Frame.DAQ])
    tray.Execute(params.n_events)
    tray.Finish()

#############################################################################
if (__name__=="__main__"):

    #get parameters, parse etc
    params = RunParameters()
    usage = "usage: %prog [options]"
    parser = OptionParser(usage)
    parseOptions(parser, params)
    tray = I3Tray()

    main (params,tray)

# End
#############################################################################    
