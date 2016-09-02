from icecube.icetray import I3Units
from icecube.shovelart import PyArtist, PyQFont, Scenario
from icecube.common_variables import hit_statistics

class I3HitStatisticsValues(PyArtist):

    def description(self):
        return 'CommonVariables HitStatistics'

    requiredTypes = [ hit_statistics.I3HitStatisticsValues ]

    def __init__(self):
        PyArtist.__init__(self)
        self.defineSettings( { 'Show key': True, 'Font': PyQFont(), 'size':12 } )

    def create( self, frame, output ):
        cvs = frame[ list(self.keys())[0] ]
        string = "COG [m]: %.1f/%.1f/%.1f\n"%(cvs.cog.x/I3Units.m,cvs.cog.y/I3Units.m,cvs.cog.z/I3Units.m)\
               + "COGZsigma [m]: %.1f\n"%(cvs.cog_z_sigma/I3Units.m)\
               + "maxPulseTime [ns]: %.1f\n" %(cvs.max_pulse_time/I3Units.ns) \
               + "minPulseTime [ns]: %.1f\n" %(cvs.min_pulse_time/I3Units.ns) \
               + "QmaxDOMs [PE]: %.1f\n"%(cvs.q_max_doms)\
               + "QtotPulses [PE]: %.1f\n"%(cvs.q_tot_pulses)\
               + "Zmax [m]: %.1f\n"%(cvs.z_max/I3Units.m)\
               + "Zmin [m]: %.1f\n"%(cvs.z_min/I3Units.m)\
               + "Zmean [m]: %.1f\n"%(cvs.z_mean/I3Units.m)\
               + "Zsigma [m]: %.1f\n"%(cvs.z_sigma/I3Units.m)\
               + "ZTravel [m]: %.1f\n"%(cvs.z_travel/I3Units.m)

        font = self.setting('Font')
        font.pointSize = self.setting('size')
        if( self.setting('Show key') ):
            string = list(self.keys())[0] + "::\n" + string

        output.addTextOverlay( string, font )

Scenario.registerArtist(I3HitStatisticsValues)
