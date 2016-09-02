#!/usr/bin/env python

# This a working script taken from Ken for generating PINGU.
# The goal is to create something that can run using the latest
# release of simulation instead of the non-java decaf version


from I3Tray import *
import os
import os.path
import sys
import random

from .. import ipmodule


def AddEmptyWeights(frame): 
   frame["WeightDict"] = dataclasses.I3MapStringDouble()
   
def ResetMCPE(frame):
  frame.Delete("I3MCPESeriesMap")
  frame["I3MCPESeriesMap"] = frame["CleanedI3MCPESeriesMap"]
  frame.Delete("CleanedI3MCPESeriesMap")


class Genie(ipmodule.ParsingModule):

   def __init__(self):
        ipmodule.ParsingModule.__init__(self)

        self.AddParameter('gcdfile','GeoCalibDetStatus filename','')
        self.AddParameter('geniepath',"GENIE's very own ROOTSYS",'')
        self.AddParameter('outputfile','Output filename','')
        self.AddParameter('summaryfile','XMLSummary filename','')
        self.AddParameter('nevents','Number of events',1000)
        self.AddParameter('emax','Maximum energy',190.0*I3Units.GeV)
        self.AddParameter('emin','Minimun energy',1.0*I3Units.GeV)
        self.AddParameter('gamma','Gamma index',1.0)
        self.AddParameter('NuFlavor','Neutrino Flavor','NuE')
        self.AddParameter("seed","RNG seed",0)
        self.AddParameter("procnum","RNG stream number",0)
        self.AddParameter("nproc","Number of RNG streams",1)
        self.AddParameter('Polyplopia','Produce coincident showers',False)
        self.AddParameter('BackgroundFile','pre-generated coincident showers file',"")

   def Execute(self,stats):
        if not ipmodule.ParsingModule.Execute(self,stats): return 0

        from icecube import icetray
        from icecube import dataclasses
        from icecube import dataio
        from icecube import phys_services
        from icecube import sim_services
        from icecube import genie_icetray
        from icecube import simclasses


        # Instantiate a tray 
        icetray.set_log_level_for_unit('I3CLSimStepToPhotonConverterOpenCL', icetray.I3LogLevel.LOG_FATAL)
        tray = I3Tray()

        if not self.summaryfile:
            raise Exception('must define summary file')

        tray.AddService("I3XMLSummaryServiceFactory","summary")(
            ("outputfilename",self.summaryfile),
        )

        randomService = phys_services.I3SPRNGRandomService(
             seed = self.seed, 
             nstreams = self.nproc, 
             streamnum = self.procnum)

        randomServiceForPropagators = phys_services.I3SPRNGRandomService(
             seed = self.seed,
             nstreams = self.nproc*2,
             streamnum = self.nproc+ self.procnum)

        tray.context['I3RandomService'] = randomService
        tray.context['I3PropagatorRandomService'] = randomServiceForPropagators

        tray.AddModule("I3InfiniteSource","TheSource",
                       Prefix=self.gcdfile,
                       Stream=icetray.I3Frame.DAQ)
    
        tray.AddModule("I3GENIEGenerator","genie_generator",
 	       RandomService   = randomService, # alternatively, this can be None and the I3RandomService can be installed using tray.AddService()
 	       SplineFilename  = os.path.expandvars("$I3_BUILD/genie-icetray/resources/splines/splines_water_2.6.4.xml"),
 	       LHAPDFPath      = os.path.expandvars("$I3_BUILD/genie-icetray/resources/PDFsets"), # use $I3_PORTS/bin/lhapdf-getdata to download the PDFsets
 	       NuEnergyMin          = self.emin,
 	       NuEnergyMax          = self.emax,
 	       PowerLawIndex        = self.gamma, 
 	       GenVolRadius         = 800.*I3Units.m,
 	       GenVolLength         = 1200.*I3Units.m,
 	       GenVolDepth          = 1950.*I3Units.m,
	       NeutrinoFlavor       = self.nuflavor, # generates neutrinos and anti-neutrinos (1:1)
 	       MaterialDensity      = 0.93*I3Units.g/I3Units.cm3, # ice density
 	       TargetMixIngredients = [1000080160,1000010010], # O16, H1
	       TargetMixQuantities  = [1,2], # H2O (O16->1x, H1->2x)
 	       ForceSingleProbScale = False,
 	       NEvents              = self.nevents,
 	       GENIEPATH            = self.geniepath
 	)

        # Add empty weightdict 
        tray.AddModule(AddEmptyWeights)

        if self.polyplopia:

            from .. import segments
            tray.AddSegment(segments.PolyplopiaSegment,"coincify",
                    RandomService = randomService,
                    mctype = 'Genie',
                    mctree_name = "I3MCTree",
                    separate_coincident_mctree_name = "CoincidentI3MCTree_preMuonProp",
                    bgfile = self.backgroundfile,
                    timewindow = 40.*I3Units.microsecond,
                    rate = 5.0*I3Units.kilohertz,
            ) 

            tray.AddSegment(segments.PropagateMuons, 'propagator',
                    RandomService = randomServiceForPropagators,
                    InputMCTreeName ="CoincidentI3MCTree_preMuonProp",
                    OutputMCTreeName ="CoincidentI3MCTree",
            ) 
 
        tray.AddModule("I3Writer","writer")(
            ("filename",self.outputfile),
            ("streams",[icetray.I3Frame.DAQ])
        )

        tray.AddModule("TrashCan", "the can")
        tray.Execute()
        tray.Finish()
        return 0



