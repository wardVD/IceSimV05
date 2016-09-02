"""
 * class: ModifyTime
 * (c) 2014 IceCube Collaboration
 * author: Juan Carlos Diaz Velez (juancarlos@icecube.wisc.edu)
 * IceProd classes for modifiying event frames
 * 
"""
######################################################################
import os
import sys,getopt
from os.path import expandvars

from icecube import icetray,dataclasses
from .. import ipmodule

class ModifyEventTime(icetray.I3Module):
   """
   Modify the time in the I3EventHeader
   """
   def __init__(self,ctx) : 
       icetray.I3Module.__init__(self,ctx)

       self.AddParameter("MJD","Modified Julian Date",55697);
       self.AddParameter("MJDSeconds","Number of seconds after the start of the MJD.",0);
       self.AddParameter("MJDNanoSeconds","Number of nanoseconds after the start of the MJD.",0);
       self.AddParameter("Randomize","Randomize mjd within year starting with MJD",False);
       self.AddParameter("RNG","(pseudo-) random number generator",None);
       self.AddOutBox("OutBox");

   def Configure(self):
       mjd     = self.GetParameter("MJD")
       sec     = self.GetParameter("MJDSeconds")
       nanosec = self.GetParameter("MJDNanoSeconds")
       self.rand = self.GetParameter("Randomize") 
       self.rng  = self.GetParameter("RNG") 
       self.time  = dataclasses.I3Time()
       self.time.set_mod_julian_time(mjd,sec,nanosec)

   def DAQ(self,frame):
       if self.rand and self.rng is not None:
          mjdStart = self.GetParameter("MJD")
          mjdEnd = self.GetParameter("MJD") + 365
          mjd = self.rng.uniform(0.,1.)*(mjdEnd-mjdStart)
          sec = self.rng.uniform(0.,1.)*24*3600
          self.time.set_mod_julian_time(mjd,sec,0)

       if "I3EventHeader" in frame:
          eventheader = frame["I3EventHeader"]
          eventheader.start_time = self.time
          del frame["I3EventHeader"]
          frame["I3EventHeader"] = eventheader
       else:
          print("No I3EventHeader to modify.")

       # some GCDs come with driving times and some don't
       if "DrivingTime" in frame :
          del frame["DrivingTime"]
       frame.Put("DrivingTime", self.time )
       
       self.PushFrame(frame,"OutBox")

 

class DateShifter(ipmodule.ParsingModule):

   def __init__(self):
        ipmodule.ParsingModule.__init__(self)

        self.AddParameter('inputfile','Output filename','$steering(prev_file)')
        self.AddParameter('outputfile','Output filename','$steering(current_file)')
        self.AddParameter('summaryfile','XMLSummary filename','')
        self.AddParameter("MJD","Modified Julian Date",55697)
        self.AddParameter("MJDSeconds","Time added in seconds after mjd",0)
        self.AddParameter("MJDNanoSeconds","Time added in seconds after mjd",0)

   def Execute(self,stats):
        if not ipmodule.IPBaseClass.Execute(self,stats): return 0

        import icecube.icetray
        from I3Tray import I3Tray
        from icecube import dataio,dataclasses,phys_services

        inputfile = self.GetParameter('inputfile')
        outputfile = self.GetParameter('outputfile')

        summary     = self.GetParameter('summaryfile')
        summary_in  = summary
        summary_out = summary
        if not os.path.exists(summary): summary_in = ''

        # Instantiate a tray 
        tray = I3Tray()

        # Configure IceTray services
        if summary_out:
           tray.AddService("I3XMLSummaryServiceFactory","summary",
            outputfilename=summary_out, inputfilename=summary_in)
         
        # Configure IceTray modules 
        tray.AddModule("I3Reader","reader",filenamelist=[inputfile])
        tray.AddModule(ModifyEventTime,"mod_mjd", 
             MJD=self.GetParameter('mjd'),
             MJDSeconds=self.GetParameter('mjdseconds'),
             MJDNanoSeconds=self.GetParameter('mjdnanoseconds'))
        tray.AddModule("I3Writer","writer", filename=outputfile)
        tray.AddModule("TrashCan","recycle")

        # Execute the Tray
        tray.Execute()
        tray.Finish()

        # Free memory
        del tray
        return 0

