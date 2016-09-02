#!/usr/bin/env python

import unittest
from icecube import dataclasses
from I3Tray import I3Units

class Mytest(unittest.TestCase):

    # change all ocurances of
    # dataclasses.I3Trigger.*  --> dataclasses.*
    # once TriggerKey is completely out of I3Trigger scope
    def test_example(self):
        t = dataclasses.TriggerKey()
        print t
        t2 = dataclasses.TriggerKey(dataclasses.I3Trigger.EXTERNAL,
                dataclasses.I3Trigger.STRING)
        print t2
        t3 =  dataclasses.TriggerKey(dataclasses.I3Trigger.AMANDA_MUON_DAQ,
                dataclasses.I3Trigger.STRING)
        print t3
        t4 =  dataclasses.TriggerKey(dataclasses.I3Trigger.AMANDA_TWR_DAQ,
                dataclasses.I3Trigger.STRING)
        print t4

    def test_get_source_string(self):
        tk = dataclasses.TriggerKey()
        self.assertEqual(tk.get_source_string(dataclasses.I3Trigger.IN_ICE),
                "IN_ICE", "Should be IN_ICE")
        self.assertEqual(tk.get_source_string(dataclasses.I3Trigger.ICE_TOP),
                "ICE_TOP", "Should be ICE_TOP")
        self.assertEqual(tk.get_source_string(dataclasses.I3Trigger.AMANDA_TWR_DAQ),
                "AMANDA_TWR_DAQ", "Should be AMANDA_TWR_DAQ")
        self.assertEqual(tk.get_source_string(dataclasses.I3Trigger.EXTERNAL),
                "EXTERNAL", "Should be EXTERNAL")
        self.assertEqual(tk.get_source_string(dataclasses.I3Trigger.GLOBAL),
                "GLOBAL", "Should be GLOBAL")
        self.assertEqual(tk.get_source_string(dataclasses.I3Trigger.AMANDA_MUON_DAQ),
                "AMANDA_MUON_DAQ", "Should be AMANDA_MUON_DAQ")
        self.assertEqual(tk.get_source_string(dataclasses.I3Trigger.SPASE),
                "SPASE", "Should be SPASE")
        self.assertEqual(tk.get_source_string(dataclasses.I3Trigger.UNKNOWN_SOURCE),
                "UNKNOWN_SOURCE", "Should be UNKNOWN_SOURCE")

    def test_get_source_from_string(self):
        tk = dataclasses.TriggerKey()
        self.assertEqual(tk.get_source_from_string("IN_ICE"),
                dataclasses.I3Trigger.IN_ICE, "Should be IN_ICE")
        self.assertEqual(tk.get_source_from_string("ICE_TOP"),
                dataclasses.I3Trigger.ICE_TOP, "Should be ICE_TOP")
        self.assertEqual(tk.get_source_from_string("AMANDA_TWR_DAQ"),
                dataclasses.I3Trigger.AMANDA_TWR_DAQ, "Should be AMANDA_TWR_DAQ")
        self.assertEqual(tk.get_source_from_string("EXTERNAL"),
                dataclasses.I3Trigger.EXTERNAL, "Should be EXTERNAL")
        self.assertEqual(tk.get_source_from_string("GLOBAL"),
                dataclasses.I3Trigger.GLOBAL, "Should be GLOBAL")
        self.assertEqual(tk.get_source_from_string("AMANDA_MUON_DAQ"),
                dataclasses.I3Trigger.AMANDA_MUON_DAQ, "Should be AMANDA_MUON_DAQ")
        self.assertEqual(tk.get_source_from_string("SPASE"),
                dataclasses.I3Trigger.SPASE, "Should be SPASE")
        self.assertEqual(tk.get_source_from_string("UNKNOWN_SOURCE"),
                dataclasses.I3Trigger.UNKNOWN_SOURCE, "Should be UNKNOWN_SOURCE")

    def test_get_type_string(self):
        tk = dataclasses.TriggerKey()
        self.assertEqual(tk.get_type_string(dataclasses.I3Trigger.SIMPLE_MULTIPLICITY),
                "SIMPLE_MULTIPLICITY", "Should be SIMPLE_MULTIPLICITY")
        self.assertEqual(tk.get_type_string(dataclasses.I3Trigger.CALIBRATION),
                "CALIBRATION", "Should be CALIBRATION")
        self.assertEqual(tk.get_type_string(dataclasses.I3Trigger.MIN_BIAS),
                "MIN_BIAS", "Should be MIN_BIAS")
        self.assertEqual(tk.get_type_string(dataclasses.I3Trigger.THROUGHPUT),
                "THROUGHPUT", "Should be THROUGHPUT")
        self.assertEqual(tk.get_type_string(dataclasses.I3Trigger.TWO_COINCIDENCE),
                "TWO_COINCIDENCE", "Should be TWO_COINCIDENCE")
        self.assertEqual(tk.get_type_string(dataclasses.I3Trigger.THREE_COINCIDENCE),
                "THREE_COINCIDENCE", "Should be THREE_COINCIDENCE")
        self.assertEqual(tk.get_type_string(dataclasses.I3Trigger.MERGED),
                "MERGED", "Should be MERGED")
        self.assertEqual(tk.get_type_string(dataclasses.I3Trigger.SLOW_PARTICLE),
                "SLOW_PARTICLE", "Should be SLOW_PARTICLE")
        self.assertEqual(tk.get_type_string(dataclasses.I3Trigger.FRAGMENT_MULTIPLICITY),
                "FRAGMENT_MULTIPLICITY", "Should be FRAGMENT_MULTIPLICITY")
        self.assertEqual(tk.get_type_string(dataclasses.I3Trigger.STRING),
                "STRING", "Should be STRING")
        self.assertEqual(tk.get_type_string(dataclasses.I3Trigger.VOLUME),
                "VOLUME", "Should be VOLUME")
        self.assertEqual(tk.get_type_string(dataclasses.I3Trigger.SPHERE),
                "SPHERE", "Should be SPHERE")
        self.assertEqual(tk.get_type_string(dataclasses.I3Trigger.UNBIASED),
                "UNBIASED", "Should be UNBIASED")
        self.assertEqual(tk.get_type_string(dataclasses.I3Trigger.SPASE_2),
                "SPASE_2", "Should be SPASE_2")
        self.assertEqual(tk.get_type_string(dataclasses.I3Trigger.UNKNOWN_TYPE),
                "UNKNOWN_TYPE", "Should be UNKNOWN_TYPE")

    def test_get_type_from_string(self):
        tk = dataclasses.TriggerKey()
        self.assertEqual(tk.get_type_from_string("SIMPLE_MULTIPLICITY"),
                dataclasses.I3Trigger.SIMPLE_MULTIPLICITY,
                "Should be SIMPLE_MULTIPLICITY")
        self.assertEqual(tk.get_type_from_string("CALIBRATION"),
                dataclasses.I3Trigger.CALIBRATION,
                "Should be CALIBRATION")
        self.assertEqual(tk.get_type_from_string("MIN_BIAS"),
                dataclasses.I3Trigger.MIN_BIAS,
                "Should be MIN_BIAS")
        self.assertEqual(tk.get_type_from_string("THROUGHPUT"),
                dataclasses.I3Trigger.THROUGHPUT,
                "Should be THROUGHPUT")
        self.assertEqual(tk.get_type_from_string("TWO_COINCIDENCE"),
                dataclasses.I3Trigger.TWO_COINCIDENCE,
                "Should be TWO_COINCIDENCE")
        self.assertEqual(tk.get_type_from_string("THREE_COINCIDENCE"),
                dataclasses.I3Trigger.THREE_COINCIDENCE,
                "Should be THREE_COINCIDENCE")
        self.assertEqual(tk.get_type_from_string("MERGED"),
                dataclasses.I3Trigger.MERGED,
                "Should be MERGED")
        self.assertEqual(tk.get_type_from_string("SLOW_PARTICLE"),
                dataclasses.I3Trigger.SLOW_PARTICLE,
                "Should be SLOW_PARTICLE")
        self.assertEqual(tk.get_type_from_string("FRAGMENT_MULTIPLICITY"),
                dataclasses.I3Trigger.FRAGMENT_MULTIPLICITY,
                "Should be FRAGMENT_MULTIPLICITY")
        self.assertEqual(tk.get_type_from_string("STRING"),
                dataclasses.I3Trigger.STRING,
                "Should be STRING")
        self.assertEqual(tk.get_type_from_string("VOLUME"),
                dataclasses.I3Trigger.VOLUME,
                "Should be VOLUME")
        self.assertEqual(tk.get_type_from_string("SPHERE"),
                dataclasses.I3Trigger.SPHERE,
                "Should be SPHERE")
        self.assertEqual(tk.get_type_from_string("UNBIASED"),
                dataclasses.I3Trigger.UNBIASED,
                "Should be UNBIASED")
        self.assertEqual(tk.get_type_from_string("SPASE_2"),
                dataclasses.I3Trigger.SPASE_2,
                "Should be SPASE_2")
        self.assertEqual(tk.get_type_from_string("UNKNOWN_TYPE"),
                dataclasses.I3Trigger.UNKNOWN_TYPE,
                "Should be UNKNOWN_TYPE")



unittest.main()
