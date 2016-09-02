from icecube import icetray
from icecube import dataclasses


def launch_splitter(frame, mapname = "I3DOMLaunchSeriesMap"):
    '''
        A one-off moudule to split the output DOMLaunchSeriesMap from DOMLauncher into an
        InIce part and IceTop part.
    '''
    geo = frame["I3Geometry"]
    launch_map = frame[mapname]
    inice = dataclasses.I3DOMLaunchSeriesMap()
    icetop = dataclasses.I3DOMLaunchSeriesMap()
    beacon = dataclasses.I3DOMLaunchSeriesMap()
    for omkey,launch_series in launch_map :
        g = geo.omgeo[omkey]
        #First we find the beacon launches and remove them
        #from the launch series
        index = 0
        spetriggered = dataclasses.I3DOMLaunchSeries()
        for l in launch_series:
            if(l.trigger_type == dataclasses.I3DOMLaunch.CPU_REQUESTED):
                if(omkey in beacon):
                    beacon[omkey].append(l)
                else:
                    beacon[omkey] =  dataclasses.I3DOMLaunchSeries([l])
            else:
                spetriggered.append(l)
            index += 1



        launch_series = spetriggered
        if (len(launch_series) == 0):
            continue

        if (g.omtype == dataclasses.I3OMGeo.IceCube):
            inice[omkey] = launch_series
        elif (g.omtype == dataclasses.I3OMGeo.IceTop) :
            icetop[omkey] = launch_series
        else :
            icetray.logging.log_fatal("Unknown DOM type %s at %s in the launch map" \
                                    % (str(g.omtype), str(omkey)))

    frame["InIceRawData"] = inice
    frame["IceTopRawData"] = icetop
    frame["BeaconLaunches"] = beacon
    del frame[mapname]


def mcpulse_to_recopulse(frame, mapname = "I3MCPulseSeriesMap", outputmap = "I3RecoPulseSeriesMap"):
    '''
        A module that does a direct conversion of I3MCPulses to I3RecoPulses.
        It is intended to be used with PMTResponseSimulator output when  one
        wants to avoid the DOM simulation for some reason (no DOM electronic simulation. ie no launches but
        PMT effects such as saturation is present).
    '''
    recopulsemap = dataclasses.I3RecoPulseSeriesMap()
    mcpulsemap = frame[mapname]
    for omkey, pulses in mcpulsemap:
        recopulsemap[omkey] = dataclasses.I3RecoPulseSeries()
        for pulse in pulses:
            rpulse = dataclasses.I3RecoPulse()
            rpulse.time = pulse.time
            rpulse.charge = pulse.charge
            rpulse.flags = dataclasses.I3RecoPulse.PulseFlags.LC
            recopulsemap[omkey].append(rpulse)

    frame[outputmap] = recopulsemap


def effective_domsim(frame, mapname = "I3MCPulseSeriesMap", outputmap = "I3RecoPulseSeriesMap"):
    '''
        A module similar to mcpulse_to_recopulse above. It does an effective DOM
        electronics simulation by adding jitter to the time and charge to the pulse
        while converting the MCPulse to a reco pulse. The values for the spread of the gaussian
        jitter were found by studying the spread of 1PE extracted pulses.
        The module is intended to be use with the PMTResponseSimulator output in situations
        when one wants to avoid the actual DOM simulation for some reason.
    '''
    from icecube.icetray import I3Units
    recopulsemap = dataclasses.I3RecoPulseSeriesMap()
    mcpulsemap = frame[mapname]
    for omkey, pulses in mcpulsemap:
        recopulsemap[omkey] = dataclasses.I3RecoPulseSeries()
        for pulse in pulses:
            rpulse = dataclasses.I3RecoPulse()
            rpulse.time = random_service.gaus(pulse.time,2.0*I3Units.ns)
            rpulse.charge = random_service.gaus(pulse.charge,0.012)#PE
            rpulse.flags = dataclasses.I3RecoPulse.PulseFlags.LC
            recopulsemap[omkey].append(rpulse)

    frame[outputmap] = recopulsemap


## DOMLauncher
## This traysegment is intended for standard usage of detector simulation.
@icetray.traysegment
def DetectorResponse(tray, name,
                     pmt_config = dict(),
                     dom_config = dict()):

  #create PMT response
  tray.AddModule('PMTResponseSimulator', name + "_pmt",**pmt_config)

  #create Detector response
  tray.AddModule('DOMLauncher', name+'_dommb',**dom_config)

  tray.AddModule(launch_splitter, "split", streams = [icetray.I3Frame.DAQ])
