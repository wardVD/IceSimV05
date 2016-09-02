#!/usr/bin/env python
'''
Runs the detector simulation for IC59 for primary events.

It runs with the PolyPlopia segment, but PolyPlopia is turned off.

Minimum inputs:
 - gcdfile: A GCD file
 - inputfile: an input file with I3MCPEs

Optional inputs:
 - summaryfile: A XML summary file

To see all the options, do "detector_ic59.py --help" on the cmd line.
'''

import os
import logging
logging.basicConfig()
rootLogger = logging.getLogger('')
rootLogger.setLevel(logging.INFO)

from icecube.simprod.modules import IC59

if __name__ == '__main__':
    det = IC59()
    det.ExecuteOpts()
