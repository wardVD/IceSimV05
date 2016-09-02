#!/usr/bin/env python
from optparse import OptionParser

parser = OptionParser()
parser.add_option("-s","--season",
                  dest="SEASON", default="2015" ,
                  help="Season to generate %s")

parser.add_option("-m","--mjd",
                  dest="MJD", default=None , type = "int",
                  help="Modified Julian Date")

parser.add_option("-d","--dbhost",
                  dest="DBHOST", default="dbs2.icecube.wisc.edu",
                  help="Database from which to pull the GCD.")

parser.add_option("-l","--logfile",
                  dest="LOGFILE", default="./gcd_logfile",
                  help="Name of logfile.")

(options, args) = parser.parse_args()
 
from os.path import expandvars
 
from I3Tray import I3Units
from I3Tray import I3Tray

from icecube import icetray
from icecube import dataclasses as dc
from icecube import phys_services
from icecube import dataio
from icecube import I3Db
from icecube import BadDomList
from icecube import vuvuzela
from icecube import WaveCalibrator

icetray.logging.rotating_files(options.LOGFILE)
icetray.set_log_level_for_unit('Inject', icetray.I3LogLevel.LOG_TRACE)


tray = I3Tray()

tray.AddService("I3DbOMKey2MBIDFactory","dbomkey2mbid",
    host = options.DBHOST)

tray.AddService("I3DbGeometryServiceFactory","dbgeometry",
    host = "dbs2.icecube.wisc.edu",
    CompleteGeometry = False,
    DeepCore = True,
    MJD = options.MJD)

tray.AddService("I3DbDetectorStatusServiceFactory","dbstatus",
    host = options.DBHOST,
    MJD = options.MJD)

tray.AddService("I3DbCalibrationServiceFactory","dbcalibration",
    host = options.DBHOST,
    MJD = options.MJD)

# get BadDomList from DB
xmlfile = expandvars('$I3_SRC/BadDomList/resources/scripts/QueryConfigurationIC86.xml')
tray.AddService('I3BadDomListFactory', 'BadDomListService',
                ServiceName = 'BadDomListService',
                Hostname = options.DBHOST,
                Timeout = 180,
                QFileName = xmlfile,
                )

tray.AddService('I3BadDomListFactory', 'BadDomListServiceSLC',
                ServiceName = 'BadDomListServiceSLC',
                Hostname = options.DBHOST,
                Timeout = 180,
                QFileName = xmlfile,
                )

tray.AddModule("I3InfiniteSource","streams", Stream=icetray.I3Frame.DAQ)

# I3MetaSynth needs either a driving time or an event header
def inject_header(frame):
    time = dc.I3Time()
    time.set_mod_julian_time(options.MJD, 0, 0.0)
    eh = dc.I3EventHeader()
    eh.start_time = time
    eh.end_time = time
    frame["I3EventHeader"] = eh

tray.AddModule(inject_header, 'event_header',
               streams = [icetray.I3Frame.DAQ])

tray.AddModule("I3MetaSynth","muxme")

tray.AddModule('I3BadDomListModule', 'BadDoms',
               BadDomListServiceName = 'BadDomListService',
               CleanedKeys = list(),
               BadDomsListVector = "BadDomsList",
               CleanedKeysOnly = False,
               DisabledKeysOnly = True,
               AddGoodSlcOnlyKeys  = True,
               )

tray.AddModule('I3BadDomListModule', 'BadDomsSLC',
               BadDomListServiceName = 'BadDomListServiceSLC',
               CleanedKeys = list(),
               BadDomsListVector = "BadDomsListSLC",
               CleanedKeysOnly = False,
               DisabledKeysOnly = True,
               AddGoodSlcOnlyKeys  = False,
               )

tray.AddModule("Inject", "InjectNoiseParams",
	       InputNoiseFile = \
           expandvars("$I3_SRC/vuvuzela/resources/data/parameters.dat"),
	       )

tray.AddModule("I3Writer","writer",
               streams = [icetray.I3Frame.TrayInfo, \
                          icetray.I3Frame.Geometry, \
                          icetray.I3Frame.Calibration, \
                          icetray.I3Frame.DetectorStatus],
               filename = "./gcd_snapshot.i3.gz" )

def dump(frame):
    f = open(options.LOGFILE, 'a')
    f.write("%s\n" % str(frame))
    
tray.AddModule(dump,"dump")

tray.Execute(4)
tray.Finish()
