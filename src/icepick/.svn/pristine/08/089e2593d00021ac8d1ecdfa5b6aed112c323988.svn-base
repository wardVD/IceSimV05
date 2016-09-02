#!/usr/bin/env python
import sys
from optparse import OptionParser
parser = OptionParser()
parser.add_option("-i", "--infile", dest="FILENAME",
                  help="Input I3File")

parser.add_option("-p", "--recopulsename", dest="RPNAME",
                  help="Frame key of I3RecoPulseSeriesMap",
                  default="SRTOfflinePulses")

(options, args) = parser.parse_args()

if not options.FILENAME :
    print("You must supply a filename.")
    sys.exit(1)
 
from icecube import icetray, dataclasses, dataio, icepick
from I3Tray import I3Tray

class CondModule( icetray.I3ConditionalModule ):
    def __init__(self, context):
        icetray.I3ConditionalModule.__init__(self, context)
        self.AddParameter("PrintMe","", "default") 
        self.AddOutBox("OutBox")

    def Configure(self):        
        self.msg = self.GetParameter("PrintMe") 

    def DAQ(self, frame):
        print(self.msg)
        self.PushFrame(frame)        

def condition(frame):
    print("just stopped by to see what condition my condition was in")
    return True

def condition2(frame):
    print("NO SOUP FOR YOU!")
    return False

@icetray.traysegment
def TraySegment0(tray, name, If = lambda f : True ):
    tray.AddModule(CondModule, "cond_" + name, If = If, PrintMe = "cond_"+name )

@icetray.traysegment
def TraySegment1(tray, name, If = lambda f : True ):
    def local_condition( frame ):
        print("staying local")
        return True
    tray.AddModule(CondModule, "cond_" + name, If = If and local_condition, \
                       PrintMe = "cond_"+name )

tray = I3Tray()

tray.AddModule("I3Reader", "read", filename = options.FILENAME )

# simple old-style condition
tray.AddModule( CondModule, "cond", If = condition, PrintMe = "cond" )

# combine old-style conditions and pypicks
tray.AddSegment( TraySegment0,"ts0", If = icetray.pypick(condition) & \
                   icepick.I3NChFilter(options.RPNAME, NCh = 1) )

tray.AddModule( icepick.I3NChFilter(options.RPNAME, NCh = 2), "pick" )

tray.AddSegment( TraySegment0,"ts_pypick", \
                   If = icepick.I3NChFilter(options.RPNAME, NCh = 10000) & \
                   icepick.I3ChargeFilter(options.RPNAME, Charge = 1000000) )

tray.AddSegment( TraySegment0,"ts_AND", \
                   If = condition and condition2 )

tray.AddSegment( TraySegment0,"ts_OR", \
                   If = condition or condition2 )

tray.AddSegment( TraySegment1,"ts1_OR", \
                   If = condition or condition2 )

tray.AddSegment( TraySegment1,"ts1_pypick", \
                   If = icepick.I3NChFilter(options.RPNAME, NCh = 10) )

tray.AddModule("Dump","dump")

tray.AddModule("TrashCan", "garbage")

tray.Execute(10)

