from icecube import icetray, dataio, phys_services, diplopia
from optparse import OptionParser

from I3Tray import *

usage = "usage: %prog [options]"
parser = OptionParser(usage)
parser.add_option("-b", "--backgroundfile", dest="BACKGROUNDFILE", help="Read background CR from file (.i3{.gz} format)")
parser.add_option("-i", "--inputfile", dest="SIGNALFILE", help="Read signal From file (.i3{.gz} format)")
parser.add_option("-o", "--outfile",default="merged.i3", dest="OUTFILE", help="Write output to OUTFILE (.i3{.gz} format)")
parser.add_option("--seed",type="int",default=12344, dest="SEED", help="Initial seed for the random number generator")

(options,args) = parser.parse_args()
if len(args) != 0:
        crap = "Got undefined options:"
        for a in args:
                crap += a
                crap += " "
        parser.error(crap)

if not options.SIGNALFILE:
        parser.error("Need to specify a signal file")
if not options.BACKGROUNDFILE:
        parser.error("Need to specify a background file")


tray = I3Tray()

rng = phys_services.I3GSLRandomService(options.SEED)
tray.context["I3RandomService"] = rng

background = diplopia.CoincidentI3ReaderService()
background.open(options.BACKGROUNDFILE)

tray.AddModule("I3Reader","reader",
    FilenameList=[options.SIGNALFILE]
    )

tray.AddModule("PoissonMerger","merge",
       BGWeights = "CorsikaWeightMap",
       CoincidentEventService = background,
)
tray.AddModule("MPHitFilter","prune")

tray.AddModule("I3Writer","write",Filename=options.OUTFILE)

tray.Execute()
tray.Finish()

