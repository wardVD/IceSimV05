#!/usr/bin/env python
#
# $Id: add_slop_params_to_gcd.py 137437 2015-09-10 03:14:12Z olivas $
#

'''
This example creates a new GCD file with the new SLOP trigger settings.
'''

import sys
import os.path
from optparse import OptionParser

usage = """ %prog -g [GCD file] [Options]

This script writes a GCD file with additional or modified configuration parameters for the SlowMonopole Trigger.
The original GCD remains untouched."""

parser = OptionParser(usage=usage)

parser.add_option("-g","--gcd", dest="gcd_file", metavar="FILE",
                  help="gcd file")

parser.add_option("-o","--output", dest="outfile", metavar="FILE",
                  help="optional output filename")

parser.add_option("-r","--readonly", action="store_true", dest="readonly",
                  help="print slop parameters from GCD and exit")

parser.add_option("--dc_algo", dest="dc_algo", help="dc_algo", type='int')

parser.add_option("--alpha_min", dest="alpha_min", help="alpha_min")

parser.add_option("--delta_d", dest="delta_d", help="delta_d")

parser.add_option("--domSet", dest="domSet", help="domSet")

parser.add_option("--min_n_tuples", dest="min_n_tuples", help="min_n_tuples")

parser.add_option("--rel_v", dest="rel_v", help="rel_v")

parser.add_option("--t_proximity", dest="t_proximity", help="t_proximity")

parser.add_option("--t_min", dest="t_min", help="t_min")

parser.add_option("--t_max", dest="t_max", help="t_max")

parser.add_option("--max_event_length", dest="max_event_length", help="max_event_length")

(options, args) = parser.parse_args()

gcd_file = options.gcd_file
outfile = options.outfile

# some sanity checks:
if options.gcd_file is None:
    print("GCD is None")
    parser.print_help()
    sys.exit(1)
if options.dc_algo is not None and options.dc_algo not in set([0,1]):
    print('dc_algo can be either 0 or 1')
    sys.exit(1)

# add extension for output file if not given with filename
if options.outfile is not None and os.path.splitext(outfile)[1] not in ['.gz','.bz2','.i3'] :
    outfile = outfile + '.i3.gz'

# determine name for output file if not given
if options.outfile is None:
    gcd_path,ext = os.path.splitext(gcd_file)
    if ext in ['.gz','.bz2']:
        ext = os.path.splitext(gcd_path)[1] + ext
        gcd_path = os.path.splitext(gcd_path)[0]
    outfile = gcd_path + '_SLOPified' + ext

# import icecube stuff
try:
    from I3Tray import *
    from icecube import icetray, dataclasses, dataio
except ImportError as exc:
    print("You have to run this inside an envshell.sh environment!")
    print(exc)
    sys.exit(1)

# open files for reading and writing
oldgcd = dataio.I3File(gcd_file)
if not options.readonly: newgcd = dataio.I3File(outfile ,dataio.I3File.Mode.Writing)

# let's go
while oldgcd.more():
    frame = oldgcd.pop_frame()
    if frame.Has('I3DetectorStatus'):
        i3ds = frame.Get('I3DetectorStatus')

        # print slop parameters of the original file
        for tkey, ts in i3ds.trigger_status:
            if tkey.type == dataclasses.I3Trigger.SLOW_PARTICLE :
                print("\nThe SLOP trigger configuration parameters in the file %s are:" % os.path.basename(gcd_file))
                print(tkey)
                for sets in ts.trigger_settings:
                    print(sets[0] + ': %s' % sets[1])
        # if -r option was given exit after printing the parameters
        if options.readonly:
            exit(0)

        # write the new parameters
        for tkey, ts in i3ds.trigger_status:
            if tkey.type == dataclasses.I3Trigger.SLOW_PARTICLE :
                if options.dc_algo is not None:
                    ts.trigger_settings['dc_algo'] = str(options.dc_algo)
                if options.alpha_min is not None:
                    ts.trigger_settings['alpha_min'] = options.alpha_min
                if options.delta_d is not None:
                    ts.trigger_settings['delta_d'] = options.delta_d
                if options.domSet is not None:
                    ts.trigger_settings['domSet'] = options.domSet
                if options.min_n_tuples is not None:
                    ts.trigger_settings['min_n_tuples'] = options.min_n_tuples
                if options.rel_v is not None:
                    ts.trigger_settings['rel_v'] = options.rel_v
                if options.t_proximity is not None:
                    ts.trigger_settings['t_proximity'] = options.t_proximity
                if options.t_min is not None:
                    ts.trigger_settings['t_min'] = options.t_min
                if options.t_max is not None:
                    ts.trigger_settings['t_max'] = options.t_max
                if options.max_event_length is not None:
                    ts.trigger_settings['max_event_length'] = options.max_event_length

                # write new trigger status
                i3ds.trigger_status[tkey] = ts        

                # print slop parameters of the generated file
                print("\nThe following SLOP trigger configuration parameters have been written to %s:" % os.path.basename(outfile))
                print(tkey)
                for sets in ts.trigger_settings:
                    print(sets[0] + ': %s' % sets[1])

        # delete the original D frame and replace it with modified one
        del frame["I3DetectorStatus"]   
        frame["I3DetectorStatus"] = i3ds
    if not options.readonly:
        newgcd.push(frame)
if not options.readonly:
    newgcd.close()
print("\nFile generated: " + outfile) 
exit(0)