class GeniePlusClSim(ipmodule.ParsingModule):

   def __init__(self):
        ipmodule.ParsingModule.__init__(self)

        self.AddParameter('gcdfile','GeoCalibDetStatus filename','')
        self.AddParameter('geniepath',"GENIE's very own ROOTSYS",'')
        self.AddParameter('outputfile','Output filename','')
        self.AddParameter('summaryfile','XMLSummary filename','')
        self.AddParameter('nevents','Number of events',1000)
        self.AddParameter('gpuopt','GPU',0)
        self.AddParameter('emax','Maximum energy',190.0*I3Units.GeV)
        self.AddParameter('emin','Minimun energy',1.0*I3Units.GeV)
        self.AddParameter('gamma','Gamma index',1.0)
        self.AddParameter('NuFlavor','Neutrino Flavor','NuE')
        self.AddParameter("RNGSeed","RNG seed",0)
        self.AddParameter("RNGStream","RNG stream number",0)
        self.AddParameter("RNGNumberOfStreams","Number of RNG streams",1)

        self.AddParameter("oversize","over-R: DOM radius oversize scaling factor",1)
        self.AddParameter("efficiency","overall DOM efficiency correction",0.9)
        self.AddParameter("volumecyl","set volume to regular cylinder (set to False for 300m spacing from the DOMs)",True)
        self.AddParameter("IceModelLocation","Location of ice model param files", os.path.expandvars("$I3_BUILD/clsim/resources/ice")) 
        self.AddParameter("IceModel","ice model subdirectory", "SpiceMie") 
        self.AddParameter("PhotonSeriesName","Photon Series Name","I3MCPESeriesMap") 
        self.AddParameter("RawPhotonSeriesName","Raw Photon Series Name",None)
        self.AddParameter('Polyplopia','Produce coincident showers',False)
        self.AddParameter('BackgroundFile','pre-generated coincident showers file',"")



   def Execute(self,stats):
        if not ipmodule.ParsingModule.Execute(self,stats): return 0

        from icecube import icetray
        from icecube import dataclasses
        from icecube import dataio
        from icecube import phys_services
        from icecube import sim_services
        from icecube import genie_icetray
        from icecube import simclasses
        from ..segments import PropagatePhotons
        from ..util import BasicHitFilter
        from I3Tray import I3Tray,I3Units


        icetray.set_log_level_for_unit('I3CLSimStepToPhotonConverterOpenCL', icetray.I3LogLevel.LOG_FATAL)

        # Instantiate a tray 
        tray = I3Tray()

        randomService = phys_services.I3SPRNGRandomService(self.rngseed, self.rngnumberofstreams, self.rngstream)
        randomServiceForPropagators = phys_services.I3SPRNGRandomService(
             seed = self.rngseed,
             nstreams = self.rngnumberofstreams*2,
             streamnum = self.rngnumberofstreams + self.rngstream)

        tray.context['I3RandomService'] = randomService
        tray.context['I3PropagatorRandomService'] = randomServiceForPropagators

        if not self.summaryfile:
            raise Exception('must define summary file')

        tray.AddService("I3XMLSummaryServiceFactory","summary")(
            ("outputfilename",self.summaryfile),
        )

        tray.AddModule("I3InfiniteSource","TheSource",
                       Prefix=self.gcdfile,
                       Stream=icetray.I3Frame.DAQ)
    
        tray.AddModule("I3GENIEGenerator","genie_generator",
 	       # alternatively, this can be None and the I3RandomService can 
 	       # be installed using tray.AddService()
 	       RandomService = randomService, 
 	       SplineFilename = os.path.expandvars(
				"$I3_BUILD/genie-icetray/resources/splines/splines_water_2.6.4.xml"),
 	       # use $I3_PORTS/bin/lhapdf-getdata to download the PDFsets
 	       LHAPDFPath = os.path.expandvars(
				"$I3_BUILD/genie-icetray/resources/PDFsets"), 
 	       NuEnergyMin = self.emin,
 	       NuEnergyMax = self.emax,
 	       PowerLawIndex = self.gamma, 
 	       GenVolRadius = 650.*I3Units.m,
 	       GenVolLength = 1200.*I3Units.m,
 	       GenVolDepth  = 1950.*I3Units.m,
	       NeutrinoFlavor = self.nuflavor, # generates neutrinos and anti-neutrinos (1:1)
 	       MaterialDensity = 0.93*I3Units.g/I3Units.cm3, # ice density
 	       TargetMixIngredients = [1000080160,1000010010], # O16, H1
	       TargetMixQuantities = [1,2], # H2O (O16->1x, H1->2x)
 	       ForceSingleProbScale = False,
 	       NEvents = self.nevents,
 	       GENIEPATH  = self.geniepath
 	)

        tray.AddSegment(PropagatePhotons,"photons",
			UseGPUs = self.gpuopt,
			UnshadowedFraction = self.efficiency,
			DOMOversizeFactor = self.oversize,
			IceModelLocation = self.icemodellocation,
                        HybridMode = False,
			IceModel = self.icemodel,
			OutputPESeriesMapName = self.photonseriesname,
			OutputPhotonSeriesName = self.rawphotonseriesname,
			UseGeant4 = True,
		)
        tray.AddModule(BasicHitFilter,"hitfilter",
			Streams = [icetray.I3Frame.DAQ],
			HitSeriesMapNames = self.photonseriesname)

        # Add empty weightdict 
        tray.AddModule(AddEmptyWeights)
        tray.AddModule("I3RemoveLargeDT","removeLDT")
        tray.AddModule(ResetMCPE,'reset',Streams=[icetray.I3Frame.DAQ])

        if self.polyplopia:

            from .. import segments
            tray.AddSegment(segments.PolyplopiaSegment,"coincify",
                    RandomService=randomService,
                    mctype='Genie',
                    mctree_name="I3MCTree",
                    separate_coincident_mctree_name="CoincidentI3MCTree_preMuonProp",
                    bgfile = self.backgroundfile,
                    timewindow = 40.*I3Units.microsecond,
                    rate = 5.0*I3Units.kilohertz,
            ) 

            tray.AddSegment(segments.PropagateMuons, 'propagator',
                    RandomService = randomServiceForPropagators,
                    InputMCTreeName = "CoincidentI3MCTree_preMuonProp",
                    OutputMCTreeName = "CoincidentI3MCTree",
            ) 

        tray.AddModule("I3Writer","writer")(
            ("filename",self.outputfile),
            ("streams",[icetray.I3Frame.DAQ])
        )

        tray.AddModule("TrashCan", "the can")
        tray.Execute()
        tray.Finish()
        return 0
