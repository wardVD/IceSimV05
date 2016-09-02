from matplotlib.backends.backend_qt4agg import new_figure_manager
from PyQt4 import QtCore

from .AggMPLArtist import AggMPLArtist
from icecube.shovelart import ChoiceSetting

class QtMPLArtist( AggMPLArtist ):
    '''
    A PyArtist representing a matplotlib figure, which can be shown
    and explored in a separate Qt window if PyQt4 is available
    '''

    def __init__( self ):
        AggMPLArtist.__init__(self)
        self.addSetting( "Display", ChoiceSetting( "In scene;In own window", 0 ) )

    def get_figure_manager( self, fignum ):
        manager = new_figure_manager( fignum )
        # Allow the user to close the window and have it reopen later
        manager.window.setAttribute(QtCore.Qt.WA_DeleteOnClose, False)
        return manager

    def get_canvas_update( self, figure ):
        '''Return the update action that fires whenever the time changes'''
        def impl( time ):
            self.update_plot( time )
            self.manager.canvas.draw()
        if self.is_animated and self.setting("Animated") and self.setting("Display"):
            return impl
        else:
            return None

    def create_output(self, output):
        '''Export this artist's output to OpenGL and/or the dedicated MPL window'''

        if self.setting("Display") == 1:
            self.manager.show()
        else:
            AggMPLArtist.create_output( self, output )

    def autosize(self, fig, output):
        '''Adjust the size of the figure and axes as best as possible'''

        if self.setting("Display") == 0:
            AggMPLArtist.autosize(self, fig, output)
