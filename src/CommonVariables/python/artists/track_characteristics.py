from icecube.icetray import I3Units
from icecube.shovelart import PyArtist, PyQFont, Scenario
from icecube.common_variables import track_characteristics

class I3TrackCharacteristicsValues(PyArtist):

    def description(self):
        return 'CommonVariables TrackCharacteristics'

    requiredTypes = [ track_characteristics.I3TrackCharacteristicsValues ]

    def __init__(self):
        PyArtist.__init__(self)
        self.defineSettings( { 'Show key': True, 'Font': PyQFont(), 'size':12 } )

    def create( self, frame, output ):
        string = str( frame[ list(self.keys())[0] ] )
        cvt = frame[ list(self.keys())[0] ]
        string = "avgDOMDistQtotDOM [m]: %.1f\n"%(cvt.avg_dom_dist_q_tot_dom/I3Units.m)\
               + "EmptyHitsTrackLength [m]: %.1f\n"%(cvt.empty_hits_track_length/I3Units.m)\
               + "TrackHitsDistributionSmoothness: %.1f\n"%(cvt.track_hits_distribution_smoothness)\
               + "TrackHitsSeparationLength [m]: %.1f\n"%(cvt.track_hits_separation_length/I3Units.m)

        font = self.setting('Font')
        font.pointSize = self.setting('size')
        if( self.setting('Show key') ):
            string = list(self.keys())[0] + "::\n" + string

        output.addTextOverlay( string, font )

Scenario.registerArtist(I3TrackCharacteristicsValues)
