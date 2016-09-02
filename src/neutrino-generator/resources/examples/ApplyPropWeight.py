##########################################################
# 
# Example script to apply propatgation weight to 
# simulation files generated with simmode "FINALONLY" 
#
# The "FINALONLY" simulation starts simulation 
# near by the IceCube detector, so the "primary" neutrino
# is identical to "InIce" neutrino.
#
# Use this script to include the effect of interactions 
# inside the Earth. By default, it simulates PREM with
# no oscillation (same as FULL simulation mode)
#
# This script replaces following parameters in
# I3MCWeightDict
# see details in apply_propagation_wegihts.py
#
# Before you run the script, you have to download InEarth table.
# $ wget http://www.icecube.wisc.edu/~hoshina/propagation_weights.tgz
# $ tar -zxvf propagation_weights.tgz -C $I3_BUILD/neutrino-generator/resources 
#

from I3Tray import *
from icecube import icetray, dataclasses, dataio, phys_services

#
# for logging
#
icetray.I3Logger.global_logger = icetray.I3PrintfLogger()
icetray.set_log_level(icetray.I3LogLevel.LOG_WARN)
#icetray.set_log_level(icetray.I3LogLevel.LOG_INFO)
#icetray.set_log_level_for_unit("EarthModelService",icetray.I3LogLevel.LOG_INFO)

#----------------
# start icetray
#----------------

import sys
import glob

outfilename= sys.argv[1]

# gamma index of primary flux for inputfile, currently we support only 1.0, 1.5, 2.0
gengamma = sys.argv[2]


import glob
inputfiles = sys.argv[3:]
filelist = []
for file in inputfiles :
    if file.count("\*") > 0 :
         filelist.append(glob.glob(file))
    else :
         filelist.append(file)

print filelist

# generated energy range of input file
elogmin = 2.0
elogmax = 7.0

tray = I3Tray()

tray.AddModule('I3Reader', 'reader', FilenameList=filelist)

from update_weightdict import update_weightdict
tray.AddModule(update_weightdict, "update", ProductionTag="IC86")

from apply_propagation_weights import apply_propagation_weights
tray.AddModule(apply_propagation_weights,"applypropweights", 
               WeightFilePaths=["/data/user/hoshina/SIMULATION/trunk/src/neutrino-generator/resources/propagation_weights"],
               WeightFilePrefixes=["PREM_CSMS"],    
               ParamSuffixes=["PREM_CSMS"],    
               PrimaryGammaIndex=gengamma, 
               EnergyLogMin = elogmin, 
               EnergyLogMax = elogmax, 
               )

from icecube import tableio, hdfwriter
hdf_outputkeys = ["I3MCWeightDict"]
hdftable2 = hdfwriter.I3HDFTableService(outfilename+".h5")
tray.AddModule(tableio.I3TableWriter, "writerhdf")(
            ("tableservice",hdftable2),
            ("SubEventStreams",["in_ice"]),
            ("keys",hdf_outputkeys),
            )

tray.AddModule("I3Writer","writer")(
    ("filename", outfilename), 
    )

tray.AddModule('TrashCan','can')
tray.Execute()
tray.Finish()


