#!/usr/bin/env python
import time
from I3Tray import I3Tray, I3Units
from icecube import icetray, dataio, dataclasses, phys_services

from icecube.common_variables import hit_statistics, track_characteristics

class Extractor(icetray.I3Module):
    def __init__(self, context):
        icetray.I3Module.__init__(self, context)
        self.AddOutBox("OutBox")
        self.AddParameter("Pulses",
                          "Name of the input RecoPulseSeriesMap or RecoPulseSeriesMapMask",
                          "HLCPulseSeriesReco")
        self.AddParameter("MCTree",
                          "Name of the I3MCTree",
                          "I3MCTree")
        self.AddParameter("OutputNPE",
                          "Name of the output frame object",
                          "NPE")
        self.AddParameter("OutputEnergy",
                          "Name of the output frame object",
                          "Energy")
        self.AddParameter("OutputTrackLength",
                          "Name of the output frame object",
                          "TrackLength")
        self.AddParameter("OutputZenith",
                          "Name of the output frame object",
                          "Zenith")
        self.AddParameter("OutputAzimuth",
                          "Name of the output frame object",
                          "Azimuth")
        self.AddParameter("OutputZTravel",
                          "Name of the output frame object",
                          "ZTravel")
        self.AddParameter("OutputTrackSeparationLength",
                          "Name of the output frame object",
                          "TrackSeparationLength")
        self.AddParameter("OutputTrackDistribution",
                          "Name of the output frame object",
                          "TrackDistribution")
        self.AddParameter("GetMostEnergeticMuon",
                          "Do you want to get the most energetic muon as primary?",
                          False)

    def Configure(self):
        self.pmap   = self.GetParameter("Pulses")
        self.mctree = self.GetParameter("MCTree")
        self.outputnpe = self.GetParameter("OutputNPE")
        self.outputenergy = self.GetParameter("OutputEnergy")
        self.outputtrack = self.GetParameter("OutputTrackLength")
        self.outputzenith = self.GetParameter("OutputZenith")
        self.outputazimuth = self.GetParameter("OutputAzimuth")
        self.outputztravel = self.GetParameter("OutputZTravel")
        self.outputtrackseparation = self.GetParameter("OutputTrackSeparationLength")
        self.outputtrackdistribution = self.GetParameter("OutputTrackDistribution")
        self.getmostenergeticmuon = self.GetParameter("GetMostEnergeticMuon")

    def Physics(self, frame):
        # Get PulseSeriesMap from frame
        if frame.Has(self.pmap):
            pulsemap = frame.Get(self.pmap)
            # treat mask correct like pulsemap
            if pulsemap.__class__==dataclasses.I3RecoPulseSeriesMapMask:
                pulsemap = pulsemap.apply(frame)
        else:
            print "ERROR (Extractor): Frame has no object named %s" % self.pmap
            exit(1)

        # Initialize
        npe = 0.
        for key,data in pulsemap:
            for pulse in data:
                npe += pulse.charge

        frame.Put(self.outputnpe, dataclasses.I3Double(npe))
        
        #Get MCTree
        if frame.Has(self.mctree):
            mcTree = frame.Get(self.mctree) 
        else:
            print "\nERROR (EXtractor): Frame has no object named %s\n" %self.mctree
        
        if self.getmostenergeticmuon and frame.Has('ConventionalWeight'):
            theparticle = dataclasses.get_most_energetic_muon(mcTree)
        else:
            theparticle = mcTree[0]
            
        energy  = theparticle.energy
        length  = theparticle.length
        azimuth = theparticle.dir.azimuth
        zenith  = theparticle.dir.zenith

            
        frame.Put(self.outputenergy, dataclasses.I3Double(energy))
        frame.Put(self.outputtrack, dataclasses.I3Double(length))
        frame.Put(self.outputazimuth, dataclasses.I3Double(azimuth))
        frame.Put(self.outputzenith, dataclasses.I3Double(zenith))

        hit_statistics_values = hit_statistics.calculate_hit_statistics(
            frame['I3Geometry'],
            pulsemap
            )
        track_characteristics_values = track_characteristics.calculate_track_characteristics(
            frame['I3Geometry'],
            pulsemap,
            theparticle,
            150.*I3Units.m
            )
        
        frame.Put(self.outputztravel, dataclasses.I3Double(hit_statistics_values.z_travel/I3Units.m))
        frame.Put(self.outputtrackseparation, dataclasses.I3Double(track_characteristics_values.track_hits_separation_length/I3Units.m))
        frame.Put(self.outputtrackdistribution, dataclasses.I3Double(track_characteristics_values.track_hits_distribution_smoothness))

        self.PushFrame(frame)

        return 1
