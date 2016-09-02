#!/usr/bin/env python
################################ Tray 1 ##############################
#   
#   IceProd classes for processing hit-generation in IceSim.
#   Author: juancarlos@icecube.wisc.edu
#   
######################################################################
import os
import sys,getopt,string
from os.path import expandvars
from .. import ipmodule
from ..util import CombineHits, DrivingTime, choose_max_efficiency
from ..util.fileutils import download,untar,isurl

def LoadCascadeTables(Use_cvmfs=False, amplitude_fits = None, timing_fits = None):

     # Now we import the photon stuff
     from icecube import photonics_service

     # get the amplitude spline table
     if Use_cvmfs:
            spline_path = os.path.join(Use_cvmfs,'data','photon-tables','splines',
                                       os.path.basename(amplitude_fits))
     if Use_cvmfs and os.path.exists(spline_path):
            amplitude_fits = spline_path
     elif isurl(amplitude_fits):
            download(amplitude_fits)
            amplitude_fits = os.path.basename(amplitude_fits)
        
     # get the timing spline table
     if Use_cvmfs:
            spline_path = os.path.join(Use_cvmfs,'data','photon-tables','splines',
                                       os.path.basename(timing_fits))
     if Use_cvmfs and os.path.exists(spline_path):
            timing_fits = spline_path
     elif isurl(timing_fits):
            download(timing_fits)
            timing_fits = os.path.basename(timing_fits)

     return photonics_service.I3PhotoSplineService(
           amplitudetable=amplitude_fits,
           timingtable=timing_fits,
           timingSigma=0.)



####### IPModules ##################

