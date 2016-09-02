#!/usr/bin/env python

from optparse import OptionParser

usage = """ %prog -g [GCD file]

This script pulls the trigger configuration
information from the input GCD file.
"""

parser = OptionParser(usage=usage)

parser.add_option("-g","--gcd",
                  dest="GCD_FILENAME",
                  help="Name of the input GCD file.")

(options, args) = parser.parse_args()


from icecube import dataclasses, dataio

f = dataio.I3File(options.GCD_FILENAME)

frame = f.pop_frame()
while f.more() and not "I3DetectorStatus" in frame:
    frame = f.pop_frame()
detector_status = frame["I3DetectorStatus"]

trigger_status_map = detector_status.trigger_status

for key, config in trigger_status_map.iteritems() :
    print("TriggerKey : %s" % str(key))
    print("  %s" % config.trigger_name)
    for name, setting in config.trigger_settings.iteritems():
        print("    %s = %s" % (name,setting))

 
    
    
