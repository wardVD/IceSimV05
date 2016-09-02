from icecube import icetray, dataio, phys_services, diplopia, sim_services
from icecube.trigger_sim.modules.time_shifter import I3TimeShifter
from icecube.simprod.util import DAQCounter
from icecube import vuvuzela
from optparse import OptionParser

usage = "usage: %prog [options]"
parser = OptionParser(usage)
parser.add_option("-i", "--inputfile", dest="INPUTFILE", help="Read signal From file (.i3{.gz} format)")
parser.add_option("-o", "--outfile",default="merged.i3", dest="OUTFILE", help="Write output to OUTFILE (.i3{.gz} format)")
parser.add_option("-g", "--gcdfile", dest="GCDFILE", help="Read GCD input (.i3{.gz} format)")
parser.add_option("-r", "--rate", default=5000.0, type="float", dest="RATE", help="Event rate")
parser.add_option("--seed",type="int",default=12344, dest="SEED", help="Initial seed for the random number generator")

(options,args) = parser.parse_args()
if len(args) != 0:
        crap = "Got undefined options:"
        for a in args:
                crap += a
                crap += " "
        parser.error(crap)

if not options.INPUTFILE:
        parser.error("Need to specify a signal file")




from I3Tray import *

tray = I3Tray()

rng = phys_services.I3GSLRandomService(options.SEED)
tray.context["I3RandomService"] = rng

tray.AddService("I3XMLSummaryServiceFactory", "summary",
       inputfilename="summary.xml",
       outputfilename="10s_summary.xml"
)

tray.AddModule("I3Reader","reader",
    FilenameList=[options.GCDFILE,options.INPUTFILE]
    )

#tray.AddModule("MPHitFilter","filter", HitSeriesMapName="I3MCPESeriesMap",HitOMThreshold=20)
tray.AddModule("I3PolyplopiaExp","merge",
       Rate=options.RATE*I3Units.hertz,
       TimeWindow=1.0*I3Units.ms
)

"""
tray.AddSegment(vuvuzela.AddNoise, "vuvuzela",
      OutputName = "MCPEsWnoise",
      InputName = "I3MCPESeriesMap",
      StartTime = -10.*I3Units.microsecond,
      EndTime = 10.*I3Units.microsecond,
      RandomServiceName = rng,)
"""


tray.AddModule(I3TimeShifter,"shift")

tray.AddModule("Dump","dump2")
tray.AddModule("I3Writer","write",Filename=options.OUTFILE, streams=[icetray.I3Frame.DAQ])

#tray.AddModule(DAQCounter,"count", NEvents=10)

tray.Execute()
tray.Finish()

