#!/usr/bin/env python
import sys
if sys.version_info < (2,7):
    import unittest2 as unittest
else:
    import unittest    
from math import isnan, log10, pi, cos, sin, atan2
from icecube.recclasses import \
    I3LaputopParams, LaputopParameter as Par, \
    LaputopLDF as LDF, LaputopFrontDelay as FrontDelay, \
    LaputopEnergy as Energy


class TestEnergy(unittest.TestCase):

    def setUp(self):
        self.obj = I3LaputopParams()
        self.x = 0.5
        self.obj.set_value(Par.Log10_S125, self.x)
        self.obj.type_energy = Energy.IC73SpectrumPaper

    def cosz(self, cosz):
        v = (0.5 * (1.0 - cosz ** 2)) ** 0.5
        self.obj.set_value(Par.Nx, v)
        self.obj.set_value(Par.Ny, v)

    def test_large_angle(self):
        self.obj.set_value(Par.Nx, 1.0)
        self.obj.set_value(Par.Ny, 1.0)
        self.assertTrue(isnan(self.obj.energy()))
        self.cosz(0.79)
        self.assertTrue(isnan(self.obj.energy()))

    def test_100(self):
        self.cosz(1.00)
        self.assertAlmostEqual(log10(self.obj.energy()), (6.018 + 0.938 * self.x))

    def test_098(self):
        self.cosz(0.98)
        self.assertAlmostEqual(log10(self.obj.energy()), (6.018 + 0.938 * self.x))

    def test_092(self):
        self.cosz(0.92)
        self.assertAlmostEqual(log10(self.obj.energy()), (6.062 + 0.929 * self.x))

    def test_088(self):
        self.cosz(0.88)
        self.assertAlmostEqual(log10(self.obj.energy()), (6.117 + 0.921 * self.x))

    def test_082(self):
        self.cosz(0.82)
        self.assertAlmostEqual(log10(self.obj.energy()), (6.182 + 0.914 * self.x))


class TestTransform(unittest.TestCase):

    def runTest(self):
        param = I3LaputopParams()
        param.type_ldf = LDF.DLP
        param.type_front_delay = FrontDelay.GaussParabola
        param.type_energy = Energy.ICRC2015_H4a_E27
        par_list = ('Xc', 'Yc', 'Log10_S125', 'Beta')
        npar = len(par_list)
        for i in xrange(npar):
            pari = getattr(Par, par_list[i])
            for j in xrange(npar):
                parj = getattr(Par, par_list[j])
                param.set_cov(pari, parj, 1.0 + i + 0.1 * j)
        param.set_value(Par.Log10_S125, 10.0)
        param.set_value(Par.Beta, 11.0)
        param.set_value(Par.Age, 12.0)
        param.log_likelihood = 0.5
        param.log_likelihood_silent = 0.6
        param.chi2_ldf = 0.7
        param.chi2_time = 0.8
        param.ndf_ldf = 4
        param.ndf_time = 4
        param.n_mini = 5

        from icecube.recclasses import I3TopLateralFitParams
        tlfp = I3TopLateralFitParams()
        tlfp.s125 = 10 ** param.value(Par.Log10_S125)
        tlfp.beta = param.value(Par.Beta)
        tlfp.age = param.value(Par.Age)
        tlfp.llh = param.log_likelihood
        tlfp.llh_silent = param.log_likelihood_silent
        tlfp.chi2 = param.chi2_ldf / param.ndf_ldf
        tlfp.chi2_time = param.chi2_time / param.ndf_time
        tlfp.ndf = param.ndf_ldf
        tlfp.nmini = param.n_mini
        tlfp.xc_err = param.error(Par.Xc)
        tlfp.yc_err = param.error(Par.Yc)
        tlfp.log10_s125_err = param.error(Par.Log10_S125)
        tlfp.beta_err = param.error(Par.Beta)
        tlfp.corr_xc_yc = param.cov(Par.Xc, Par.Yc) / (tlfp.xc_err * tlfp.yc_err)
        tlfp.corr_xc_log10_s125 = param.cov(Par.Xc, Par.Log10_S125) / (tlfp.xc_err * tlfp.log10_s125_err)
        tlfp.corr_xc_beta = param.cov(Par.Xc, Par.Beta) / (tlfp.xc_err * tlfp.beta_err)
        tlfp.corr_yc_log10_s125 = param.cov(Par.Yc, Par.Log10_S125) / (tlfp.yc_err * tlfp.log10_s125_err)
        tlfp.corr_yc_beta = param.cov(Par.Yc, Par.Beta) / (tlfp.yc_err * tlfp.beta_err)
        tlfp.corr_log10_s125_beta = param.cov(Par.Log10_S125, Par.Beta) / (tlfp.log10_s125_err * tlfp.beta_err)

        from icecube.icetray import I3Frame
        frame = I3Frame()
        frame["tlfp"] = tlfp
        param2 = I3LaputopParams.from_frame(frame, "tlfp")
        self.assertNotEqual(id(param), id(param2))
        self.assertEqual(str(param), str(param2))


