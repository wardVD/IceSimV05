#!/usr/bin/env python

from I3Tray import I3Tray
from icecube import icetray
from icecube.icetray.round_robin_streams import RoundRobinStreams
import os

tray = I3Tray()

#
#  Will issue frames on the 4 rotating streams
#
tray.AddModule(RoundRobinStreams, "bs",
               Streams = [icetray.I3Frame.Geometry,
                          icetray.I3Frame.DetectorStatus,
                          icetray.I3Frame.Calibration,
                          icetray.I3Frame.Physics]
               )

#
#  Run int-putter on only calibration and trayinfo frames
#
def putint(frame):
    frame['foo'] = icetray.I3Int(7)
    
tray.AddModule(putint, "putint",
               Streams = [icetray.I3Frame.Calibration,
                          icetray.I3Frame.Physics])

def printer(frame):
    print(frame.GetStop())
    print(frame)

# tray.AddModule(printer, 'p')

#
#   Filter out frames the putint didn't see
#
tray.AddModule(lambda x: 'foo' in x, "filter",
               Streams = [icetray.I3Frame.Geometry,
                          icetray.I3Frame.DetectorStatus,
                          icetray.I3Frame.Calibration,
                          icetray.I3Frame.Physics])

tray.AddModule("Dump", "dump")
tray.AddModule("CountFrames", "count",
               Calibration = 2,
               Physics = 2,
               # The following we get via mix-in
               DetectorStatus = 1,
               Geometry = 2)
               

tray.AddModule("TrashCan", "tc")

tray.Execute(8)
tray.Finish()


    

