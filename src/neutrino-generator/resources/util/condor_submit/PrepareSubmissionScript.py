#!/usr/bin/env python
#
# This script generates condor submission scripts to generate
# InEarth propagation tables using NuGen's "InEarth" simulation mode.
# The "InEarth" mode skips final interaction in NuGen.
#
# This propagation tables will be used with "FINALONLY" NuGen dataset
# that skips all InEarth propagation and perform final interaction only.
# 
# HOW TO USE
#
# 1) set variables.
# 2) run this script
#    python PrepareSubmissionScript.py
#    then three .sh files are generated.
# 3) login to submit node (NPX in UW), and set I3_BUILD(source env.sh)
# 4) run inearth.sh 
# 5) run combine_inearth.sh 
#  
# if you need to re-generate propagation tables from generated i3files,
# run run_maketable.sh.
#

# datasetid, you may define any integer 
datasetid = 100100100

# input neutrino flavor. If "NuMu" then both NuMu and NuMuBar are generated.
flavor = "NuMu"

# generation gamma index. 1.0 is recommended to get 
# enough statistics for all energy - zenith range.
gengamma = "1.0"

# set output data dir.
datadir = ('/data/ana/EarthCore/sim/InEarth/prod_test/%s/E%s' % (flavor, gengamma))

# log10 energy range, separated with :
erange = "2:7"

# number of energy bins for table
nebins = int((7 - 2)/0.05)

# zenith range in degree, separated with :
zenrange = "90:180"

# number of zenith bins and coszen bins
nzenbins = int((180 - 90))
ncoszenbins = 100

# Target generation index for "FINALONLY" NuGen simulation.
# The "InEarth" simulation is generated with E^-1, but 
# can be used to make propataion tables for "FINALONLY" dataset
# generated with E^-1 or a softer gamma index. 
# You may generate multiple tables for each gamma index (of FINALONLY dataset).
# This example generates three different tables for E^-1, E^-1.5, and E^-2.0.
weightgamma= "1.0:1.5:2.0"

# cross section model
xsecmodel = "csms"

# cross section global scale parameter
# do not change it unless you want to simulate modified cross section value
xsec = 1.0

# EarthModel file name. 
earthmodel = "PREM_mmc.dat"

# number of generation files.
nfiles = 100

# number of injected neutrinos per file
nevents = 1000000

# procno is used to set starting file number and random seed.
# if you want to add more files, set next file number.
procno = 0 

# do not touch following parameters.
zenithweight = 1.0
mergefiles = 1

#---- end of setting part --------------------------------

# generate CONDOR submission scripts
f1 = open("run_inearth.sh", 'w')
f1.write("sh submit_AllScripts.sh %d %d %s %s %d %s %s %s %s %2.1f %s %2.1f:%2.1f:%2.1f %s %d %d %d %s %s %d\n" %
(datasetid, nfiles, datadir, flavor, nevents, gengamma, weightgamma, erange, zenrange, zenithweight, xsecmodel, xsec,xsec,xsec, earthmodel, nzenbins, ncoszenbins, nebins, erange, zenrange, procno))

# generate script to make tables only
# you don't need it if you run 'run_inearth.sh" script.
# if you want to re-generated tables(saved in pickles file) from i3file, use run_maketable.sh.
f2 = open("run_maketable.sh", 'w')
f3 = open("combine_inearth.sh", 'w')

wgammalist = weightgamma.split(":")
for wgamma in wgammalist :
        picklesdir = ('%s/%d/Z%d_C%d_E%d' % (datadir,datasetid, nzenbins, ncoszenbins, nebins))
        curpicklesdir = ('%s/W%s' % (picklesdir, wgamma))
        f2.write("sh submit_MakeInEarthPropWeightTable.sh %s %d %d %d %d %s %s %s %s %s/%d %s %d\n" % 
             (flavor, datasetid, nzenbins, ncoszenbins, nebins, erange, zenrange, gengamma, wgamma, datadir, datasetid, curpicklesdir,  mergefiles))

        # generate script to combine pickles.
        f3.write("sh submit_combine_inearth.sh %s %d_W%s_%s %d_*_%s.pickles\n" % 
             (curpicklesdir, datasetid, wgamma, flavor,  datasetid, flavor))
        f3.write("sh submit_combine_inearth.sh %s %d_W%s_%sBar %d_*_%sBar.pickles\n" % 
             (curpicklesdir, datasetid, wgamma, flavor,  datasetid, flavor))



