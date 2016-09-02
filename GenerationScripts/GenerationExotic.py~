from I3Tray import I3Units
from icecube import dataclasses,icetray,dataio,phys_services,sim_services

import time, os

from icecube.exoticgenerator.Generate import Generate
from icecube.exoticpropagator.Propagate import Propagate

#############################################################################
def GenerationTray(params,tray):
    #Set up the time stuff
    tijd = dataclasses.I3Time()
    tijd.set_unix_time(int(time.time()), 0)
    
    randomService = phys_services.I3SPRNGRandomService(
        seed = 0,
        nstreams = 1,
        streamnum = 0)
    tray.AddService("I3SPRNGRandomServiceFactory",'random', Seed = 0)

    #make P frame: if we're dealing with PQ frames, set up the header!!!!! 
    #NO I3READER BECAUSE OF PREFIX = GCDFILE !!!!!!!
    tray.Add("I3InfiniteSource","streams", Stream=icetray.I3Frame.Physics, Prefix=params.gcd)
    tray.Add("I3MCEventHeaderGenerator","gen_header",
                  Year=2012,
                  DAQTime=158100000000000000,
                  RunNumber=1,
                  EventID=1,
                  IncrementEventID=True) 
        
    def DrivingTime(frame):
        if "DrivingTime" in frame :
            del frame["DrivingTime"]
        frame.Put("DrivingTime", tijd)

    tray.Add(DrivingTime, "dt", Streams = [icetray.I3Frame.Physics] )
    #tray.Add(lambda f: f.Put('DrivingTime',tijd) if "DrivingTime" not in f else del frame["DrivingTime"], "dt", Streams = [icetray.I3Frame.DAQ] )

    ######################################################
    #                  Particle Generation               #
    ######################################################
    tray.Add(Generate, "Gen",
             Mass = params.mass,
             Energy = params.energy,
             ZenithMin = params.zenithmin,
             ZenithMax = params.zenithmax,
             )

    ### print status message
    print "Generation tray finished"
    ### finish
    return tray

# END
###############################################################################

#############################################################################
def PropagationTray(params,tray):

    ######################################################
    #                 Particle Propagation               #
    ######################################################
    tray.Add(Propagate,"Prop")
    ### print status message
    print "Propagation tray finished"
    ### finish
    return tray

# END
###############################################################################
