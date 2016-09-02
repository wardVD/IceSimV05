#!/usr/bin/env python

import unittest
from icecube import dataclasses
from I3Tray import I3Units

class TestI3DOMCalibration(unittest.TestCase):

    def test_LinearFit_equality(self):
        fit1 = dataclasses.LinearFit()
        fit1.slope = 1.618
        fit1.intercept = 1.20205

        fit2 = dataclasses.LinearFit()
        fit2.slope = 1.618
        fit2.intercept = 1.20205
        
        self.assertEqual(fit1, fit2, "these should be the same.")

    def test_QuadraticFit_equality(self):
        fit1 = dataclasses.QuadraticFit()
        fit1.quad_fit_a = 1.618
        fit1.quad_fit_b = 1.645
        fit1.quad_fit_c = 1.20205

        fit2 = dataclasses.QuadraticFit()
        fit2.quad_fit_a = 1.618
        fit2.quad_fit_b = 1.645
        fit2.quad_fit_c = 1.20205
        
        self.assertEqual(fit1, fit2, "these should be the same.")

    def test_SPEChargeDistribution_equality(self):
        d1 = dataclasses.SPEChargeDistribution()
        d1.exp_amp = 0.5772
        d1.exp_width = 1.645
        d1.gaus_amp = 1.202
        d1.gaus_mean = 1.0823
        d1.gaus_width = 2.612

        d2 = dataclasses.SPEChargeDistribution()
        d2.exp_amp = 0.5772
        d2.exp_width = 1.645
        d2.gaus_amp = 1.202
        d2.gaus_mean = 1.0823
        d2.gaus_width = 2.612
        
        self.assertEqual(d1, d2, "these should be the same.")

    def test_TauParam_equality(self):
        d1 = dataclasses.TauParam()
        d1.p0 = 0.5772
        d1.p1 = 1.645
        d1.p2 = 1.202
        d1.p3 = 1.0823
        d1.p4 = 2.612
        d1.p5 = -1.4604
        d1.tau_frac = -0.5
        
        d2 = dataclasses.TauParam()
        d2.p0 = 0.5772
        d2.p1 = 1.645
        d2.p2 = 1.202
        d2.p3 = 1.0823
        d2.p4 = 2.612
        d2.p5 = -1.4604
        d2.tau_frac = -0.5
       
        self.assertEqual(d1, d2, "these should be the same.")


    def test_DroopedSPETemplate_equality(self):
        temp1 = dataclasses.SPETemplate()
        temp1.c = 1.89
        temp1.x0= 1.2
        temp1.b1= 1.2
        temp1.b2= 1.1
        temp2 = dataclasses.SPETemplate()
        temp2.c = 1.89
        temp2.x0= 1.2
        temp2.b1= 1.2
        temp2.b2= 1.1

        droop1 = dataclasses.DroopedSPETemplate(temp1,temp2,1.1,1.1,1.1)
        droop2 = dataclasses.DroopedSPETemplate(temp1,temp2,1.1,1.1,1.1)
         
        self.assertEqual(droop1, droop1, "these should be the same.")


unittest.main()        

