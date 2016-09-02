#!/usr/bin/env python

"""
Calculate weights for 5-component CORSIKA simulation.
"""

from optparse import OptionParser
parser = OptionParser()
parser.add_option("--dataset", type=int, default=10285)
parser.add_option("--nframes", type=int, default=2)
opts, args = parser.parse_args()

try:
    from icecube.weighting import weighting
    mysql = weighting._import_mysql()
except ImportError:
    print("This test is disabled temporarily until mysql is installed on the bots.")
    print("To follow the progress check out : http://code.icecube.wisc.edu/projects/icecube/ticket/1295")
    import sys
    sys.exit(0)


from icecube import icetray, dataclasses, dataio
from icecube.hdfwriter import I3HDFWriter
from icecube.weighting import weighting
from I3Tray import I3Tray

from compare_oneweight import get_random_filename
import numpy

icetray.logging.set_level_for_unit('Python', 'INFO')

tray = I3Tray()

tray.AddModule('I3Reader', 'reader', filenamelist=[get_random_filename(opts.dataset)])
# do an end run around the cache. do not do this in production. ever.
opts.dataset = weighting.from_simprod(opts.dataset)

from icecube.weighting import CORSIKAWeightCalculator
from icecube.weighting import fluxes
tray.AddModule(CORSIKAWeightCalculator, 'GaisserH3aWeight', Dataset=opts.dataset, Flux=fluxes.GaisserH3a())

def check_weight(frame):
    assert 'GaisserH3aWeight' in frame
    weight = frame['GaisserH3aWeight'].value
    icetray.logging.log_info('GaisserH3aWeight: %f' % (weight))
    assert numpy.isfinite(weight)
    assert weight > 0

tray.Add(check_weight, Streams=[icetray.I3Frame.DAQ])

tray.AddModule('TrashCan', 'YesWeCan')
tray.Execute(opts.nframes)
tray.Finish()
