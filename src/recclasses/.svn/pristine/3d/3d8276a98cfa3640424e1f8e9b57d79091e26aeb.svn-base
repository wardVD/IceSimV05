#!/usr/bin/env python
import sys
if sys.version_info < (2,7):
    import unittest2 as unittest
else:
    import unittest    
from icecube import icetray, recclasses
import cPickle
import StringIO
from math import isnan, log10


def with_standard_tests(cls):
    class enhanced_class(cls):

        def test_serialization(self):
            obj = self.obj
            frame = icetray.I3Frame()
            frame["obj"] = self.obj
            pseudo_file = StringIO.StringIO()
            cPickle.dump(frame, pseudo_file)
            pseudo_file.seek(0)
            frame = cPickle.load(pseudo_file)
            obj2 = frame["obj"]
            self.assertNotEqual(id(obj), id(obj2))
            self.assertEqual(obj, obj2,
                "\noriginal: {0}\ndeserialized: {1}"
                .format(obj, obj2))

        def test_obj_is_not_default(self):
            obj = self.obj
            self.assertNotEqual(obj, obj.__class__())

    return enhanced_class


@with_standard_tests
class TestI3CompactKeyList(unittest.TestCase):

    def setUp(self):
        from icecube.icetray import OMKey
        obj = recclasses.I3CompactKeyList()
        obj.append(OMKey(1, 2))
        obj.append(OMKey(3, 4))
        self.obj = obj

    def test_default_equal_to_self(self):
        cls = self.obj.__class__
        self.assertEqual(cls(), cls())


@with_standard_tests
class TestI3DipoleFitParams(unittest.TestCase):

    def setUp(self):
        obj = recclasses.I3DipoleFitParams()
        obj.Magnet = 1.0
        obj.MagnetX = 1.1
        obj.MagnetY = 1.2
        obj.MagnetZ = 1.3
        obj.AmpSum = 2.0
        obj.NHits = 3
        obj.NPairs = 4
        obj.MaxAmp = 5.0
        self.obj = obj

    def test_default_equal_to_self(self):
        cls = self.obj.__class__
        self.assertEqual(cls(), cls())


@with_standard_tests
class TestI3FillRatioInfo(unittest.TestCase):

    def setUp(self):
        obj = recclasses.I3FillRatioInfo()
        obj.mean_distance = 1.0
        obj.rms_distance = 2.0
        obj.energy_distance = 3.0
        obj.fill_radius_from_mean = 4.0
        obj.fill_radius_from_mean_plus_rms = 5.0
        obj.fill_radius_from_energy = 6.0
        obj.fill_ratio_from_rms = 7.0
        obj.fill_ratio_from_mean = 8.0
        obj.fill_ratio_from_mean_plus_rms = 9.0
        obj.fill_ratio_from_energy = 10.0
        obj.hit_count = 11
        self.obj = obj

    def test_default_equal_to_self(self):
        cls = self.obj.__class__
        self.assertEqual(cls(), cls())


@with_standard_tests
class TestI3FiniteCuts(unittest.TestCase):

    def setUp(self):
        obj = recclasses.I3FiniteCuts()
        obj.Length = 1.0
        obj.endFraction = 2.0
        obj.startFraction = 3.0
        obj.Sdet = 4.0
        obj.finiteCut = 5.0
        obj.DetectorLength = 6.0
        self.obj = obj

    def test_default_equal_to_self(self):
        cls = self.obj.__class__
        self.assertEqual(cls(), cls())