class ClSim(ipmodule.ParsingModule):
   """
    GPU Photon propagation
   """
   def __init__(self):
        ipmodule.ParsingModule.__init__(self)

        self.AddParameter('gcdfile','GeoCalibDetStatus filename','')
        self.AddParameter('inputfilelist','list of input filenames',[])
        self.AddParameter('outputfile','Output filename','')
        self.AddParameter('seed','RNG seed',0)
        self.AddParameter('procnum','job number (RNG stream number)',0)
        self.AddParameter('nproc','Number of jobs (Number of RNG streams)',1)
        self.AddParameter('summaryfile','XMLSummary filename','summary.xml')
        self.AddParameter('MJD','MJD (0=do not modify)',0)
        self.AddParameter("GPU", 
                          "Graphics Processing Unit number (shoud default to environment if None)",
                          None)
        self.AddParameter("UseGPUs", "Use Graphics Processing Unit",False)
        self.AddParameter('ParticleType',
                        'type of particle that we are simulating','corsika')
        self.AddParameter('RunMPHitFilter',"Run polyplopia's mphitfilter",True)
        self.AddParameter("oversize","over-R: DOM radius oversize scaling factor",5)
        self.AddParameter("holeiceparametrization", "Turn on(True)/off(False) hole ice parametrization in clsim", True)
        self.AddParameter("maxparallelevents", "Number of frames to be processed in parallel", 100)
        self.AddParameter("totalenergytoprocess", "Accumulate frames according to amount of light deposited in the detector", 0)
        self.AddParameter("efficiency","overall DOM efficiency correction",[0.99])
        self.AddParameter("volumecyl","set volume to regular cylinder (set to False for 300m spacing from the DOMs)",True)
        self.AddParameter("IceModelLocation","Location of ice model param files", expandvars("$I3_BUILD/clsim/resources/ice")) 
        self.AddParameter("IceModel","ice model subdirectory", "SpiceMie") 
        self.AddParameter("MCTreeName","Name of MCTree frame object", "I3MCTree") 
        self.AddParameter("PhotonSeriesName","Photon Series Name","I3MCPESeriesMap") 
        self.AddParameter("RawPhotonSeriesName","Raw Photon Series Name",None) 
        self.AddParameter("UseGeant4","Enable Geant4 propagation",False) 
        self.AddParameter('HistogramFilename', 'Histogram filename.', None)

   def Execute(self,stats):
        if not ipmodule.ParsingModule.Execute(self,stats): return 0

        from icecube import icetray, dataclasses, dataio, phys_services, interfaces
        from I3Tray import I3Tray,I3Units

        # Instantiate a tray 
        tray = I3Tray()

        inputfiles  = self.GetParameter('inputfilelist')

        summary_in  = self.summaryfile
        summary_out = self.summaryfile
        if not os.path.exists(self.summaryfile):
           summary_in  = ''

        if self.gpu is not None and self.usegpus:
           os.putenv("CUDA_VISIBLE_DEVICES",str(self.gpu))
           os.putenv("COMPUTE",":0."+str(self.gpu))
           os.putenv("GPU_DEVICE_ORDINAL",str(self.gpu))

        # Configure IceTray services
        tray.AddService("I3XMLSummaryServiceFactory","summary",outputfilename=summary_out,inputfilename=summary_in)

        # RNG
        rngstate    = "rng.state"
        if not os.path.exists(rngstate): 
           rngstate = ''
           print("Warning: no RNG state found. Using seed instead.")

        tray.AddService("I3SPRNGRandomServiceFactory","sprngrandom",
            seed=self.seed, streamNum=self.procnum,nstreams=self.nproc,
            instatefile=rngstate,outstatefile="rng.state")


        # Configure IceTray modules 
        tray.AddModule("I3Reader", "reader",filenamelist=[self.gcdfile]+inputfiles)

        if type(self.efficiency) == list or type(self.efficiency) == tuple:
           if len(self.efficiency) == 1:
              efficiency=float(self.efficiency[0])
           elif len(self.efficiency) > 1:
              efficiency=map(float,self.efficiency)
           elif len(self.efficiency) > 1:
              raise Exception("Configured empty efficiency list")
        else:
            efficiency = choose_max_efficiency(self.efficiency)
         
        from .. import segments 

        tray.AddSegment(segments.PropagatePhotons, "normalpes",
            RandomService = "I3RandomService",
            HybridMode = False,
            IgnoreMuons = False,
            IgnoreCascades = False,
            UseCascadeExtension = False,
            UseGPUs = self.usegpus,
            UseAllCPUCores = False,
            IceModel         = self.icemodel,
            IceModelLocation = self.icemodellocation,
            UnshadowedFraction = efficiency,
            DOMOversizeFactor = self.oversize,
            InputMCTree = self.mctreename,
            UseGeant4 = self.usegeant4,
            OutputPESeriesMapName = self.photonseriesname,
            OutputPhotonSeriesName =  self.rawphotonseriesname,
            UseHoleIceParameterization = self.holeiceparametrization,
            MaxParallelEvents = self.maxparallelevents,
            TotalEnergyToProcess = self.totalenergytoprocess)

        if self.runmphitfilter:
            from icecube import polyplopia
            tray.AddModule("MPHitFilter","hitfilter",
                 HitOMThreshold=1,
                 RemoveBackgroundOnly=False,
                 I3MCPESeriesMapName=self.photonseriesname)

        if self.histogramfilename:         
            from icecube.production_histograms import ProductionHistogramModule
            from icecube.production_histograms.histogram_modules.simulation.mcpe_module import I3MCPEModule
        
            tray.AddModule(ProductionHistogramModule, 
                           Histograms = [I3MCPEModule],
                           OutputFilename = self.histogramfilename)

        tray.AddModule("I3Writer","writer", filename=self.outputfile,streams=[icetray.I3Frame.DAQ])
         
        tray.AddModule("TrashCan","trashcan")
         
        # Execute the Tray
        tray.Execute()
        tray.Finish()

        tray.PrintUsage()
        # Save stats
        for k in tray.Usage():
            stats[str(k.key())+":usr"] = k.data().usertime
            stats[str(k.key())+":sys"] = k.data().systime
            stats[str(k.key())+":ncall"] = k.data().systime

        del tray
        return 0


   def Finish(self,stats={}):
        self.logger.info("finish %s: %s" % (self.__class__.__name__,self.GetParameter("execute")))
        return 0



