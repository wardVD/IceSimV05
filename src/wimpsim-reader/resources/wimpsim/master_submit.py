#!/usr/bin/env python
#
# Script to create a bunch of WimpSim on the cluster
# USAGE: EDIT all capitalized variables, specify masses channels and number of files you would like to run as tupples in RUN_THESE

import os
from os.path import expandvars

script = "submit.py"

#INPUTs : please configure
SCRIPTPATH = '/net/user/mzoll/WimpSim/WimpSimProd'
WORKPATH = '/net/user/mzoll/WimpSim/wimpsim-3.03'
RUN = 9001
MJDSTART = 55555
MJDSTOP = 55666
SEED = -1

#I want this (channel,mass,files)
RUN_THESE = [
#channel 11 (tautau)
(11,10,100),
(11,20,100),
(11,35,100),
(11,50,100),
#channel 5 (bb)
(5,35,200),
(5,50,200),
(5,100,200),
(5,250,100),
(5,500,100),
(5,1000,50),
(5,3000,20),
(5,5000,20),
#channel 8 (WW)
(8,100,50),
(8,250,20),
(8,500,20),
(8,1000,20),
(8,3000,20),
(8,5000,20),
#channel 100 (LKP)
(100,250,20),
(100,500,20),
(100,750,20),
(100,1000,20),
(100,1250,20),
(100,1500,20),
(100,3000,20)
]

print("Submitting Jobs")
for i in RUN_THESE:
  os.system('python '+SCRIPTPATH+'/'+script+' --channel %d --mass %d --files %d --mjdstart %d --mjdstop %d --run %d --seed %d --workpath %s --scriptpath %s' % (i[0], i[1], i[2], MJDSTART, MJDSTOP, RUN, SEED, WORKPATH, SCRIPTPATH))

print("Jobs submitted")
