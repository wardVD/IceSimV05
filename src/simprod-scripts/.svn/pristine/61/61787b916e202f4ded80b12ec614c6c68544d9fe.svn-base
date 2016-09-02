#!/usr/bin/env python
##################################################################################
# A script designed to produce events containing only noise triggers. Contains
# both a normal tray segment (with everything except a random module and an
# I3Writer) and an iceprod module for SimProd generation.
#
# Each event produced will initally be a 100 ms long frame of noise. This will
# be triggered and then cut down using CoincidenceAfterProcessing from trigger-sim
# to create normal-sized events.
#
# @author Michael Larson (mjlarson@nbi.ku.dk)
# @date 15 July 2015
##################################################################################
from ..segments import ProduceNoiseTriggers
from .. import ipmodule

class NoiseTriggers(ipmodule.ParsingModule):
    def __init__(self):
        ipmodule.ParsingModule.__init__(self)

	self.AddParameter('gcdfile','GeoCalibDetStatus filename','')
        self.AddParameter('summaryfile','XMLSummary filename','')
	self.AddParameter('outputfile','Output filename','')
	self.AddParameter('mjd','MJD for the GCD file',56063)
        self.AddParameter("Detector","detector to simulate","IC86:2011")
	self.AddParameter('nevents','Number of events',1000)
	self.AddParameter("RunId","Configure run ID",0)
	self.AddParameter("RNGSeed","RNG seed",0)

    def Execute(self, stats):
        if not ipmodule.ParsingModule.Execute(self, stats): return 0

        tray = I3Tray()
        randomService = phys_services.I3GSLRandomService(self.rngseed)
        tray.context['I3RandomService'] = randomService

        tray.AddService("I3XMLSummaryServiceFactory","summary")(
            outputfilename=self.GetParameter('summaryfile'),
        )

        tray.AddSegment(ProduceNoiseTriggers, "noise_triggers",
                gcd_file = self.gcdfile,
                mjd = int(self.mjd),
                runId = self.RunId,
                nevents = self.nevents,
                )

        from ..util import BasicCounter, DAQCounter
        tray.AddModule(BasicCounter,"count_triggers",
                       Streams = [icetray.I3Frame.DAQ] ,
                       name="%s Triggered Events" % self.GetParameter('Detector'),
                       Stats=stats)
        
        skipkeys = [ "I3Triggers", "MCPMTResponseMap", "MCTimeIncEventID"]

        tray.AddModule("I3Writer","writer",
                       filename=self.outfile,
                       streams=[icetray.I3Frame.DAQ],
                       SkipKeys = skipkeys,
                       )

        tray.AddModule("TrashCan", "the can")
        tray.Execute(4+self.nevents)
        tray.Finish()
        return 0

