#!/usr/bin/env python

from I3Tray import I3Tray
from icecube.icetray import I3Test
from icecube import icetray
from icecube import dataclasses
from icecube import phys_services
from icecube.trigger_sim.modules.time_shifter import I3TimeShifter
from icecube import simclasses

TIME = 999.9
def TestSetup(frame, add_empty_trigger = False, add_mchits = True ) :

    p = dataclasses.I3Particle()
    p.time = TIME

    frame["MCPrimary"] = p

    mctree = dataclasses.I3MCTree()
    mctree.add_primary(p)
    frame["MCTree"] = mctree

    # I3MMCTrack
    mmctrack = simclasses.I3MMCTrack()
    mmctrack.SetParticle(p)
    mmctrack.ti = TIME
    mmctrack.tc = TIME
    mmctrack.tf = TIME
    mmctracklist = simclasses.I3MMCTrackList()
    mmctracklist.append(mmctrack)
    frame["MMCTracks"] = mmctracklist

    # I3TriggerHierarchy
    t = dataclasses.I3Trigger()
    t.time = TIME
    t.fired = True
    trigger_h = dataclasses.I3TriggerHierarchy()
    if not add_empty_trigger :
        trigger_h.insert(t)
    frame["Trigger"] = trigger_h

    # I3VectorI3Trigger
    t1 = dataclasses.I3Trigger()
    t1.time = TIME
    t1.fired = True
    trigger_v = dataclasses.I3VectorI3Trigger()
    trigger_v.append(t1)
    frame["TriggerVector"] = trigger_v

    # I3FlasherInfoVector
    fi = dataclasses.I3FlasherInfo()
    fi.flash_time = TIME
    fi_v = dataclasses.I3FlasherInfoVect()
    fi_v.append(fi)
    frame["FlasherInfos"] = fi_v

    # I3DOMLaunchSeriesMap
    d = dataclasses.I3DOMLaunch()
    d.time = TIME
    launchseries = dataclasses.I3DOMLaunchSeries()
    launchseries.append(d)
    launchmap = dataclasses.I3DOMLaunchSeriesMap()
    launchmap[icetray.OMKey(21,30)] = launchseries
    frame["DOMLaunchMap"] = launchmap
        
    # I3MCHit
    mchit = dataclasses.I3MCHit()
    mchit.time = TIME
    mchitseries = dataclasses.I3MCHitSeries()
    mchitseries.append(mchit)
    mchitmap = dataclasses.I3MCHitSeriesMap()
    mchitmap[icetray.OMKey(21,30)] = mchitseries
    if add_mchits :
        frame["MCHitMap"] = mchitmap

    # I3MCPE
    mcpe = simclasses.I3MCPE()
    mcpe.time = TIME
    mcpeseries = simclasses.I3MCPESeries()
    mcpeseries.append(mcpe)
    mcpemap = simclasses.I3MCPESeriesMap()
    mcpemap[icetray.OMKey(21,30)] = mcpeseries
    frame["MCPEMap"] = mcpemap

    # I3MCPulse
    mcpulse = simclasses.I3MCPulse()
    mcpulse.time = TIME
    mcpulseseries = simclasses.I3MCPulseSeries()
    mcpulseseries.append(mcpulse)
    mcpulsemap = simclasses.I3MCPulseSeriesMap()
    mcpulsemap[icetray.OMKey(21,30)] = mcpulseseries
    frame["MCPulseMap"] = mcpulsemap
    
    # I3Double
    sometime = dataclasses.I3Double(TIME)
    frame["SomeTime"] = sometime
    nottime = dataclasses.I3Double(TIME)
    frame["NotTime"] = nottime

