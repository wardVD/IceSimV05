"""
IceProd Modules for production NuGen
"""

import os,sys
from os.path import expandvars
import logging

from I3Tray import *
from icecube import (icetray, dataio, dataclasses, phys_services,
                     interfaces, simclasses, sim_services)

from .. import ipmodule

class NuGen(ipmodule.ParsingModule):

   def __init__(self):
        ipmodule.ParsingModule.__init__(self)

        self.AddParameter('gcdfile','GeoCalibDetStatus filename','')
        self.AddParameter('outputfile','Output filename','')
        self.AddParameter('summaryfile','XMLSummary filename','summary.xml')
        self.AddParameter('mjd','MJD for the GCD file',0)

        self.AddParameter("seed","RNG seed",0)
        self.AddParameter("procnum","RNG stream number",0)
        self.AddParameter("nproc","Number of RNG streams",1)

        self.AddParameter('nevents','Number of events',0)
        self.AddParameter('SimMode','simulation mode','FULL')
        self.AddParameter('VTXGenMode','currently supports only NuGen','NuGen')
        self.AddParameter('InjectionMode','injection mode','Surface')
        self.AddParameter('CylinderParams','For CIRCLE[radius, active_height_before, active_height_after],'
                                           ' for SURFACE[radius, length, center_x, center_y, center_z]',[0,0,0,0,0])
        self.AddParameter('AutoExtendMuonVolume','Detection volume extension, set off for starting events',True)
        self.AddParameter('NuFlavor','Neutrino Flavor','NuMu')
        self.AddParameter('Polyplopia','Produce coincident showers',False)
        self.AddParameter('gamma','Gamma index',2.0)
        self.AddParameter('FromEnergy','Minimum energy',1.*I3Units.TeV)
        self.AddParameter('ToEnergy','Maximum energy',10.*I3Units.PeV)
        self.AddParameter('ZenithMin','min zenith',0.)
        self.AddParameter('ZenithMax','max zenith',180.*I3Units.degree)
        self.AddParameter('AzimuthMin','min azimuth',0.)
        self.AddParameter('AzimuthMax','max azimuth',360.*I3Units.degree)
        self.AddParameter('CrossSections','cross section tables','csms')
        self.AddParameter('CrossSectionsPath','cross section tables path',None,explicit_type='string')
        self.AddParameter('ParamsMap','any other parameters',dict())
        self.AddParameter('PROPOSALParams','any other parameters for proposal',dict())
        self.AddParameter('BackgroundFile','pre-generated coincident showers file',"")
        self.AddParameter('HistogramFilename', 'Histogram filename.', None)


   def Execute(self,stats):
        if not ipmodule.ParsingModule.Execute(self,stats): return 0

        # Load libraries 
        from icecube import (neutrino_generator, earthmodel_service, PROPOSAL, cmc, phys_services)
        from .. import segments 
        
        # support json ParamsMap, so we can get our dict from the iceprod config file
        try:
            import json
        except ImportError:
            json = None
        if isinstance(self.paramsmap,str):  
            if not json:
                raise Exception('ParamsMap provided as string, but python does not understand json')
            self.paramsmap = json.loads(self.paramsmap)
            
        if isinstance(self.proposalparams,str):
            if not json:
                raise Exception('ProposalParams provided as string, but python does not understand json')
            self.proposalparams = json.loads(self.proposalparams) 	
        # Instantiate a tray 
        tray = I3Tray()

        randomService = phys_services.I3SPRNGRandomService(
             seed = self.seed, 
             nstreams = self.nproc, 
             streamnum = self.procnum)

        randomServiceForPropagators = phys_services.I3SPRNGRandomService(
             seed = self.seed,
             nstreams = self.nproc*2,
             streamnum = self.nproc + self.procnum)

        tray.context['I3RandomService'] = randomService
        tray.context['I3PropagatorRandomService'] = randomServiceForPropagators

        if not self.summaryfile:
            raise Exception('must define summary file')
        tray.AddService("I3XMLSummaryServiceFactory", "summary",
            outputfilename=self.summaryfile,
        )

        tray.AddModule("I3InfiniteSource","TheSource",
                       Prefix=self.gcdfile,
                       Stream=icetray.I3Frame.DAQ)
        tray.AddModule(DAQCounter, "counter3", nevents=int(self.nevents))

        tray.AddSegment(segments.GenerateNeutrinos, 'generator',
            RandomService=randomService,
            NumEvents=self.nevents,
            SimMode=self.simmode,
            VTXGenMode=self.vtxgenmode,
            InjectionMode=self.injectionmode,
            CylinderParams=self.cylinderparams,
            AutoExtendMuonVolume=self.autoextendmuonvolume,
            Flavor=self.nuflavor,
            GammaIndex=self.gamma,
            FromEnergy = self.fromenergy,
            ToEnergy = self.toenergy,
            ZenithRange=[self.zenithmin,self.zenithmax],
            AzimuthRange=[self.azimuthmin,self.azimuthmax],
            CrossSections=self.crosssections,
            CrossSectionsPath=self.crosssectionspath,
            ParamsMap=self.paramsmap
        )

        if self.polyplopia:
           tray.AddSegment(segments.PolyplopiaSegment,"coincify",
                    RandomService=randomService,
                    mctype='NuGen',
                    bgfile = self.backgroundfile,
                    timewindow = 40.*I3Units.microsecond,
                    rate = 5.0*I3Units.kilohertz,
           ) 
        
        tray.AddSegment(segments.PropagateMuons, 'propagator',
                        RandomService= randomServiceForPropagators,
                        **self.proposalparams
        ) 

        tray.AddModule(BasicCounter, "count_g",
                       Streams=[icetray.I3Frame.DAQ],
                       name="Generated Events",
                       Stats=stats,
        )

        if self.histogramfilename:         
            from icecube.production_histograms import ProductionHistogramModule
            from icecube.production_histograms.histogram_modules.simulation.mctree_primary import I3MCTreePrimaryModule
            from icecube.production_histograms.histogram_modules.simulation.mctree import I3MCTreeModule
        
            tray.AddModule(ProductionHistogramModule, 
                           Histograms = [I3MCTreePrimaryModule, I3MCTreeModule],
                           OutputFilename = self.histogramfilename)

        
        tray.AddModule("I3Writer", "writer",
            filename=self.outputfile,
            streams=[icetray.I3Frame.DAQ],
        )

        tray.AddModule("TrashCan","trashcan")

        # Execute the Tray
        tray.Execute()
        tray.Finish()

        tray.PrintUsage()
        for k in tray.Usage():
            stats[str(k.key())+":usr"] = k.data().usertime
            stats[str(k.key())+":sys"] = k.data().systime
            stats[str(k.key())+":ncall"] = k.data().systime

        # Free memory
        del tray
        return 0

class DAQCounter(icetray.I3Module):
    count   = 0
    nevents = 0

    def __init__(self,ctx):
        icetray.I3Module.__init__(self,ctx)
        self.AddParameter("NEvents","name of event counter",self.nevents)
        self.AddOutBox("OutBox");

    def Configure(self):
        self.nevents = self.GetParameter("NEvents")

    def DAQ(self,frame):
        self.count += 1
        if self.count > self.nevents: 
           self.RequestSuspension()
        self.PushFrame(frame)


def BasicCounter(frame,name="basic_counter",Stats={}):
    if name not in Stats:
        Stats[name]=0
    Stats[name] += 1


