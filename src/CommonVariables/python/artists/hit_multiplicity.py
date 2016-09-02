from icecube.shovelart import PyArtist, PyQFont, Scenario
from icecube.common_variables import hit_multiplicity

class I3HitMultiplicityValues(PyArtist):

    def description(self):
        return 'CommonVariables HitMultiplicity'

    requiredTypes = [ hit_multiplicity.I3HitMultiplicityValues ]

    def __init__(self):
        PyArtist.__init__(self)
        self.defineSettings( { 'Show key': True, 'Font': PyQFont(), 'size':12 } )

    def create( self, frame, output ):
        cvm = frame[ list(self.keys())[0] ]
        string = "nHitStrings: %d\n"%(cvm.n_hit_strings)\
               + "nHitDOMs: %d\n"%(cvm.n_hit_doms)\
               + "nHitDOMs_OnePulse: %d\n"%(cvm.n_hit_doms_one_pulse)\
               + "nPulses: %d\n"%(cvm.n_pulses)
        font = self.setting('Font')
        font.pointSize = self.setting('size')
        if( self.setting('Show key') ):
            string = list(self.keys())[0] + "::\n" + string

        output.addTextOverlay( string, font )

Scenario.registerArtist(I3HitMultiplicityValues)
