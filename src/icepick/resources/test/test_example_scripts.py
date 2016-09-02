#!/usr/bin/env python

import unittest
from icecube.icetray import I3Test

class TestSimpleExample(I3Test.TestExampleScripts):

    project_name = "icepick"

    def test_pypick_example(self):
        input_file = self.I3_TESTDATA + "/sim/Level2_IC86.2011_corsika.010281.001664.00.i3.bz2"
        self.run_example('pypick_example.py', '--i', input_file)

unittest.main()
