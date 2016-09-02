#! /usr/bin/env python
import unittest
import sys
import os
import platform
import subprocess as subp
try:
    import numpy as np
    from PyQt4.QtGui import QImage, qRed, qGreen, qBlue
except ImportError:
    pass

base_path = os.path.dirname( __file__ )

@unittest.skipIf(platform.system() == "Darwin" and 
                 os.environ["USER"] == "buildslave",
                 "suite does not work on Mac buildbots")
@unittest.skipIf(platform.system() == "Linux" and
                 "DISPLAY" not in os.environ,
                 "suite requires X-server")
class TestShovelScripts( unittest.TestCase ):

    def assertImage(self, testcase, threshold):
        'Compares a testcase with a reference image'

        def data( fn ):
            img = QImage( fn )
            d = np.zeros((img.width(), img.height(), 3))
            for i in xrange(img.width()):
                for j in xrange(img.height()):
                    raw_col = img.pixel(i, j)
                    d[i,j][0] = qRed(raw_col)
                    d[i,j][1] = qGreen(raw_col)
                    d[i,j][2] = qBlue(raw_col)
            return d

        tst = data( testcase )
        ref = data( "{0}/refs/{1}".format(base_path, testcase) )

        # check psnr, see https://en.wikipedia.org/wiki/Peak_signal-to-noise_ratio
        # perhaps this is better: https://en.wikipedia.org/wiki/Structural_similarity
        mse = np.sum( (tst - ref) ** 2 ) / np.prod( ref.shape )
        if mse == 0: # images are identical
            return
        psnr = 20 * np.log10(np.max(tst)) - 10 * np.log10(mse)
        self.assertGreater(psnr, threshold)

    def runtest( self, script ):
        p = subp.Popen(
            ("steamshovel", "--testmode", "--batch", "--noconsole", "-s",
             "{}/shovelscripts/{}.py".format(base_path, script)), 
            stdout=subp.PIPE, stderr=subp.STDOUT
        )
        output = p.communicate()[0]
        ret_code = p.returncode
        self.assertEqual( ret_code, 0,
                          "return code {0} is not zero (output below)\n{1}"
                          .format(ret_code, output) )
        self.assertTrue( "ERROR" not in output
                         and "Error" not in output
                         and "FATAL" not in output 
                         and "exception" not in output,
                         "found error messages (output below)\n" + output )

    def test_run_and_quit( self ):
        self.runtest( "run_and_quit" )

    def test_frame_reading( self ):
        self.runtest( "frame_reading" )

    def test_create_all_artists( self ):
        self.runtest( "create_all_artists" )

    @unittest.skipIf("QImage" not in globals(),
                     "test requires PyQt4")
    def test_particles_artist_draws_tracks( self ):
        name = "particles_artist_draws_tracks"
        self.runtest( name )
        self.assertImage( name + ".png", 35 )

    @unittest.skipIf("QImage" not in globals(),
                     "test requires PyQt4")
    def test_cherenkov_cone( self ):
        name = "cherenkov_cone"
        self.runtest( name )
        self.assertImage( name + ".png", 35 )

if __name__ == "__main__":
    unittest.main()
