#!/usr/bin/env python

import unittest
from I3Tray import I3Tray, I3Units
from icecube import icetray, dataclasses, DomTools
from icecube.icetray import OMKey

doms = [OMKey(1,1), OMKey(1,2), OMKey(1,3), OMKey(1,4), OMKey(2,4)]

def make_geometry():
    geo = dataclasses.I3Geometry()
    for omkey in doms:
        geo.omgeo[omkey] = dataclasses.I3OMGeo()
    geo.omgeo[icetray.OMKey(1,1)].position = dataclasses.I3Position(0,0,0)
    geo.omgeo[icetray.OMKey(1,2)].position = dataclasses.I3Position(0,0,50)
    geo.omgeo[icetray.OMKey(1,3)].position = dataclasses.I3Position(0,0,100)
    geo.omgeo[icetray.OMKey(1,4)].position = dataclasses.I3Position(0,0,500)
    geo.omgeo[icetray.OMKey(2,4)].position = dataclasses.I3Position(0,50,500)
    return geo

def make_rpsmap():
    rpsmap = dataclasses.I3RecoPulseSeriesMap()
    for omkey in doms:
        rpsmap[omkey] = dataclasses.I3RecoPulseSeries()

    # time, width, and charge
    rp_properties = [(1650,100,0),
                     (1850,100,1),
                     (0,100,2),
                     (550,0,3),
                     (800,0,4),
                     (3500,100,5),
                     (3500,100,6),
                     (3500,100,7),
                     (3500,100,8),
                     (8000,3000,9),
                     (9000,100,10),
                     (9500,100,11)]

    rp_list = [dataclasses.I3RecoPulse() for i in range(len(rp_properties))]
    for idx, prop in enumerate(rp_properties):
        rp_list[idx].time = prop[0]
        rp_list[idx].width = prop[1]
        rp_list[idx].charge = prop[2]

    rpsmap[doms[0]].append(rp_list[0])
    rpsmap[doms[0]].append(rp_list[1])
    rpsmap[doms[0]].append(rp_list[2])
    rpsmap[doms[1]].append(rp_list[3])
    rpsmap[doms[2]].append(rp_list[4])
    rpsmap[doms[2]].append(rp_list[5])
    rpsmap[doms[3]].append(rp_list[6])
    rpsmap[doms[3]].append(rp_list[7])
    rpsmap[doms[4]].append(rp_list[8])
    rpsmap[doms[4]].append(rp_list[9])
    rpsmap[doms[4]].append(rp_list[10])
    rpsmap[doms[4]].append(rp_list[11])

    return rpsmap
    
def setup(frame):
    frame["I3Geometry"] = make_geometry()
    frame["test_pulses"] = make_rpsmap()

class I3IsolatedHitsCutModuleTest(unittest.TestCase):

    def test_I3IsolatedHitsCutModule(self):
        rpsmap = dataclasses.I3RecoPulseSeriesMap.from_frame(self.frame, "cleaned_pulses")
        hit_numbers = [rp for omkey, rps in rpsmap for rp in rps]
        self.assertEqual(len(hit_numbers), 7,"IsolatedHitsCutModule didn't leave the right number of hits")            

        hit_numbers.sort(cmp = lambda rp1, rp2 : rp1.time < rp2.time)            
        self.assertEqual(hit_numbers[0].charge, 3, "1st surviving hit after IsolatedHitsCutModule should have been #3")
        self.assertEqual(hit_numbers[1].charge, 6, "2nd surviving hit after IsolatedHitsCutModule should have been #6")
        self.assertEqual(hit_numbers[2].charge, 7, "3rd surviving hit after IsolatedHitsCutModule should have been #7")
        self.assertEqual(hit_numbers[3].charge, 8, "4th surviving hit after IsolatedHitsCutModule should have been #8")
        self.assertEqual(hit_numbers[4].charge, 9, "5th surviving hit after IsolatedHitsCutModule should have been #9")
        self.assertEqual(hit_numbers[5].charge, 10, "6th surviving hit after IsolatedHitsCutModule should have been #10")
        self.assertEqual(hit_numbers[6].charge, 11, "7th surviving hit after IsolatedHitsCutModule should have been #11")

tray = I3Tray()
tray.Add("BottomlessSource")
tray.Add(setup)
tray.Add("I3IsolatedHitsCutModule<I3RecoPulse>",
               InputResponse = "test_pulses",
               OutputResponse = "cleaned_pulses",
               RTRadius = 100*I3Units.m,
               RTTime = 500*I3Units.ns,
               RTMultiplicity = 2)
tray.Add(icetray.I3TestModuleFactory(I3IsolatedHitsCutModuleTest))
tray.Execute(100)
tray.Finish()

