#!/usr/bin/env python
import os,sys
from os.path import expandvars

from I3Tray import *
from icecube import icetray, dataclasses, dataio, trigger_sim
from .. import ipmodule

class SlowMonopoles(ipmodule.IPBaseClass):
   """
   Wrapper class that runs MonoPole generator and propagator
   """

   def __init__(self):
        ipmodule.IPBaseClass.__init__(self)

        self.i3ver = 3
        self.AddParameter('gcdfile','GeoCalibDetStatus filename','$steering(gcdfile_09)')
        self.AddParameter('outputfile','Output filename','$steering(current_file)')
        self.AddParameter('summaryfile','XMLSummary filename','$steering(summaryfile)')
        self.AddParameter('mjd','MJD for the GCD file','$steering(mjd_09)')
        self.AddParameter('Beta','Beta parameter for calculating lorentz factor Gamma',1e-3)
        self.AddParameter('FreeMeanPath','Free mean path',1)
        self.AddParameter('NEVENTS','Number of events to generate',100)

   def Execute(self,stats):
        from icecube import BadDomList

        from icecube.BadDomList import bad_dom_list_static

        from icecube.trigger_sim.Monopole_splitter import MonopoleEventSplitter

        from os.path import expandvars

        import os
        import sys
        import math
        import time

        gcdfile    = self.GetParameter('gcdfile')
        gcdfile    = self.parser.parse(gcdfile) #in case we are using default

        outputfile = self.GetParameter('outputfile')
        outputfile = self.parser.parse(outputfile)

        summary    = self.GetParameter('summaryfile')
        summary    = self.parser.parse(summary)

        mjd        = self.GetParameter('mjd')
        mjd        = int(self.parser.parse(str(mjd)))

        self.i3ver = self.GetParameter('i3ver')
        beta       = self.GetParameter('Beta')
        freepath   = self.GetParameter('FreeMeanPath')
        nevents    = self.GetParameter('NEVENTS')
        nevents    = int(self.parser.parse(nevents))


        load("libicetray")
        load("libdataclasses")
        load("libphys-services")
        load("libdataio")
        load("libpmt-simulator")
        load("libphotonics-service")
        load("libhit-maker")
        load("libDOMsimulator")
        load("libDOMcalibrator")
        load("libsim-services")
        load("libtrigger-sim")
        load("libmonopole-generator")
        load("libmonopole-propagator")
        load("libtrigger-sim")
        load("DomTools")


        tray = I3Tray()

        ## variables for geometry selector

        ic22 = "21,29,30,38,39,40,49,50,59,58,67,66,74,73,65,72,78,48,57,47,46,56"
        ic2008= "63,64,55,71,70,76,77,75,69,60,68,61,62,52,44,53,54,45"
        ic2009 = "2,3,4,5,6,10,11,12,13,17,18,19,20,26,27,28,36,37,83"
        ic2010 ="8,9,16,25,35,85,84,82,81,86,43,34,24,15,23,33,42,51"
        stationsToExclude = "1:80"


        tray.AddService("I3PhotonicsServiceFactory","photonics")(
            ("TablesDirectory",tablesdir),
            ("DriverFileDirectory",driverdir),
            ("PhotonicsLevel1DriverFile",level1Driver),
            ("PhotonicsLevel2DriverFile",""),#don't need level2
            ("PhotonicsTableSelection",1)
            )


        tray.AddService("I3SPRNGRandomServiceFactory","random")(
            ("NStreams",int(self.parser.parse("$args(nproc)"))),
            ("Seed",int(self.parser.parse("$steering(seed)"))),
            ("StreamNum",int(self.parser.parse("$args(procnum)"))),
            ("outstatefile","rng.state"),
        )
         
                                                                                    
        tray.AddService("I3MCTimeGeneratorServiceFactory","time-gen")(
                ("Mjd",mjd),
                ("RunNumber",int(self.parser.parse("$eval($args(dataset)*100000 + $args(procnum))"))),
                ('IncrementEventID', True)
                )
                                                                                                               

        tray.AddService("I3ReaderServiceFactory","reader")(
            ("omitevent",True),
            ("filename",gcdfile),
        )

        tray.AddService("I3GeometrySelectorServiceFactory","geo-selector")(
                ("StringsToUse",ic22 +","+ ic2008 +","+ic2009),
                ("StationsToExclude",stationsToExclude),
                ("GeoSelectorName","IC59-Geo")
                )
                                                                                                          
        tray.AddModule("I3Muxer","muxer")(
                  ("GeometryService","IC59-Geo")
                  )

        tray.AddModule("I3EventCounter","counter3")(
            ("nevents",nevents),
            ("physicscountername","Generated Events"),
        )
                                                                                                          
        GAMMA = 1.0/math.sqrt(1-beta**2)                                                                  
        tray.AddModule("I3MonopoleGenerator","mp-gen")(                                                   
            ("Mass",1e15*I3Units.GeV),                                                                    
            ("Gamma",GAMMA),                                                                              
            ("Disk_rad", 300*I3Units.m),                                                                  
            ("Disk_dist", 500*I3Units.m),
            ("Rand_dir",True)                                                                             
            )                   

        tray.AddModule("I3MonopolePropagator","mp-prop")(
           ("InputTreeName","I3MCTree"),
           ("OutputTreeName","I3MCTree"),
           ("CalculateEnergy",True),
           ("CalculateDensityCorrection",True),
           ("StepSize",NaN),
           ("IncludeErrorChecking",True),
           ("SpeedMin", 1e-5*dataclasses.I3Constants.c),
           ("MaxLength",10.0*I3Units.m),
           ("MinLength",0.1*I3Units.cm),
           ("MaxDistanceFromCenter",800*I3Units.m)
           )
   

        tray.AddModule("I3MonopoleSecondaries","mp-sec")(                                                 
            ("MeanFreePath", freepath * I3Units.m),                                                        
            ('ScaleEnergy', False)
            ) 
                      
        streams = ["Physics"]
        if i3ver >= 3: 
            streams = [icecube.icetray.I3Frame.Physics]

        tray.AddModule("I3Writer","writer")(
            ("filename",outputfile),
            ("streams",streams),
        )
         
        tray.AddModule("TrashCan", "the can")                                                             
        tray.Execute()                                                                         
        tray.Finish()
        del tray
        return 0


