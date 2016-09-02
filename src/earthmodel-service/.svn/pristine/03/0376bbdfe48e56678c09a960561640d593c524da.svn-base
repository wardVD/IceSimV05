#!/usr/bin/env python

from I3Tray import *
from icecube import icetray, dataclasses, dataio, earthmodel_service 

import numpy as np
import matplotlib.pyplot as plt

# get service
service = earthmodel_service.EarthModelService("EarthModelService","",
                                               ["PREM_mmc"], [], "IceSheet", 
                                               20.0*I3Units.degree, 1948*I3Units.m);

params = service.get_earth_params_list()

def rescale_param(params, scale) :
    for index, param in enumerate(params) :
        #print "original param", param
        params[index] = scale * param
        #print "mod param", params[index]


def rescale(earth_params, r1, r2, coreedge, outfname) :
    f = open(outfname, 'w')
    for earth_param in earth_params:
        # copy earthparam 
        param = earthmodel_service.EarthParam(earth_param)

        #print earth_param.print_density()

        cur_r = param.upper_radius
        boundary = param.boundary_name

        # scale core edge
        if (boundary == "coremantle_boundary") :
            param.upper_radius = cur_r * coreedge 

        # scale density
        if (boundary == "innercore_boundary" or boundary == "coremantle_boundary") :
            rescale_param(param.params, r1)
        elif (boundary == "lowermantle_boundary") :
            rescale_param(param.params, r2)
            
        if (boundary != "atmo_radius") :
            f.write(param.print_density() + "\n")

    # dirty fix
    # add prem part
    f.write("6373934 clearice_boundary     ICE         1    0.921585   # 200m below ice surface, 1.005 x 0.917 \n")
    f.write("6374134 iceair_boundary       ICE         1    0.762944   # surface of ice, 0.832 x 0.917 \n")
    f.write("6478000 atmo_radius           AIR         1    0.000811   # 0.673atm x 1.205e-3(g/cm3 for 1atm) \n")
    f.close()


erange = "3:7"
nebins = int((7 - 3)/0.05)
zenrange = "90:180"
nzenbins = 45
ncoszenbins = 100
gamma = "1.0"
nfiles = 50 
#nfiles = 30 
nevents = 1000000
zenithweight = 1.0
mergefiles = 1
procno = 0 
#procno = 30 
xsecmodel = "csms"
datadir = ('/data/ana/EarthCore/sim/InEarth/E%s' % gamma)

import os
I3_BUILD = os.environ["I3_BUILD"]

f1 = open("run_inearth.sh", 'w')
f2 = open("run_maketable.sh", 'w')
f3 = open("combine_inearth.sh", 'w')

for line in open('fitparams.input', 'r'):
    items= line[:-1].split(' ')
    xsec = float(items[0])
    histid = int(items[1])
    filename = "PREM_mmc_R1_%1.2f_R2_%1.2f_R3_%1.2f.dat" %(float(items[2]), float(items[3]), float(items[4]))
    print filename

    # generate earthmodel files!
    rescale(params, float(items[2]), float(items[3]), float(items[4]), filename)

    # generate CONDOR submission scripts
    f1.write("sh submit_AllScripts.sh %d %d %s NuMu %d %s %s %s %2.1f %s %2.1f:%2.1f:%2.1f %s %d %d %d %s %s %d\n" %
        (histid, nfiles, datadir, nevents, gamma, erange, zenrange, zenithweight, xsecmodel, xsec,xsec,xsec, filename, nzenbins, ncoszenbins, nebins, erange, zenrange, procno))
   
    # generate script to make tables
    picklesdir = ('%s/%d/Z%d_C%d_E%d' % (datadir,histid, nzenbins, ncoszenbins, nebins))
    f2.write("sh submit_MakeInEarthPropWeightTable.sh %d %d %d %d %s %s %s/%d %s %d\n" % 
             (histid, nzenbins, ncoszenbins, nebins, erange, zenrange, datadir, histid, picklesdir, mergefiles))

    # generate script to combine pickles 
    f3.write("python %s/neutrino-generator/resources/util/CombineInEarthPropWeightTable.py %s/%d_Nu %s/%d_*_Nu.pickles\n" % 
             (I3_BUILD, picklesdir, histid, picklesdir, histid))
    f3.write("python %s/neutrino-generator/resources/util/CombineInEarthPropWeightTable.py %s/%d_AntiNu %s/%d_*_Nu.pickles\n" % 
             (I3_BUILD, picklesdir, histid, picklesdir, histid))
 
    # move earthmodel files under densities directry
    os.rename(filename, "../earthparams/densities/%s" % filename)


# move run scripts to NuGen's condor_submit dir
os.rename("run_inearth.sh", "../../../neutrino-generator/resources/util/condor_submit/run_inearth.sh")
os.rename("run_maketable.sh", "../../../neutrino-generator/resources/util/condor_submit/run_maketable.sh")
os.rename("combine_inearth.sh", "../../../neutrino-generator/resources/util/condor_submit/combine_inearth.sh")


