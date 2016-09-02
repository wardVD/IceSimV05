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

from I3Tray import *
from .. import ipmodule


def PPCTraySegment(tray,name,
        efficiency, oversize,
        gpulib,
        volumecyl,
        IceModelLocation=expandvars("$I3_BUILD/ppc/resources/ice"),
        keep_empty_events=False,
        IceModel="SpiceMie",
        InputMCTree="I3MCTree",
        mcpeseries = "I3MCPESeriesMap",
        UseGPUs=True,
        GPU=-1):

        """
        PPC Photon Propagation Code TraySegment (supports CUDA/OpenCL) 
        """

        # Do one or the other
        UseCPU = not UseGPUs

        ppcIceModel = None
        if IceModelLocation is None:
             IceModelLocation = expandvars("$I3_SRC/ppc/resources/ice")
        if IceModel == "Spice1":
             ppcIceModel = expandvars(IceModelLocation+"/sp1")
        elif IceModel == "SpiceMie":
             ppcIceModel = expandvars(IceModelLocation+"/mie")
        elif IceModel == "SpiceLea":
             ppcIceModel = expandvars(IceModelLocation+"/lea")
        elif IceModel == "Spice3":
             ppcIceModel = expandvars(IceModelLocation+"/sp3")
        elif IceModel == "SpiceX":
             ppcIceModel = expandvars(IceModelLocation+"/spx")
        else:
             raise RuntimeError("Unknown ice model: %s", IceModel)



        ppc_tables_dir_cfg = os.path.join(ppcIceModel,'cfg.txt')

        x_ppc_tables_dir = os.path.join(os.getcwd(),'xppc_ice'+os.path.basename(os.tmpnam()))
        x_ppc_tables_dir_cfg = os.path.join(x_ppc_tables_dir,'cfg.txt')


        print('cp -r -L %s %s' % ( ppcIceModel, x_ppc_tables_dir ))
        if os.system('cp -r -L %s %s' % ( ppcIceModel, x_ppc_tables_dir )):
           raise Exception, "XPPC cannot copy ice model parameters"

        ppc_cfg_in  = open(ppc_tables_dir_cfg,'r')
        ppc_cfg_out = open(x_ppc_tables_dir_cfg,'w')
        for line in ppc_cfg_in.readlines():
            toks = map(string.strip,line.split('#'))
            if len(toks) > 1:
               if toks[1].startswith("over-R:"):
                  toks[0] = str(oversize)
               if toks[1].startswith("overall DOM efficiency"):
                  toks[0] = str(efficiency)
            print("  #".join(toks))
            ppc_cfg_out.write("  #".join(toks)+'\n')
        ppc_cfg_in.close()
        ppc_cfg_out.close()
        
        os.putenv("PPCTABLESDIR",x_ppc_tables_dir)
        if UseGPUs:
            os.putenv("OGPU","1")
        if UseCPU:
            os.putenv("OCPU","1")
        if GPU >= 0 and UseGPUs:
            os.putenv("CUDA_VISIBLE_DEVICES",str(GPU))
            os.putenv("COMPUTE",":0."+str(GPU))
            os.putenv("GPU_DEVICE_ORDINAL",str(GPU))

        # Load libraries 
        from icecube import icetray, interfaces ,dataclasses ,simclasses,sim_services
        from icecube import polyplopia
        if gpulib == 'cuda':
           load("libcudart")
        elif gpulib == 'opencl':
           load("libOpenCL")
#           os.putenv("XMLT","12") # make 12 blocks
        load("xppc")
        load("ppc")
        #load("ppc_eff")

        tray.AddModule("i3ppc", "ppc", 
                  If = lambda f: f[InputMCTree].size() or keep_empty_events,
                  gpu=GPU, 
                  cyl=volumecyl,
                  keep=keep_empty_events,
                  MCTree=InputMCTree)

        if mcpeseries != "MCPESeriesMap":
           tray.AddModule("Rename","rename", keys=["MCPESeriesMap","BackgroundI3MCPESeriesMap"])

###### IP Modules ###########

class PPC(ipmodule.ParsingModule):
   """
    GPU Photon propagation
   """
   def __init__(self):
        ipmodule.ParsingModule.__init__(self)
        self.configured = False

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
                          -1)
        self.AddParameter("UseGPUs", "Use Graphics Processing Unit",False)
        self.AddParameter('ParticleType',
                        'type of particle that we are simulating','corsika')
        self.AddParameter('RunMPHitFilter',"Run polyplopia's mphitfilter",True)
        self.AddParameter("oversize","over-R: DOM radius oversize scaling factor",5)
        self.AddParameter("efficiency","overall DOM efficiency correction",0.99)
        self.AddParameter("gpulib","set gpu library to load (defaults to cuda)","opencl")
        self.AddParameter("volumecyl","set volume to regular cylinder (set to False for 300m spacing from the DOMs)",True)
        self.AddParameter("PhotonSeriesName","Photon Series Name","I3MCPESeriesMap") 
        self.AddParameter("IceModelLocation","Location of ice model param files", expandvars("$I3_BUILD/ppc/resources/ice")) 
        self.AddParameter("IceModel","ice model subdirectory", "SpiceMie") 
        self.AddParameter("MCTreeName","Name of MCTree frame object", "I3MCTree") 
        self.AddParameter("KeepEmptyEvents","Don't discard events with no MCPEs", False) 
        self.AddParameter('HistogramFilename', 'Histogram filename.', None)
        self.configured = False

   def Execute(self,stats):
        if not ipmodule.ParsingModule.Execute(self,stats): return 0
        from icecube import icetray, dataclasses, dataio, phys_services, interfaces
        from .ppc import PPCTraySegment
        from I3Tray import I3Tray,I3Units

        # Instantiate a tray 
        tray = I3Tray()


        summary_in  = self.summaryfile
        summary_out = self.summaryfile
        if not os.path.exists(self.summaryfile):
           summary_in  = ''

        # Configure IceTray services
        #tray.AddService("I3XMLSummaryServiceFactory","summary", outputfilename=summary_out,inputfilename=summary_in)

        # RNG
        rngstate    = "rng.state"
        if not os.path.exists(rngstate): 
           rngstate = ''
           print("Warning: no RNG state found. Using seed instead.")

        tray.AddService("I3SPRNGRandomServiceFactory","sprngrandom",
            seed=self.seed, streamNum=self.procnum,nstreams=self.nproc,
            instatefile=rngstate,outstatefile="rng.state")

        # Configure IceTray modules 
        tray.AddModule("I3Reader", "reader",filenamelist=[self.gcdfile]+self.inputfilelist)
         
        tray.AddSegment(PPCTraySegment,"ppc_photons",
			gpu = self.gpu,
			usegpus = self.usegpus,
			efficiency = self.efficiency,
			oversize = self.oversize,
			IceModelLocation = self.icemodellocation,
			IceModel = self.icemodel,
			volumecyl = self.volumecyl,
			gpulib = self.gpulib,
			InputMCTree = self.mctreename,
			keep_empty_events = self.keepemptyevents,
			mcpeseries = self.photonseriesname)

        if self.photonseriesname != "MCPESeriesMap":
           tray.AddModule("Rename","rename", keys=["MCPESeriesMap",self.photonseriesname])

        if self.runmphitfilter:
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

        del tray
        return 0


   def Finish(self,stats={}):
        self.logger.info("finish %s: %s" % (self.__class__.__name__,self.GetParameter("execute")))
        return 0




