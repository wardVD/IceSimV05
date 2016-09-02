from icecube.icetray import I3Units
from icecube.shovelart import PyArtist, PyQFont, Scenario
from icecube.common_variables import direct_hits

class I3DirectHitsValues(PyArtist):

    def description(self):
        return 'CommonVariables DirectHits'

    requiredTypes = [ direct_hits.I3DirectHitsValues ]

    def __init__(self):
        PyArtist.__init__(self)
        self.defineSettings( { 'Show key': True, 'Font': PyQFont(), 'size': 12 } )

    def create( self, frame, output ):
        cvd = frame[ list(self.keys())[0] ]
        string = "nDirDoms: %d\n"%(cvd.n_dir_doms)\
        +"nDirPulses: %d\n"%(cvd.n_dir_pulses)\
        +"nDirStrings: %d\n"%(cvd.n_dir_strings)\
        +"qDirPulses [PE]: %.1f\n"%(cvd.q_dir_pulses)\
        +"nEarlyDoms: %d\n"%(cvd.n_early_doms)\
        +"nEarlyPulses: %d\n"%(cvd.n_early_pulses)\
        +"nEarlyStrings: %d\n"%(cvd.n_early_strings)\
        +"qEarlyPulses [PE]: %.1f\n"%(cvd.q_early_pulses)\
        +"nLateDoms: %d\n"%(cvd.n_late_doms)\
        +"nLatePulses: %d\n"%(cvd.n_late_pulses)\
        +"nLateStrings: %d\n"%(cvd.n_late_strings)\
        +"qLatePulses [PE]: %.1f\n"%(cvd.q_late_pulses)\
        +"DirTrackHitDistributionSmoothness: %.1f\n"%(cvd.dir_track_hit_distribution_smoothness)\
        +"DirTrackLength [m]: %.1f\n"%(cvd.dir_track_length/I3Units.m)

        font = self.setting('Font')
        font.pointSize = self.setting('size')
        if( self.setting('Show key') ):
            string = list(self.keys())[0] + "::\n" + string

        output.addTextOverlay( string, font )

Scenario.registerArtist(I3DirectHitsValues)
