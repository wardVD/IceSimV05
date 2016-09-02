#!/usr/bin/env python
"""
Script for running IceTop's tank response simulation with single particles as input.
"""

from optparse import OptionParser, IndentedHelpFormatter
from icecube.topsimulator.segments import SimIceTop

parser = OptionParser(formatter=IndentedHelpFormatter(indent_increment=4, max_help_position=60))
parser.add_option("-o", "--output", action="store", type="string", dest="output",
                  help="Output file name (with extension .i3, .i3.gz, .h5 or .hdf)", metavar="FILE")
parser.add_option("-n", action="store", type="int", dest="n_events",
                  help="Number of iterations. Default: 1000", metavar="N", default=1000)
parser.add_option("-t", "--tanks", action="append", type="string", dest="tanks",
                  help="Tank ID, eg 39B. Default: all tanks.", metavar="tank_id", default=[])
parser.add_option("-p", "--particle", action="store", type="string", dest="particle",
                  help="Particle ID. One of: mu+, mu-, e+, e-, gamma, p+, p-, n. Default: mu-", metavar="id", default='mu-')
parser.add_option("-e", "--energy", action="append", type="float", dest="energy",
                  help="Energy (GeV). Can give two values to specify a range. Default: 1 GeV", metavar="energy")
parser.add_option("-z", "--zenith", action="append", type="float", dest="zenith",
                  help="Zenith angle (deg).  Can give two values to specify a range. Default: 0 deg.", metavar="zenith")
parser.add_option("-r", "--radius", action="append", type="float", dest="radius",
                  help="Injection point's maximum distance to center of tank (m). Default: 0 m.",
                  metavar="radius", default=[0])
parser.add_option("--new", action="store_true", dest="new",
                  help="Use new simulation (DOMLauncher). Default.", default=True)
parser.add_option("--old", action="store_false", dest="new",
                  help="Use old simulation (I3PMTSimulator)")
parser.add_option("--gcd", action="store", type="string", dest="gcd", help="GCD file",
                  metavar="gcd_file")
parser.add_option("--response", action="store", dest="response",
                  help="Tank response to use. Default: g4", metavar="[param|g4]", default='g4')
parser.add_option("--rng", action="store", dest="rng",
                  help="Random number generator to use: Default: gsl, metavar=[gsl|sprng|root]",
                  default="gsl")
parser.add_option("--debug", action="store_const", dest="log_level",
                  help="Debug log-level", const=1, metavar="N", default=2)
parser.add_option("--trace", action="store_const", dest="log_level",
                  help="Trace log-level", const=0, metavar="N", default=2)
parser.add_option("--calibrate", action="store_true", dest="calibrate",
                  help="Make RecoPulses and Waveforms", default=False)

(options, infiles) = parser.parse_args()

if not options.energy:
    options.energy = [1]
if not options.zenith:
    options.zenith = [0.]

options.input = (
  "I3InjectorFactory<I3ParticleInjector>",
  dict(
    AllTanks = len(options.tanks) == 0,
    RadiusRange = options.radius,
    TankKeys = options.tanks,
    NumEvents = options.n_events,
    ParticleType = options.particle,
    EnergyRange = options.energy,
    ZenithRange = options.zenith
  )
)

options = vars(options)
# don't pass these to SimIceTop, they are not understood
del options["radius"]
del options["n_events"]
del options["particle"]
del options["energy"]
del options["zenith"]

# Instantiate and execute a tray
try:
    from I3Tray import I3Tray
    tray = I3Tray()
    tray.AddSegment(SimIceTop, 'sim_single_particles', **options)
    tray.AddModule("TrashCan", "trashcan")
    tray.Execute()
    tray.Finish()
except (ValueError, IOError):
    import sys
    exc_type, exc_value, traceback = sys.exc_info()
    print "Error:", exc_value
    parser.print_help()
    sys.exit(1)
