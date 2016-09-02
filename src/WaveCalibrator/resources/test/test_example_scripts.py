#!/usr/bin/env python

import os
import unittest
from icecube.icetray import I3Test

class TestSimpleExample(I3Test.TestExampleScripts):

    project_name = "WaveCalibrator"

    def test_simple_example(self):
        '''
        Test that runs the one example script in this project.
        '''
        gcd_file = self.I3_TESTDATA + "/sim/GeoCalibDetectorStatus_2013.56429_V1.i3.gz"
        input_file = self.I3_TESTDATA + "/sim/Level2_IC86.2011_corsika.010281.001664.00.i3.bz2"
        output_file = "./output.i3.gz"
        self.run_example('simple_example.py', gcd_file, input_file, output_file)

    def tearDown(self):
        '''
        Be nice and clean up after yourself.
        '''
        os.remove("./output.i3.gz")
        
unittest.main()
