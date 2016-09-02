#!/usr/bin/python 

#----------------------------------------------------------
# Generate simple root tree for MC particle info
# You need to install MCParticleExtractor project from
# http://code.icecube.wisc.edu/svn/sandbox/MCParticleExtractor/
#
# [Usage]
#
# python MakeSimpleMCRootTree.py [input_physics_file]
#
#

from I3Tray import *
from icecube import icetray, dataio, dataclasses
from ExtractTree import MakeTree
import os

#nevts=10000
#nevts=1000
nevts=100

global ievt
ievt=0

def evtcounter( frame ) :
    global ievt
    if ievt % 1000 == 0 :
    #if ievt % 1 == 0 :
    #if ievt % 10 == 0 :
        print("event %s start" % (ievt))
    ievt = ievt + 1
    return True

if (len(sys.argv) < 2 ) :
    print("Usage : python MakeSimpleMCRootTree.py [input_physics_file]")

filename=sys.argv[1]
rootfilename = filename + ".root"

load("libMCParticleExtractor")
load("libsimclasses")

#**************************
# tray 0
#**************************
tray = I3Tray()

tray.AddModule( "I3Reader", "Reader",
                Filename = filename )

#tray.AddModule("I3NullSplitter", "fullevent")

tray.AddModule("I3MCParticleExtractor", "mcextractor") 

tray.AddModule( MakeTree, "MakeTree",
                Filename = rootfilename)
 
tray.AddModule ( evtcounter, "evtcounter")

tray.AddModule( "TrashCan", "Bye" )
 
#tray.Execute(nevts)
tray.Execute()
tray.Finish()


