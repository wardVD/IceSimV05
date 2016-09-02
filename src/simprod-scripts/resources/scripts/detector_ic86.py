#!/usr/bin/env python
'''
Runs the detector simulation for IC86 for primary events.

Optionally simulates coincident background in-ice using PolyPlopia.
The number of coincidences may be random or forced, in the latter
case the event is weighted according to the chance probability for
such a multiplicity.

Minimum inputs:
 - gcdfile: A GCD file
 - inputfile: an input file with I3MCPEs

Optional inputs:
 - summaryfile: A XML summary file
 - BackGroundMCFile: a file with background events.

By default, background simulation is off. To turn it on, either set
  EnablePolyplopia = True
or
  NumberOfPrimaries = <integer larger than 1>

If PolyPlopia is used, you can define the PolyplopiaRate = <some rate>,
or set it to zero, then the rate is computed from summaryfile.

To see all the options, do "detector.py --help" on the cmd line.
'''

import os
import logging
logging.basicConfig()
rootLogger = logging.getLogger('')
rootLogger.setLevel(logging.INFO)

from icecube.simprod.modules import IC86

if __name__ == '__main__':
    det = IC86()
    det.ExecuteOpts()