class HybridPhotons(ipmodule.ParsingModule):
   """
    GPU+Spline-based Photon propagation
   """
   def __init__(self):
        ipmodule.ParsingModule.__init__(self)

        self.AddParameter('Hybrid','Run in hybrid mode',True)
        self.AddParameter('gcdfile','GeoCalibDetStatus filename','')
        self.AddParameter('inputfilelist','list of input filenames',[])
        self.AddParameter('outputfile','Output filename','')
        self.AddParameter('seed','RNG seed',0)
        self.AddParameter('procnum','job number (RNG stream number)',0)
        self.AddParameter('nproc','Number of jobs (Number of RNG streams)',1)
        self.AddParameter('summaryfile','XMLSummary filename','$steering(summaryfile)')
        self.AddParameter('MJD','MJD (0=do not modify)',0)
        self.AddParameter("GPU", 
                          "Graphics Processing Unit number (shoud default to environment if None)",
                          None)
        self.AddParameter("UseGPUs", "Use Graphics Processing Unit",False)
        self.AddParameter('ParticleType',
                        'type of particle that we are simulating','corsika')
        self.AddParameter('RunMPHitFilter',"Run polyplopia's mphitfilter",True)
        self.AddParameter("oversize","over-R: DOM radius oversize scaling factor",5)
        self.AddParameter("efficiency","overall DOM efficiency correction",0.99)
        self.AddParameter("volumecyl","set volume to regular cylinder (set to False for 300m spacing from the DOMs)",True)
        self.AddParameter("IceModelLocation","Location of ice model param files", expandvars("$I3_BUILD/clsim/resources/ice")) 
        self.AddParameter("IceModel","ice model subdirectory", "SpiceMie") 
        self.AddParameter("PhotonSeriesName","Photon Series Name","I3MCPESeriesMap") 
        self.AddParameter("CscdSplineAmplitudeFits",
                        "Cacade spline amplitude .fits tables",
                        'ems_spice1_z20_a10.abs.fits')
        self.AddParameter("CscdSplineTimingFits",
                        "Cacade spline timing .fits tables",
                        'ems_spice1_z20_a10.prob.fits')
        self.AddParameter("usecvmfs","Use CVMFS for spline tables (if possible)",
                          '/cvmfs/icecube.opensciencegrid.org')
        self.AddParameter("ConvertToMCHits", "Convert MCPEs to MCHits for backwards compatibility", False)

   def Execute(self,stats):
        if not ipmodule.ParsingModule.Execute(self,stats): return 0
        from ..util.fileutils import download,untar,isurl
        from icecube import icetray, dataclasses, dataio, phys_services, interfaces, sim_services
        #from .clsim import HybridClSimSpline
        from I3Tray import I3Tray, I3Units

        # Instantiate a tray 
        tray = I3Tray()

        gcdfile     = self.GetParameter('gcdfile')
        inputfiles  = self.GetParameter('inputfilelist')
        outputfile  = self.GetParameter('outputfile')

        summary_in  = self.summaryfile
        summary_out = self.summaryfile
        if not os.path.exists(self.summaryfile):
           summary_in  = ''

        # Configure IceTray services
        tray.AddService("I3XMLSummaryServiceFactory","summary", outputfilename=summary_out,inputfilename=summary_in)

        # RNG
        rngstate    = "rng.state"
        if not os.path.exists(rngstate): 
           rngstate = ''
           print("Warning: no RNG state found. Using seed instead.")

        tray.AddService("I3SPRNGRandomServiceFactory","sprngrandom",
            seed=self.seed, streamNum=self.procnum,nstreams=self.nproc,
            instatefile=rngstate,outstatefile="rng.state")

        # Configure IceTray modules 
        tray.AddModule("I3Reader", "reader",filenamelist=[gcdfile]+inputfiles)
        
        #if self.gpu is not None and self.usegpus:
        #   os.putenv("CUDA_VISIBLE_DEVICES",str(self.gpu))
        #   os.putenv("COMPUTE",":0."+str(self.gpu))
        #   os.putenv("GPU_DEVICE_ORDINAL",str(self.gpu))

        # simulate cascades (with photonics-hit-maker)
        cascade_service = LoadCascadeTables(
            Use_cvmfs=self.usecvmfs, 
            amplitude_fits = self.cscdsplineamplitudefits, 
            timing_fits = self.cscdsplinetimingfits)

        from .. import segments
        tray.AddSegment(segments.PropagatePhotons, "hybridpes",
            RandomService = "I3RandomService",
            HybridMode = True,
            UseGPUs = False,
            UseAllCPUCores = False,
            IceModel = self.icemodel,
            IceModelLocation = self.icemodellocation,
            CascadeService = cascade_service,
            UseCascadeExtension = False,
            UnshadowedFraction = self.efficiency,
            DOMOversizeFactor = self.oversize,
            OutputPESeriesMapName = self.photonseriesname)


        if self.runmphitfilter:
           from icecube import polyplopia
           tray.AddModule("MPHitFilter","hitfilter",
                 HitOMThreshold=1,
                 RemoveBackgroundOnly=False,
                 I3MCPESeriesMapName=self.photonseriesname)

        # Convert for backwards compatibility
        if self.GetParameter("ConvertToMCHits"):
           tray.AddModule("I3MCPEtoI3MCHitConverter","converthits", 
                          InputResponse=self.photonseriesname,
                          OutputResponse = "MCHitSeriesMap")
           tray.AddModule( DrivingTime, "dt", Streams = [icetray.I3Frame.DAQ] )

        tray.AddModule("I3Writer","writer", filename=outputfile,streams=[icetray.I3Frame.DAQ])
         
        tray.AddModule("TrashCan","trashcan")
         
        # Execute the Tray
        tray.Execute()
        tray.Finish()

        del tray
        return 0


   def Finish(self,stats={}):
        self.logger.info("finish %s: %s" % (self.__class__.__name__,self.GetParameter("execute")))
        return 0


