#!/usr/bin/env python
from I3Tray import load
import os

#############################################################################################################
def LoadPhotonLibraries():
#Checking if symlinks are up to date
    creationxppc = os.popen("ls -la "+os.environ['I3_BUILD']+'/lib/libxppc.so').read()[44:50]
    creationcudart = os.popen("ls -la "+os.environ['I3_BUILD']+'/lib/libcudart.so').read()[44:50]

    assert creationcudart==creationxppc, "Seems like you need to remake xppc!"

    load("libcudart")
    load("xppc")
    load("ppc")
    load("libDOMLauncher")

    icemodel = "$I3_BUILD/ppc/resources/ice/lea"
    os.putenv("PPCTABLESDIR",os.path.expandvars(icemodel))

#############################################################################
def PhotonTray(tray):
    ### Libraries
    LoadPhotonLibraries()

    ######################################################
    #                   Light Simulation                 #
    ######################################################
    tray.Add('i3ppc','ppc',
             gpu = -1,
             MCTree = "I3MCTree",
             )

    ### print status message
    print "Light simulation finished"
    ### finish
    return tray

#############################################################################################################
# END
