from icecube.shovelart import *
from icecube.wimpsim_reader import I3WimpParams

class WimpParams( PyArtist ):

    requiredTypes = [ I3WimpParams ]

    def __init__( self ):
        PyArtist.__init__(self)
        self.defineSettings( {"Font":PyQFont(), "size":12 } )

    def description( self ):
        return "I3WimpParams summary"

    def create( self, frame, output ):
        wimp_params = frame[self.keys()[0]]
        font = self.setting("Font")
        font.pointSize = self.setting("size")
        output.addTextOverlay( str(wimp_params), font )
