#!/usr/bin/env python

try:
    import unittest2 as unittest
except ImportError:
    import unittest

import os
import cPickle as pickle
from icecube.icetray import I3Test

from icecube import icetray

def all_histograms_are_not_empty(histograms):
    all_is_well = True
    for name, histogram in histograms.iteritems():
        if sum(histogram.bin_values) == 0 :
            icetray.logging.log_error("%s is empty" % histogram.name)
            all_is_well = False
    return all_is_well

def cleanup():
    files = ["trigger.pkl", "level1.pkl", "level2.pkl"]
    for fn in files:
        if os.path.exists(fn):
            os.remove(fn)
            
class TestSimpleExample(I3Test.TestExampleScripts):

    project_name = "production-histograms"

    def setUp(self):
        self.addCleanup(cleanup)

    def test_trigger_level(self):
        # FIXME : Need to get a good sample into test-data.
        # We may have to generte this ourselves, since we'll also want
        # things like IceTop and SLOP triggers if we want to be complete,
        # ...and we do.
        #input_file = self.I3_TESTDATA + "/production-histograms/nugen_test_file.i3.bz2"
        input_file = os.path.expandvars("$I3_BUILD/Level2_IC86.2012_nugen_numu.011374.000857.clsim-base-4.0.3.0.99_eff.i3.bz2")
        self.run_example('trigger_level_histograms.py', '-i', input_file, '-o', 'trigger.pkl')

        h = pickle.load(open("trigger.pkl"))
        self.assertTrue(all_histograms_are_not_empty(h), msg = "Generated some empty histograms")

    def test_filter_level1(self):
        input_file = os.path.expandvars("$I3_BUILD/Level2_IC86.2012_nugen_numu.011374.000857.clsim-base-4.0.3.0.99_eff.i3.bz2")
        self.run_example('filter_level1_histograms.py', '-i', input_file, '-o', 'level1.pkl')

        h = pickle.load(open("level1.pkl"))
        self.assertTrue(all_histograms_are_not_empty(h), msg = "Generated some empty histograms")

    def test_filter_level2(self):
        input_file = os.path.expandvars("$I3_BUILD/Level2_IC86.2012_nugen_numu.011374.000857.clsim-base-4.0.3.0.99_eff.i3.bz2")
        self.run_example('filter_level2_histograms.py', '-i', input_file, '-o', 'level2.pkl')

        h = pickle.load(open("level2.pkl"))
        self.assertTrue(all_histograms_are_not_empty(h), msg = "Generated some empty histograms")

    def tearDown(self):
        '''
        Be nice and clean up after yourself.
        '''

unittest.main()
