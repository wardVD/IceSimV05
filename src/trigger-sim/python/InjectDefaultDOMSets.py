#!/usr/bin/env python
from I3Tray import *

from icecube import icetray, dataclasses, dataio
from icecube.trigger_sim import GetDefaultDOMSets

class InjectDefaultDOMSets(icetray.I3Module):
    """
    This module adds the default DOMSets information
    to the DetectorStatus frame. This will be the same
    data that older versions of trigger-sim used.
    
    The module will not overwrite existing information.
    """

    def __init__(self, context):
        icetray.I3Module.__init__(self, context)
        self.AddParameter('DOMSetsName', 'Name of the DOMSets frame object', 'DOMSets')
        self.AddOutBox("OutBox")
        
    def Configure(self):
        self.domSetsName = self.GetParameter('DOMSetsName')

    def DetectorStatus(self, frame):
        if self.domSetsName in frame:
            print(" *** There already is a frame object names %s in your DetectorStatus frame.")
            print(" *** This object will not be overwritten!")
            self.PushFrame(frame)
            return

        # do the magic
        frame[self.domSetsName] = GetDefaultDOMSets()
        
        self.PushFrame(frame)
