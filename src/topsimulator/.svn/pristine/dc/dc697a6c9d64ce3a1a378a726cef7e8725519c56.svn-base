#!/usr/bin/env python
"""
Script for testing IceTop's injectors.
"""

import sys
from optparse import OptionParser, IndentedHelpFormatter
from icecube import icetray

@icetray.traysegment
def SimIceTop(tray, name,
              input,
              output=None,
              gcd=None,
              log_level=2,
              samples=1,
              x=0.0,
              y=0.0,
              r=0.0,
              seed=0,
              injector="normal",
              response="g4",
              weighted=False):
    import os
    import re
    from icecube import icetray, dataclasses, dataio
    from icecube import simclasses, sim_services, phys_services
    from icecube import topsimulator
    from icecube.icetray import I3Units

    if gcd is None:
        raise ValueError("Need to specify a GCD file")
    if not os.path.exists(gcd):
        raise IOError("Specified GCD file does not exist")

    for unit in ('I3TopSimulator', 'I3CorsikaReader', 'I3CorsikaInjector',
                 'I3CorsikaThinnedInjector', 'I3CorsikaGridInjector',
                 'I3G4TankResponse', 'I3ParamTankResponse'):
        icetray.set_log_level_for_unit(unit, icetray.I3LogLevel(log_level))

    # to be used by injector
    for tag in ("inj", "resp", "other"):
        tray.AddService("I3GSLRandomServiceFactory", "gslrandom_" + tag,
                        Seed=seed,
                        InstallServiceAs='random_' + tag)

    if weighted:
        if injector == "normal":
            tray.AddService("I3InjectorFactory<I3CorsikaInjector>", "particle-injector",
                            FileNameList = input,
                            RandomServiceName = 'random_inj',
                            NumSamples = samples,     # <-- Number of re-samples of the same shower
                            ImportanceSampling = True,
                            #PartDistr = os.path.join(os.path.dirname(options.output),
                            #                         'on_regions_' + os.path.basename(options.output).replace(extension, 'root')),
                            IgnoreParticleTypes = [75, 76, 85, 86, 95, 96]
                            )
        else:
            raise ValueError("option weighted requires normal injector")
    else:
        if injector == "normal":
            tray.AddService("I3InjectorFactory<I3CorsikaInjector>", "particle-injector",
                            FileNameList = input,
                            RandomServiceName = 'random_inj',
                            NumSamples = samples,     # <-- Number of re-samples of the same shower
                            RelocationX = x,  # <-- x-coordinate of core or resampling center (if Relocation R > 0)
                            RelocationY = y, # <-- y-coordinate or core or resampling center (if Relocation R > 0)
                            RelocationR = r,   # <-- Re-sampling radius (if zero --> fixed core location)
                            IgnoreParticleTypes = [75, 76, 85, 86, 95, 96]
                            )
        elif injector == "unthin":
            tray.AddService("I3InjectorFactory<I3CorsikaThinnedInjector>", "particle-injector",
                  FileNameList = input,
                  RandomServiceName = 'random_inj',
                  NumSamples = samples,     # <-- Number of re-samples of the same shower
                  RelocationX = x,  # <-- x-coordinate of core or resampling center (if Relocation R > 0)
                  RelocationY = y, # <-- y-coordinate or core or resampling center (if Relocation R > 0)
                  RelocationR = r,   # <-- Re-sampling radius (if zero --> fixed core location)
                  SmartUnthinning = False,
                  IgnoreParticleTypes = [75, 76, 85, 86, 95, 96]
                  )
        else:
            raise ValueError("unknown injector option")

    if response == 'g4':
        from icecube import g4_tankresponse
        tray.AddService("I3IceTopResponseFactory<I3G4TankResponse>", "topresponse",
                        RandomServiceName =  "random_resp",
                        ChargeScale =  1.02
                        )
    elif response == 'param':
        tray.AddService("I3IceTopResponseFactory<I3ParamTankResponse>", "topresponse",
                        RandomServiceName =  "random_resp",
                        UseSnowParam = True
                        )
    else:
        raise ValueError("Unknown IceTop tank response: " + response)

    def Inspect(frame):
        print frame.keys()

    tray.AddModule("I3InfiniteSource", "source",
                   prefix = gcd,
                   stream = icetray.I3Frame.DAQ )

    tray.AddModule(Inspect, "inspect1")

    tray.AddModule("I3TopSimulator","I3TopSimulator",
                   InjectorServiceName = "particle-injector",
                   ResponseServiceName = "topresponse",
                   PrimaryName = "MCPrimary",
                   InIceMCTreeName = "",
                   IceTopMCTreeName = "I3MCTreeIT",
                   MuonCountName = "MuonCountMap",
                   MuonEnergyCutOff = 0.0,
                   CompressPEs = 0
                   )

    tray.AddModule(Inspect, "inspect2")

    if output is not None:  # writing of output is requested
        # write standard output format i3
        tray.AddModule("I3Writer", "i3-writer",
                       Filename = output,
                       streams = [icetray.I3Frame.DAQ]
                       )


parser = OptionParser(formatter=IndentedHelpFormatter(indent_increment=4, max_help_position=60))
parser.add_option("-o", "--output", action="store", dest="output", default="injector-test.i3.gz",
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
parser.add_option("--gcd", action="store", dest="gcd", metavar="gcd_file",
                  help="GCD file")
parser.add_option("--response", action="store", dest="response", metavar="[param|g4]", default="param",
                  help="Tank response to use. Default: param")
parser.add_option("--injector", action="store", dest="injector",
                  help="Injector to use. Default: unthin", metavar="[normal|unthin]", default="unthin")
parser.add_option("--debug", action="store_const", dest="log_level",
                  help="Debug log-level", const=1, metavar="N", default=2)
parser.add_option("--trace", action="store_const", dest="log_level",
                  help="Trace log-level", const=0, metavar="N", default=2)

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
