#!/usr/bin/env python

from optparse import OptionParser
from datetime import datetime
from math import floor
from icecube import icetray, dataclasses

import sys


dbserver = "dbs2.icecube.wisc.edu"
dbuser   = "www"

parser = OptionParser()
parser.add_option("-m", "--mjd", type="int", help="Modified Julian Date")
parser.add_option("-d", "--date", type="string", help="Date in Gregorian Calender, yyyy-mm-dd")
parser.add_option("-t", "--time", type="string", help="Time of day (only in combination with date option), hh:mm:ss.ssss")
parser.add_option("-r", "--run", type="int", help="Run Number")
parser.add_option("-n", "--filename", type="string",
                  help="Output file name. Defaults to 'gcdfile_(mjd,run)_#.i3.gz' or 'gcdfile_<date>.i3.gz'.")
parser.add_option("", "--fullgeometry", action="store_true", dest="full_geometry", default=False)
parser.add_option("", "--dbserver", type="string", help="Database server", default=dbserver)
parser.add_option("", "--dbuser", type="string", help="Database user", default=dbuser)
(options, args) = parser.parse_args(sys.argv)


dbserver = options.dbserver
dbuser = options.dbuser


def parse_date_time(date, time):
    datestring = options.date
    dateformat = '%Y-%m-%d'

    if not time is None:
        datestring += ' ' + options.time
        dateformat = '%Y-%m-%d %H:%M:%S.%f'
        
    dt = datetime.today()   # just a dummy
    try:
        dt = datetime.strptime(datestring, dateformat)
    except:
        print("Failed to parse date and/or time.")
        sys.exit(-1)
        
    i3time = dataclasses.make_I3Time(dt)
    return (i3time.GetModJulianDay(), i3time.GetModJulianSec(), i3time.GetModJulianNanoSec())


def get_run_start(run):
    try:
        import MySQLdb
    except:
        print("Module MySQLdb not found. This is needed to find the start time of the given run.")
        sys.exit(-1)
    i3db = MySQLdb.connect(
        host = dbserver,
        db = "I3OmDb",
        user = dbuser,
        passwd = ""
        )
    cursor = i3db.cursor()
    sqlcommand = "SELECT start FROM run_summary WHERE run=%d" % run
    cursor.execute(sqlcommand)
    row = cursor.fetchone()
    dt = datetime.today()   # a dummy
    if row is None:
        print("Run %d not found in run_summary table." % run)
        sys.exit(-1)
    else:
        dt = row[0]

    i3time = dataclasses.make_I3Time(dt)
    return (i3time.GetModJulianDay(), i3time.GetModJulianSec(), i3time.GetModJulianNanoSec())
    

if options.date is None and not options.time is None:
    print("Time option only valid in combination with date option.")
    sys.exit(-1)


mjd = 0
seconds = 0
nanoseconds = 0
gcdfile = ""

if options.mjd is None:
    if not options.date is None:
        if not options.run is None:
            print("Please do not specify both a date and a run number")
            sys.exit(-1)
        (mjd, seconds, nanoseconds) = parse_date_time(options.date, options.time)
        gcdfile = "gcdfile_" + options.date + ".i3.gz"
        print("Generating GCD-file for", options.date)
    elif not options.run is None:
        (mjd, seconds, nanoseconds) = get_run_start(options.run)
        gcdfile = "gcdfile_run_%d.i3.gz" % options.run
        print("Generating GCD-file for run %d on MJD %d" % (options.run, mjd))
    else:
        print("You need to either specify an MJD, a date (and time), or a run number!")
        sys.exit(-1)
else:
    mjd = options.mjd
    gcdfile = "gcdfile_mjd_%d.i3.gz" % mjd
    print("Generating GCD-file for MJD %d" % mjd)


if not options.filename is None:
    gcdfile = options.filename


#
# Load and run IceTray
# 

from I3Tray import *

load("libdataio")
load("libdataclasses")
load("libI3Db")
load("libsim-services")

tray = I3Tray()

tray.AddService("I3MCTimeGeneratorServiceFactory", "time-gen",
                MJD = mjd,
                MJDSeconds = seconds,
                MJDNanoSeconds = nanoseconds
                )

tray.AddService("I3DbOMKey2MBIDFactory", "OmKey2MbId",
                Host = dbserver,
                Username = dbuser,
                Database = "I3OmDb"
                )

tray.AddService("I3DbGeometryServiceFactory", "Geometry",
                Host = dbserver,
                Username = dbuser,
                Database = "I3OmDb",
                CompleteGeometry = options.full_geometry
                )

tray.AddService("I3DbCalibrationServiceFactory", "Calibration",
                Host = dbserver,
                Username = dbuser,
                Database = "I3OmDb"
                )

tray.AddService( "I3DbDetectorStatusServiceFactory", "Status",
                 Host = dbserver,
                 Username = dbuser,
                 Database = "I3OmDb"
                 )

tray.AddModule("I3Muxer", "Muxme")

tray.AddModule("I3Writer", "GcdWriter",
               FileName = gcdfile,
               Streams = [ icetray.I3Frame.Geometry,
                           icetray.I3Frame.Calibration,
                           icetray.I3Frame.DetectorStatus
                           ]
               )

tray.AddModule("TrashCan", "End")

tray.Execute(4)
tray.Finish()
