#!/usr/bin/env python

import os
import unittest
from icecube.icetray import I3Test

class TestSimpleExample(I3Test.TestExampleScripts):

    project_name = "daq-decode"

    def test_dump_i3_online_data(self):
        '''
        Test that runs the example script dump-i3-online-data.py
        '''
        input_file = self.I3_TESTDATA + "/exp/IceCube/2015/PFFilt_PhysicsFiltering_Run00126598_Subrun00000000_00000028_slim.i3.gz"
        self.run_example('dump-i3-online-data.py', input_file)

    def test_i3_to_ddd(self):
        '''
        Test that runs the example script i3-to-ddd
        '''
        input_file = self.I3_TESTDATA + "/exp/IceCube/2015/PFFilt_PhysicsFiltering_Run00126598_Subrun00000000_00000028_slim.i3.gz"
        self.run_example('i3-to-ddd',
                         '-i',input_file,
                         '-o','output.i3.gz',
                         '-d', 'I3DAQDataTrimmed')

    def tearDown(self):
        '''
        Be nice and clean up after yourself.
        '''
        if os.path.exists("./output.i3.gz"):
            os.remove("./output.i3.gz")
                
unittest.main()