def TestShift(frame, expect_mchits = True, expect_trigger = True):

    print(frame)

    # I3TriggerVector
    for t in frame["TriggerVector"] :
        print("TriggerVector t.time = ",t.time)
        I3Test.ENSURE(t.time == 0, "I3TriggerVector")

    # I3TriggerHierarchy
    if expect_trigger :
        for t in frame["Trigger"] :
            print("TriggerH t.time = ",t.time)
            I3Test.ENSURE(t.time == 0, "I3Trigger")

    # I3FlasherInfoVector
    for fi in frame["FlasherInfos"] :
        print("FlasherInfo fi.flash_time = ",fi.flash_time)
        I3Test.ENSURE(fi.flash_time == 0, "I3FlasherInfo")

    # I3DOMLaunch
    for om,ls in frame["DOMLaunchMap"] :
        for l in ls :
            print("I3DOMLaunch l.time = ",l.time)
            I3Test.ENSURE(l.time == 0, "I3DOMLaunch")

    # I3MCHit
    if expect_mchits :
        for om,hs in frame["MCHitMap"] :
            for h in hs :
                print("I3MCHit h.time = ",h.time)
                I3Test.ENSURE(h.time == 0, "I3MCHit t = %f" % h.time)

    # I3MCPE
    for om,pes in frame["MCPEMap"] :
        for pe in pes :
            print("I3MCPE pe.time = ", pe.time)
            I3Test.ENSURE(pe.time == 0, "I3MCPE t = %f" % pe.time)

    # I3MCPulses
    for om,pulses in frame["MCPulseMap"] :
        for pulse in pulses :
            print("I3MCPulse pulse.time = ", pulse.time)
            I3Test.ENSURE(pulse.time == 0, "I3MCPulse t = %f" % pulse.time)

    # I3MCParticle
    print("MCPrimary.time = ",frame["MCPrimary"].time)
    I3Test.ENSURE(frame["MCPrimary"].time == 0, "MCPrimary time is %f" \
                  % frame["MCPrimary"].time)

    # I3MCTree
    for p in frame["MCTree"] :
        print("I3MCTree p.time = ",p.time)
        I3Test.ENSURE(p.time == 0, "I3MCTree time is %f"\
                      % p.time)
                              

    # I3MMCTrack
    for mmctrack in frame["MMCTracks"] :
        print("mmctrack.ti = ",mmctrack.ti)
        print("mmctrack.tc = ",mmctrack.tc)
        print("mmctrack.tf = ",mmctrack.tf)
        print("mmctrack.GetI3Particle().time = ",mmctrack.GetI3Particle().time)
        I3Test.ENSURE(mmctrack.ti == 0., "I3MMCTrack.ti = %f" % mmctrack.ti)
        I3Test.ENSURE(mmctrack.tc == 0., "I3MMCTrack.tc = %f" % mmctrack.tc)
        I3Test.ENSURE(mmctrack.tf == 0., "I3MMCTrack.tf = %f" % mmctrack.tf)
        I3Test.ENSURE(mmctrack.GetI3Particle().time == 0., \
                      "I3MMCTrack.I3Particle.time = %f"\
                        % mmctrack.GetI3Particle().time)

    # I3Double
    I3Test.ENSURE(frame["SomeTime"].value == 0., \
                  "SomeTime value = %f" % frame["SomeTime"].value)
    I3Test.ENSURE(frame["NotTime"].value == TIME, \
                  "NotTime value = %f" % frame["NotTime"].value)

    print("SomeTime value = %f" % frame["SomeTime"].value)
    print("NotTime value = %f" % frame["NotTime"].value)

# The following tests standard operating conditions
# I3TimeShifter should determine the time shift from
# the trigger hierarchy in the frame
tray = I3Tray()
tray.AddModule("I3InfiniteSource")
tray.AddModule(TestSetup, streams = [icetray.I3Frame.DAQ])
tray.AddModule(I3TimeShifter,\
               SkipKeys = ["NotTime"],
               ShiftI3DoubleKeys = ["SomeTime"])
tray.AddModule(TestShift, streams = [icetray.I3Frame.DAQ])
tray.Execute(1)


# The trigger heirarchy doesn't exist so will look for I3MCPEs to 
# compute the time shift.
def remove_frame_object(frame, key) : 
    del frame[key]

tray = I3Tray()
tray.AddModule("I3InfiniteSource")
tray.AddModule(TestSetup, \
               streams = [icetray.I3Frame.DAQ])
tray.AddModule(remove_frame_object, \
               streams = [icetray.I3Frame.DAQ], \
               key = "Trigger")
tray.AddModule(I3TimeShifter,\
               SkipKeys = ["NotTime"],
               ShiftI3DoubleKeys = ["SomeTime"])
tray.AddModule(TestShift, \
               streams = [icetray.I3Frame.DAQ],\
               expect_trigger = False)
tray.Execute(1)

# No trigger and no I3MCHits
tray = I3Tray()
tray.AddModule("I3InfiniteSource")
tray.AddModule(TestSetup, \
               streams = [icetray.I3Frame.DAQ],\
               add_mchits = False)
tray.AddModule(remove_frame_object, \
               streams = [icetray.I3Frame.DAQ], \
               key = "Trigger")
tray.AddModule(I3TimeShifter,\
               SkipKeys = ["NotTime"],
               ShiftI3DoubleKeys = ["SomeTime"])
tray.AddModule(TestShift, \
               streams = [icetray.I3Frame.DAQ],\
               expect_trigger = False,\
               expect_mchits = False)
tray.Execute(1)

# The following has an empty trigger hierarchy in the frame, 
# so should shift to the earliest I3MCPE instead.
tray = I3Tray()
tray.AddModule("I3InfiniteSource")
tray.AddModule(TestSetup, \
               streams = [icetray.I3Frame.DAQ], \
               add_empty_trigger = True)
tray.AddModule(I3TimeShifter, \
               SkipKeys = ["NotTime"], 
               ShiftI3DoubleKeys = ["SomeTime"])
tray.AddModule(TestShift, streams = [icetray.I3Frame.DAQ])
tray.Execute(1)