class TestThetaPhi(unittest.TestCase):

    def check(self, theta, phi, cov_tt, cov_tp, cov_pp):
        # do a covariance round-trip
        st = sin(theta)
        ct = cos(theta)
        sp = sin(phi)
        cp = cos(phi)

        x = st * cp
        dxt = ct * cp 
        dxp = -st * sp
        y = st * sp
        dyt = ct * sp
        dyp = st * cp

        cov_xx = (dxt ** 2 * cov_tt +
                  dxp ** 2 * cov_pp +
                  2.0 * dxt * dxp * cov_tp)
        cov_yy = (dyt ** 2 * cov_tt +
                  dyp ** 2 * cov_pp +
                  2.0 * dyt * dyp * cov_tp)
        cov_xy = (dxt * dyt * cov_tt +
                  dxp * dyp * cov_pp +
                  (dxt * dyp + dxp * dyt) * cov_tp)

        obj = I3LaputopParams()
        obj.set_value(Par.Nx, x)
        obj.set_value(Par.Ny, y)
        obj.set_cov(Par.Nx, Par.Nx, cov_xx)
        obj.set_cov(Par.Nx, Par.Ny, cov_xy)
        obj.set_cov(Par.Ny, Par.Ny, cov_yy)
        # check obvious
        self.assertAlmostEqual(obj.value(Par.Nx), x)
        self.assertAlmostEqual(obj.value(Par.Ny), y)
        self.assertAlmostEqual(obj.cov(Par.Nx, Par.Nx), cov_xx)
        self.assertAlmostEqual(obj.cov(Par.Nx, Par.Ny), cov_xy)
        self.assertAlmostEqual(obj.cov(Par.Ny, Par.Nx), cov_xy)
        self.assertAlmostEqual(obj.cov(Par.Ny, Par.Ny), cov_yy)
        # check transformed
        self.assertAlmostEqual(obj.value(Par.Theta), theta)
        self.assertAlmostEqual(obj.value(Par.Phi), phi)
        self.assertAlmostEqual(obj.cov(Par.Theta, Par.Theta), cov_tt, places=6)
        self.assertAlmostEqual(obj.cov(Par.Theta, Par.Phi), cov_tp, places=6)
        self.assertAlmostEqual(obj.cov(Par.Phi, Par.Theta), cov_tp, places=6)
        self.assertAlmostEqual(obj.cov(Par.Phi, Par.Phi), cov_pp, places=6)

    def test_degenerated(self):
        obj = I3LaputopParams()
        obj.set_value(Par.Nx, 0.0)
        obj.set_value(Par.Ny, 0.0)
        obj.set_cov(Par.Nx, Par.Nx, 0.01)
        obj.set_cov(Par.Nx, Par.Ny, 0.02)
        obj.set_cov(Par.Ny, Par.Ny, 0.03)
        self.assertAlmostEqual(obj.value(Par.Theta), 0.0)
        self.assertAlmostEqual(obj.value(Par.Phi), 0.0)
        self.assertAlmostEqual(obj.cov(Par.Theta, Par.Theta), 0.0)
        self.assertAlmostEqual(obj.cov(Par.Theta, Par.Phi), 0.0)
        self.assertAlmostEqual(obj.cov(Par.Phi, Par.Theta), 0.0)
        self.assertAlmostEqual(obj.cov(Par.Phi, Par.Phi), 0.0)

    def test_1(self):
        self.check(0.1, 0.0, 0.001, 0.002, 0.003)

    def test_2(self):
        self.check(0.1, 0.5 * pi, 0.001, 0.002, 0.003)

    def test_3(self):
        self.check(0.1, pi, 0.001, 0.002, 0.003)

    def test_4(self):
        self.check(0.1, -0.5 * pi, 0.001, 0.002, 0.003)

    def test_5(self):
        self.check(1.570796, 0.0, 0.001, 0.002, 0.003)

    def test_6(self):
        self.check(1.570796, 0.5 * pi, 0.001, 0.002, 0.003)

    def test_7(self):
        self.check(1.570796, pi, 0.001, 0.002, 0.003)

    def test_8(self):
        self.check(1.570796, -0.5 * pi, 0.001, 0.002, 0.003)


class TestAngularResolution(unittest.TestCase):

    def test_near_vertical(self):
        cxx = 0.01
        cxy = 0.02
        cyy = 0.03
        obj = I3LaputopParams()
        obj.set_value(Par.Nx, 0.0)
        obj.set_value(Par.Ny, 0.0)
        obj.set_cov(Par.Nx, Par.Nx, cxx)
        obj.set_cov(Par.Nx, Par.Ny, cxy)
        obj.set_cov(Par.Ny, Par.Ny, cyy)
        ar = obj.angular_resolution
        self.assertAlmostEqual(ar, 0.21, delta=0.01)

    def test_standard(self):
        obj = I3LaputopParams()
        cxx = 0.01
        cxy = 0.02
        cyy = 0.03
        obj.set_cov(Par.Nx, Par.Nx, cxx)
        obj.set_cov(Par.Nx, Par.Ny, cxy)
        obj.set_cov(Par.Ny, Par.Ny, cyy)
        obj.set_value(Par.Nx, 0.9)
        obj.set_value(Par.Ny, 0.0)
        ar = obj.angular_resolution
        self.assertAlmostEqual(ar, 0.26, delta=0.01)

        cxx = 0.03
        cxy = 0.02
        cyy = 0.01
        obj.set_cov(Par.Nx, Par.Nx, cxx)
        obj.set_cov(Par.Nx, Par.Ny, cxy)
        obj.set_cov(Par.Ny, Par.Ny, cyy)
        obj.set_value(Par.Nx, 0.0)
        obj.set_value(Par.Ny, 0.9)
        ar = obj.angular_resolution
        self.assertAlmostEqual(ar, 0.26, delta=0.01)


if __name__ == "__main__":
    unittest.main()
