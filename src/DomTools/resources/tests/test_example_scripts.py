#!/usr/bin/env python

import unittest
from icecube.icetray import I3Test

class TestSimpleExample(I3Test.TestExampleScripts):

    project_name = "DomTools"

    def test_first_pulsifier_example(self):
        self.run_example('I3FirstPulsifierExample.py')

    def test_lccleaning_dom_launches(self):
        self.run_example('I3LCCleaning-DOMLaunches.py')

unittest.main()
