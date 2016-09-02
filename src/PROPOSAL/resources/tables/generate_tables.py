#!/usr/bin/env python

import argparse
from os.path import expandvars

parser = argparse.ArgumentParser(description='Generate PROPOSAL tables.')
parser.add_argument('--table-path', '-t',
                        dest = "TABLE_DIR",
                        default = expandvars("$I3_BUILD/PROPOSAL/resources/tables/"),
                        help='Generates muon tables.')
args = parser.parse_args()

import os

from icecube.dataclasses import I3Particle
from icecube.PROPOSAL import I3PropagatorServicePROPOSAL

# Yeah, it's that simple.  PROPOSAL, on instantiation, will generate tables
# if it can't find them in the specified location.
I3PropagatorServicePROPOSAL(tabledir = args.TABLE_DIR, type=I3Particle.MuMinus)
I3PropagatorServicePROPOSAL(tabledir = args.TABLE_DIR, type=I3Particle.TauMinus)
I3PropagatorServicePROPOSAL(tabledir = args.TABLE_DIR, type=I3Particle.STauPlus, particleMass = 150)
I3PropagatorServicePROPOSAL(tabledir = args.TABLE_DIR, type=I3Particle.STauMinus, particleMass = 150)

# This should have generated all data files.
# Generate a marker file to tell the build system this task
# has been successfully completed.
f = open(args.TABLE_DIR + "/.tables.auto_generated", "w")
f.close()