class MonopolePolyplopia(ipmodule.IPBaseClass):

   """
   Merge monopole signal with corsika bg.
   """
   def __init__(self):
        ipmodule.IPBaseClass.__init__(self)

        self.i3ver = 3

        from icecube import icetray, dataclasses, dataio, phys_services
        from icecube.phys_services import I3GSLRandomService as random
        import math

        self.AddParameter("corsika_source",
                          "Directory which contains the CORSIKA i3files.",
                          "/data/icecube01/users/olivas/slow_mp/corsika-trees/corsika.i3.gz")
        self.AddParameter("monopole_source",
                          "Directory which contains the Monopole i3files.",
                          "/data/icecube01/users/olivas/slow_mp/slow-beta-trees/beta_2.000_m_14.000.i3.gz")
        self.AddParameter("output_file",
                          "Output file",
                          "/data/icecube01/users/olivas/slow_mp/mixed_beta_2.000_m_14.000.i3.gz")
        self.AddParameter("nevents", "Number of Monopole events", default=100)
        self.AddParameter("cosmic_ray_rate", "Cosmic ray rate", 600.0)

   def Execute(self,stats):
        corsika_source  = self.GetParameter("corsika_source")
        monopole_source = self.GetParameter("monopole_source")
        output_file     = self.GetParameter("output_file")
        nevents         = self.GetParameter("nevents")
        cosmic_ray_rate = self.GetParameter("cosmic_ray_rate")

#cosmic_ray_rate *= I3Units.hertz # this should be done at config level

        mpfile = dataio.I3File(monopole_source)
        crfile = dataio.I3File(corsika_source)

        outfile = dataio.I3File(output_file)

        I3Constants_c = 2.99792458e8 * I3Units.m / I3Units.second

        counter = 0
        first_frame = True
        while mpfile.more() and crfile.more() :

            frame = mpfile.pop_physics()

            if frame.Has("I3MCTree"):
                mctree = frame.Get("I3MCTree")
                if len(mctree.GetPrimaries()) != 1 :
                    print("Multiple Monopole Primaries.  FIX THIS!!")
                    continue
            else:
                print("No tree found.  Skipping...")
                continue

            ##find number of CORSIKA primaries
            mp = mctree.GetPrimaries()[0]
            time_window = sum([float(prim.GetLength()/prim.GetSpeed()) for prim in mctree if str(part.GetType())=='Monopole'])
            nprimaries = int(random.Poisson(float(cosmic_ray_rate) * (time_window/I3Units.second) ) )
            if not nprimaries : nprimaries = 1

            if not counter % 1000 : print("read %d frames" % counter)
            counter += 1

            for i in range(nprimaries):

                crframe = crfile.pop_physics()
                crmctree = crframe.Get("I3MCTree")
         
                delta_t = random.Uniform(mp.GetTime(), mp.GetTime() + time_window)
                delta_t -= crmctree.GetPrimaries()[0].GetTime();
                crmctree.TimeShift(delta_t);
                
                mctree.Merge(crmctree)

            del frame["I3MCTree"]
            frame["I3MCTree"] = mctree

            if first_frame :
                first_frame = False
                beta = mp.GetSpeed()/I3Constants_c
                mass_MP = frame.Get("MP_Mass").value
                fn = "beta_%3.3f_m_%3.3f.i3" % (-math.log10(beta),math.log10(mass_MP))
                print(fn)
               # outfile.open_file(ofpath + fn,outfile.Mode.Writing)

            outfile.push(frame)
