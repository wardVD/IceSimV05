#!/usr/bin/env python
#
# pybinding load test
#
# CAUTION
# If you are trying to read old i3 files
# generated up to neutrino-generator revision 121849, 
# set format_version 0
#

import glob
import sys
if sys.version_info[0] >= 3:
    import pickle
else:
    import cPickle as pickle

import numpy as np
import math as m

from I3Tray import *
from icecube import icetray, dataclasses, dataio, phys_services

#
# for logging
#
icetray.I3Logger.global_logger = icetray.I3PrintfLogger()
icetray.set_log_level(icetray.I3LogLevel.LOG_WARN)
#icetray.set_log_level_for_unit("EarthModelService",icetray.I3LogLevel.LOG_INFO)

# Do you want to save text file?
write_text = True

outfilebase = sys.argv[1]

nargv=len(sys.argv)
iargv = 2
filenames = []
while iargv < nargv :
    file=str(sys.argv[iargv])
    if file.count("*") > 0 :
        print "glob is choosen ", file
        filelist = glob.glob(file)
        filelist.sort()
        filenames.extend(filelist)
    else :
        print file
        filenames.append(file)
    iargv = iargv+1

print "input filenames are ", filenames

# read first file

f = open(filenames[0],'r')
tables = pickle.load(f)
f.close()

# they are setup parameters.
histid    = tables['histid']

if 'flavor' in tables :
    flavor    = tables['flavor']
else :
    flavor    = 'NuMu'

if 'gengamma' in tables :
    gengamma  = tables['gengamma']
else :
    gengamma  = 1.0

if 'weightgamma' in tables :
    weightgamma = tables['weightgamma']
else :
    weightgamma = 1.0

minzen    = tables['minzen']
maxzen    = tables['maxzen']
dzen      = tables['dzen']
nzenbins  = tables['nzenbins']
mincoszen = tables['mincoszen']
maxcoszen = tables['maxcoszen']
dcoszen   = tables['dcoszen']
ncoszenbins = tables['ncoszenbins']
minElog   = tables['minElog']
maxElog   = tables['maxElog']
dElog     = tables['dElog']
nEbins    = tables['nEbins']

# they are arrays for axes.
zenith    = tables['zenith']
coszenith = tables['coszenith']
energy    = tables['energy']
energycos = tables['energycos']

# number of counts for primary and inice.
primary      = tables['primary']
primary_w    = tables['primary_w']
primarycos   = tables['primarycos']
primarycos_w = tables['primarycos_w']
inice        = tables['inice']
inice_w      = tables['inice_w']
inicecos     = tables['inicecos']
inicecos_w   = tables['inicecos_w']

# 3D counts
#count3D  = tables['count3D']
count3D_w  = tables['count3D_w']
#count3D_w_noprimw  = tables['count3D_w_noprimw']

def checkfile(fname, pname, param1, param2) :
    if param1 == param2 :
       return True
    print("File %s : Setup parameter %s doesn't match! "
          + " param1 %f, param2 %f. Exit now." %
           (fname, pname, param1, param2))
    exit(0)
 
for file in filenames[1:-1] :
    f = open(file,'r')
    tables = pickle.load(f)
    f.close()

    checkfile(file,"histid",histid,tables["histid"]) 
    checkfile(file,"flavor",flavor,tables["flavor"]) 
    checkfile(file,"gengamma",gengamma,tables["gengamma"]) 
    checkfile(file,"weightgamma",weightgamma,tables["weightgamma"]) 
    checkfile(file,"minzen",minzen,tables["minzen"]) 
    checkfile(file,"maxzen",maxzen,tables["maxzen"]) 
    checkfile(file,"dzen",dzen,tables["dzen"]) 
    checkfile(file,"mincoszen",mincoszen,tables["mincoszen"]) 
    checkfile(file,"maxcoszen",maxcoszen,tables["maxcoszen"]) 
    checkfile(file,"dcoszen",dcoszen,tables["dcoszen"]) 
    checkfile(file,"minElog",minElog,tables["minElog"]) 
    checkfile(file,"maxElog",maxElog,tables["maxElog"]) 
    checkfile(file,"dElog",dElog,tables["dElog"]) 
    
    # add data.
    # since these tables are numpy.array, simple addition should work.
    primary      += tables['primary']
    primary_w    += tables['primary_w']
    primarycos   += tables['primarycos']
    primarycos_w += tables['primarycos_w']
    inice        += tables['inice']
    inice_w      += tables['inice_w']
    inicecos     += tables['inicecos']
    inicecos_w   += tables['inicecos_w']
    #count3D      += tables['count3D']
    count3D_w    += tables['count3D_w']
    #count3D_w_noprimw    += tables['count3D_w_noprimw']

print primary_w
print primarycos_w

# final 2D weight info.
# inice_w / primary_w.
weight    = inice_w / primary_w 
weightcos = inicecos_w / primarycos_w


# write binaries in pickle
# prepare dict

dict = {"histid" : histid,
        "flavor" : flavor,
        "gengamma" : gengamma,
        "weightgamma" : weightgamma,
        "minzen" : minzen,
        "maxzen" : maxzen,
        "dzen"   : dzen,
        "nzenbins"  : nzenbins,
        "mincoszen" : mincoszen,
        "maxcoszen" : maxcoszen,
        "dcoszen"   : dcoszen,
        "ncoszenbins" : ncoszenbins,
        "minElog"   : minElog,
        "maxElog"   : maxElog,
        "dElog"     : dElog,
        "nEbins"    : nEbins,
        "zenith"    : zenith,     # xaxis np.array zenith
        "coszenith" : coszenith,  # xaxis np.array coszen
        "energy"    : energy,     # yaxis np.array log10(energy) 
        "energycos" : energycos,  # yaxis np.array log10(energy) (for coszen-energy plot)
        "weight"    : weight,     # z value, ratio, np.array, weight in zenith-energy array
        "weightcos" : weightcos,  # z value, ratio, np.array, weight in coszen-energy array
        "primary"      : primary,       # primary count np.array in zenith-energy array
        "primary_w"    : primary_w,     # primary weight np.array in zenith-energy array
        "primarycos"   : primarycos,    # primary count np.array in coszen-energy array
        "primarycos_w" : primarycos_w,  # primary weight np.array in coszen-energy array
        "inice"        : inice,       # inice count np.array in zenith-energy array
        "inice_w"      : inice_w,     # inice weight np.array in zenith-energy array
        "inicecos"     : inicecos,    # inice count np.array in coszen-energy array
        "inicecos_w"   : inicecos_w,  # inice weight np.array in coszen-energy array
        #"count3D"      : count3D,  # inice weight np.array in coszen-energy array
        "count3D_w"    : count3D_w,  # inice weight np.array in coszen-energy array
        #"count3D_w_noprimw"   : count3D_w_noprimw,  # inice weight np.array in coszen-energy array
        }

f = open(outfilebase+".pickles", 'w')
pickle.dump(dict, f)
f.close()

if not write_text :
    exit(0)

# save the data to text file

np.savetxt(outfilebase+"_weight.txt",weight)
np.savetxt(outfilebase+"_zenith.txt",zenith)
np.savetxt(outfilebase+"_energy.txt",energy)
np.savetxt(outfilebase+"_weightcos.txt",weightcos)
np.savetxt(outfilebase+"_coszenith.txt",coszenith)
np.savetxt(outfilebase+"_energycos.txt",energycos)


