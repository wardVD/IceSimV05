#!/usr/bin/env python
"""
Script for running IceTop's tank response.
"""
import sys
from optparse import OptionParser, IndentedHelpFormatter
from icecube.topsimulator.segments import SimIceTop

parser = OptionParser(formatter=IndentedHelpFormatter(indent_increment=4, max_help_position=60))
parser.add_option("-o", "--output", action="store", dest="output",
                  help="Output file name (with extension .i3, .i3.gz, .root, or .h5)", metavar="FILE")
parser.add_option("-s", action="store", type="int", dest="samples",
                  help="Number of times each shower is sampled. Default: 1", metavar="S", default=1)
parser.add_option("-n", action="store", type="int", dest="n_events",
                  help="Number of iterations. Default: unbounded", metavar="N")
parser.add_option("-x", action="store", type="float", dest="x",
                  help="The shower's x-coordinate. Default: 0", metavar="X", default=0)
parser.add_option("-y", action="store", type="float", dest="y",
                  help="The shower's y-coordinate. Default: 0", metavar="Y", default=0)
parser.add_option("-r", action="store", type="float", dest="r",
                  help="Radius of a circular sampling region for the core location around (x,y). Default: 0", metavar="R", default=0)
parser.add_option("-w", action="store_true", dest="weighted",
                  help="Use weighted sampling algorithm (experimental)", default=False)
parser.add_option("--seed", action="store", type="int", dest="seed",
                  help="Seed for random number generator. Default: 0", default=0)
parser.add_option("--new", action="store_true", dest="new",
                  help="Use new simulation (DOMLauncher). Default.", default=True)
parser.add_option("--old", action="store_false", dest="new",
                  help="Use old simulation (I3PMTSimulator)")
parser.add_option("--gcd", action="store", dest="gcd", help="GCD file",
                  metavar="gcd_file")
parser.add_option("--response", action="store", dest="response",
                  help="Tank response to use. Default: g4", metavar="[param|g4]", default="g4")
parser.add_option("--injector", action="store", dest="injector",
                  help="Injector to use. Default: normal", metavar="[normal|unthin]", default="normal")
parser.add_option("--rng", action="store", dest="rng",
                  help="Random number generator to use: Default: gsl, metavar=[gsl|sprng|root]",
                  default="gsl")
parser.add_option("--debug", action="store_const", dest="log_level",
                  help="Debug log-level", const=1, metavar="N", default=2)
parser.add_option("--trace", action="store_const", dest="log_level",
                  help="Trace log-level", const=0, metavar="N", default=2)
parser.add_option("--trigger", action="store_true", dest="trigger",
                  help="Simulate trigger", default=False)
parser.add_option("--tanks", action="append", dest="tanks",
                  help="Tanks to simulate", default=[])  # tank '36A' is in the center of the array
parser.add_option("--calibrate", action="store_true", dest="calibrate",
                  help="Computes calibrated signals and waveforms", default=False)
parser.add_option("--pecompress", action="store", type="int", dest="pecompress",
                  help="Compression level for PEs per nanosecond. 0 = keep complete source info, 1 = keep source class info, 2 = drop source info.",
                  default=2)

parser.set_usage('%s -o <filename> --gcd <gcd_file> [ -n <N> ] [ [-x <X>] [-y <Y>] [-r <R>] | -w] <corsika files>'%sys.argv[0])

(options, infiles) = parser.parse_args()

n_events = options.n_events
options = vars(options)
del options["n_events"]  # not understood by SimIceTop

if len(infiles) == 0:
    print 'Error: no input files provided'
    parser.print_help()
    sys.exit(1)

# Instantiate and execute a tray
from I3Tray import I3Tray
tray = I3Tray()
tray.AddSegment(SimIceTop, 'sim_icetop', input=infiles, **options)
tray.AddModule("TrashCan", "trashcan")
if n_events is None:
    tray.Execute()
else:
    tray.Execute(n_events)
tray.Finish()
