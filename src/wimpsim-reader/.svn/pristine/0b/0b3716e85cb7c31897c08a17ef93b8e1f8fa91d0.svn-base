#!/usr/bin/env python

"""
Move trough a list of files and search for the WimpWeightMap-elements:
['DeliveredWeightMap']
  totwmap=frame['TotalWeightMap']

 written out by the I3WimpSim-Reader and sum up the weight in the respective fields
"""

import os, sys, glob
from icecube import icetray, dataio, dataclasses
from optparse import OptionParser

from I3Tray import *

parser = OptionParser()
usage = """%prog [options]"""
parser.set_usage(usage)
parser.add_option("-i", "--input", action="store", type="string", default="", dest="INPUT", help="List of i3 files with wildcards to process")

(options,args) = parser.parse_args()

infileList = glob.glob(options.INPUT)
infileList.sort()

############################## peak in back #######################
print ("moving through fileList with %d elements" % len(infileList))

delwmap = dataclasses.I3MapStringDouble()
totwmap = dataclasses.I3MapStringDouble()

del_nu_weight = 0.
del_had_weight = 0.
del_nc_weight = 0.
del_lep_weight = 0.
del_e_weight = 0.
del_mu_weight = 0.
del_tau_weight = 0.


tot_nu_weight = 0.
tot_had_weight = 0.
tot_nc_weight = 0.
tot_lep_weight = 0.
tot_e_weight = 0.
tot_mu_weight = 0.
tot_tau_weight = 0.


for infile in infileList:

  print ("opening %s" % infile)
  i3file = dataio.I3File(infile)

  while i3file.more():
    frame = i3file.pop_frame()
    if frame.Has('DeliveredWeightMap') and frame.Has('TotalWeightMap'):
      break
  
  delwmap=frame['DeliveredWeightMap']
  totwmap=frame['TotalWeightMap']
  
  del_nu_weight += delwmap["nu_weight"]
  del_had_weight += delwmap["had_weight"]
  del_nc_weight += delwmap["nc_weight"]
  del_lep_weight += delwmap["lep_weight"] 
  del_e_weight += delwmap["e_weight"]
  del_mu_weight += delwmap["mu_weight"]
  del_tau_weight += delwmap["tau_weight"]
  
  tot_nu_weight += totwmap["nu_weight"]
  tot_had_weight += totwmap["had_weight"]
  tot_nc_weight += totwmap["nc_weight"]
  tot_lep_weight += totwmap["lep_weight"]
  tot_e_weight += totwmap["e_weight"]
  tot_mu_weight += totwmap["mu_weight"]
  tot_tau_weight += totwmap["tau_weight"]

  i3file.close()
  
icetray.logging.log_notice("Delivered weight in %d elements" % len(infileList))
icetray.logging.log_notice("del_nu_weight:  "+str(del_nu_weight))
icetray.logging.log_notice("del_had_weight: "+str(del_had_weight))
icetray.logging.log_notice("del_nc_weight:  "+str(del_nc_weight))
icetray.logging.log_notice("del_lep_weight: "+str(del_lep_weight))
icetray.logging.log_notice("del_e_weight:   "+str(del_e_weight))
icetray.logging.log_notice("del_mu_weight:  "+str(del_mu_weight))
icetray.logging.log_notice("del_tau_weight: "+str(del_tau_weight))
icetray.logging.log_notice("----------------------------------")
icetray.logging.log_notice("Total weight in %d elements" % len(infileList))
icetray.logging.log_notice("tot_nu_weight:  "+str(tot_nu_weight))
icetray.logging.log_notice("tot_had_weight: "+str(tot_had_weight))
icetray.logging.log_notice("tot_nc_weight:  "+str(tot_nc_weight))
icetray.logging.log_notice("tot_lep_weight: "+str(tot_lep_weight))
icetray.logging.log_notice("tot_e_weight:   "+str(tot_e_weight))
icetray.logging.log_notice("tot_mu_weight:  "+str(tot_mu_weight))
icetray.logging.log_notice("tot_tau_weight: "+str(tot_tau_weight))























