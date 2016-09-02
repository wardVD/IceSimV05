#!/usr/bin/env python
import time, math, os
from I3Tray import I3Tray, I3Units, NaN
from icecube import icetray, dataio, dataclasses, PROPOSAL

class Propagate(icetray.I3Module):
    def __init__(self,ctx):
        icetray.I3Module.__init__(self,ctx)
        #self.AddParameter("Length", "Length of the track",NaN)
        self.AddParameter('Medium', 'Medium to propagate through', dataclasses.I3Particle.InIce)
        self.AddOutBox("OutBox")

    def Geometry(self,frame):
        assert 'I3Geometry' in frame, "Input a GCD!"
        self.PushFrame(frame)

    def Configure(self):
        #self.Length     = self.GetParameter('Length')
        self.Medium      = self.GetParameter('Medium')
        #assert self.Length > 0, "Can't have negative length"

    def DAQ(self, frame):
        #Initialize propagator using PROPOSAL package
        particle = frame.Get('Primary')
        particlemass = frame.Get('PrimaryMass').value
        propagator = PROPOSAL.I3PropagatorServicePROPOSAL(tabledir = os.path.expandvars("$I3_BUILD/PROPOSAL/resources/tables/"), type = particle.type,particleMass=particlemass)
        
        particle.location_type = self.Medium
            
        daughters = propagator.Propagate(particle)

        tree = dataclasses.I3MCTree()

        tree.add_primary(particle)
        for d in daughters:
            tree.append_child(particle,d)
        frame['I3MCTree'] = tree
        del frame['Primary']
        frame['Primary']  = particle
            
        self.PushFrame(frame, 'OutBox')

        return 1