@with_standard_tests
class TestI3LaputopParams(unittest.TestCase):

    def setUp(self):
        from icecube.recclasses import LaputopParameter as Par, \
            LaputopLDF as LDF, LaputopFrontDelay as FrontDelay, \
            LaputopEnergy as Energy
        obj = recclasses.I3LaputopParams()
        obj.set_value(Par.Log10_S125, 1.2)
        obj.set_value(Par.Beta, 3.4)
        obj.set_cov(Par.Log10_S125, Par.Beta, 5.6)
        obj.set_error(Par.Beta, 7.8)
        obj.type_ldf = LDF.DLP
        obj.type_front_delay = FrontDelay.GaussParabola
        obj.type_energy = Energy.IC73SpectrumPaper
        self.obj = obj

    def test_default_equal_to_self(self):
        cls = self.obj.__class__
        self.assertEqual(cls(), cls())

    def test_has_Log10_S125(self):
        from icecube.recclasses import LaputopParameter as Par
        self.assertTrue(self.obj.has(Par.Log10_S125))

    def test_has_beta(self):
        from icecube.recclasses import LaputopParameter as Par
        self.assertTrue(self.obj.has(Par.Beta))

    def test_has_not_Age(self):
        from icecube.recclasses import LaputopParameter as Par
        self.assertFalse(self.obj.has(Par.Age))

    def test_Log10_S125_value(self):
        from icecube.recclasses import LaputopParameter as Par
        self.assertEqual(self.obj.value(Par.Log10_S125), 1.2)

    def test_beta_value(self):
        from icecube.recclasses import LaputopParameter as Par
        self.assertEqual(self.obj.value(Par.Beta), 3.4)

    def test_Log10_S125_error_is_nan(self):
        from icecube.recclasses import LaputopParameter as Par
        self.assertTrue(isnan(self.obj.error(Par.Log10_S125)))

    def test_Log10_S125_cov_is_nan(self):
        from icecube.recclasses import LaputopParameter as Par
        self.assertTrue(isnan(self.obj.cov(Par.Log10_S125, Par.Log10_S125)))

    def test_Age_value_is_nan(self):
        from icecube.recclasses import LaputopParameter as Par
        self.assertTrue(isnan(self.obj.value(Par.Age)))

    def test_Age_error_is_nan(self):
        from icecube.recclasses import LaputopParameter as Par
        self.assertTrue(isnan(self.obj.error(Par.Age)))

    def test_Age_cov_is_nan(self):
        from icecube.recclasses import LaputopParameter as Par
        self.assertTrue(isnan(self.obj.cov(Par.Age, Par.Age)))

    def test_cov_Age_Beta_is_nan(self):
        from icecube.recclasses import LaputopParameter as Par
        self.assertTrue(isnan(self.obj.cov(Par.Age, Par.Beta)))
        self.assertTrue(isnan(self.obj.cov(Par.Beta, Par.Age)))

    def test_cov_Log10_S125_Beta(self):
        from icecube.recclasses import LaputopParameter as Par
        self.assertEqual(self.obj.cov(Par.Log10_S125, Par.Beta), 5.6)
        self.assertEqual(self.obj.cov(Par.Beta, Par.Log10_S125), 5.6)

    def test_beta_error(self):
        from icecube.recclasses import LaputopParameter as Par
        self.assertEqual(self.obj.value(Par.Beta), 3.4)

    def test_beta_cov(self):
        from icecube.recclasses import LaputopParameter as Par
        self.assertEqual(self.obj.cov(Par.Beta, Par.Beta), 7.8 ** 2)

    def test_expected_signal(self):
        obj = self.obj
        self.assertAlmostEqual(obj.expected_signal(1.0), 0.997e7, delta=0.001e7)
        self.assertAlmostEqual(obj.expected_signal(10.0), 0.368e5, delta=0.001e5)
        self.assertAlmostEqual(obj.expected_signal(100.0), 0.336e2, delta=0.001e2)

    def test_expected_shower_front_delay(self):
        obj = self.obj
        self.assertAlmostEqual(obj.expected_shower_front_delay(1.0), -0.1874e-2, delta=0.001e-2)
        self.assertAlmostEqual(obj.expected_shower_front_delay(10.0), -0.1869, delta=0.001)
        self.assertAlmostEqual(obj.expected_shower_front_delay(100.0), -0.1476e2, delta=0.001e2)


@with_standard_tests
class TestI3LineFitParams( unittest.TestCase ):

    def setUp( self ):
        obj = recclasses.I3LineFitParams()
        obj.LFVel = 1.0
        obj.LFVelX = 2.0
        obj.LFVelY = 3.0
        obj.LFVelZ = 4.0
        obj.NHits = 5
        self.obj = obj

    def test_default_equal_to_self(self):
        cls = self.obj.__class__
        self.assertEqual(cls(), cls())


@with_standard_tests
class TestI3OpheliaFirstGuessTrack( unittest.TestCase ):

    def setUp( self ):
        obj = recclasses.I3OpheliaFirstGuessTrack()
        obj.velocity = (1.0, 2.0, 3.0)
        obj.center_of_brightness = (4.0, 5.0, 6.0)
        obj.largest_npe_dom_position = (7.0, 8.0, 9.0)
        obj.fit_quality = 10.0
        obj.fit_successful = True
        self.obj = obj


@with_standard_tests
class TestI3OpheliaParticle( unittest.TestCase ):

    def setUp( self ):
        from icecube.dataclasses import I3Position, I3Direction, I3Particle
        obj = recclasses.I3OpheliaParticle()
        obj.particle = I3Particle(
          I3Position(7.0, 8.0, 9.0),
          I3Direction(0.1, 0.2, 0.3),
          10.0)
        obj.add_daughter(recclasses.I3OpheliaParticle())
        self.obj = obj


@with_standard_tests
class TestI3OpheliaRecoResult( unittest.TestCase ):

    def setUp( self ):
        from icecube.dataclasses import I3Position, I3Direction, I3Particle
        obj = recclasses.I3OpheliaRecoResult()
        t = recclasses.I3OpheliaFirstGuessTrack()
        t.velocity = (1.0, 2.0, 3.0)
        t.center_of_brightness = (4.0, 5.0, 6.0)
        t.largest_npe_dom_position = (7.0, 8.0, 9.0)
        t.fit_quality = 10.0
        t.fit_successful = True
        obj.first_guess_track = t
        p = recclasses.I3OpheliaParticle()
        p.particle = I3Particle(
          I3Position(7.0, 8.0, 9.0),
          I3Direction(0.1, 0.2, 0.3),
          10.0)
        p.add_daughter(recclasses.I3OpheliaParticle())
        obj.stochastic_cascade = p
        self.obj = obj


@with_standard_tests
class TestI3ParticleIntersections( unittest.TestCase ):

    def setUp( self ):
        from icecube.dataclasses import I3Position, I3Direction, I3Particle
        obj = recclasses.I3ParticleIntersections()
        obj.intersections = (I3Position(1.0, 2.0, 3.0), I3Position(4.0, 5.0, 6.0))
        obj.particle = I3Particle(
          I3Position(7.0, 8.0, 9.0),
          I3Direction(0.1, 0.2, 0.3),
          10.0)
        self.obj = obj


@with_standard_tests
class TestI3ClastFitParams( unittest.TestCase ):

    def setUp( self ):
        cpar = recclasses.I3CLastFitParams()
        cpar.mineval = 3.0
        cpar.evalratio = 0.9
        cpar.eval2 = 3.1
        cpar.eval3 = 4.1
        self.obj = cpar

if __name__ == "__main__":
    unittest.main()
