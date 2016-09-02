#!/usr/bin/env python                                                                                                                                                  
###
# A driver script to correctly compose the operations needed to process files. Conceived to run union with ClusterSubmitManager submit-scripts.
###
# For calling wimpsim-3.03/bin/wimpevent wit a earth file :
# --------------------------------------
# if ran manually:
# option 1: trust the script
#   call it specifying all options in the parser
# option 2: do not trust or modify
#   fix all member-variables of the classes wimpevent_earth_params,
#   fix all pathes in filelocation
#   take care with file operations at the bottom of the script

import sys, os, numpy, shutil
from os.path import expandvars
from optparse import OptionParser
from datetime import datetime

class wimpevent_earth_params:
  infile_earth = ''
  cc_nc_both = 3
  target_material = 1
  outfile_earth = ''
  equatorial_pos = 0
  summery_dir = 'none'
  seed = -1

class filelocation:
  workdir = ''
  indir = ''
  tmpdir = ''
  scratchdir = ''
  outdir = ''
  
parser = OptionParser()
usage = """%prog [options]"""
parser.set_usage(usage)
parser.add_option("-f", "--fileindex", action="store", type="int", default=float(numpy.nan), dest="FILEINDEX", help="process this FILEINDEX")
parser.add_option("-m", "--mass", action="store", type="int", default=float(numpy.nan), dest="MASS", help="MASS of the WIMP")
parser.add_option("-c", "--channel", action="store", type="int", default=float(numpy.nan), dest="CHANNEL", help="CHANNEL of the WIMP")
parser.add_option("-s", "--seed", action="store", type="int", default=-1, dest="SEED", help="SEED for everything")
parser.add_option("-r", "--run", action="store", type="int", default=float(numpy.nan), dest="RUN", help="RUN to process files")
parser.add_option("-w", "--workpath", action="store", default='', dest="WORKPATH", help="Path to WimpSim")
(options,args) = parser.parse_args()

### general purpose preparation
infile_name = 'wa-m%d-ch%d-earth-%06d-%d.dat' % (options.MASS, options.CHANNEL, options.RUN, options.FILEINDEX)
outfile_name = 'we-m%d-ch%d-earth-%06d-%d.dat' % (options.MASS, options.CHANNEL, options.RUN, options.FILEINDEX)
final_name = 'we-m%d-ch%d-earth.%06d.%06d.dat.gz' % (options.MASS, options.CHANNEL, options.RUN, options.FILEINDEX)

### filelocation preparation
pathes = filelocation()
pathes.workdir = options.WORKPATH
pathes.indir = ''
pathes.tmpdir = '/net/user/mzoll/npx3/tmp'
pathes.scratchdir = os.getenv('_CONDOR_SCRATCH_DIR')
pathes.outdir = '/data/sim/IceCube/2011/generated/WimpSim/Earth-INPUT/%04d'% (options.RUN)

### check parameter correctness                                                                                                                               
print(pathes.scratchdir)
if pathes.scratchdir == None:
  exit('_CONDOR_SCRATCH_DIR not set!')

### wimpann_parameter preparation
params = wimpevent_earth_params()
params.infile_earth = os.path.join(pathes.scratchdir, infile_name)
params.outfile_earth = os.path.join(pathes.scratchdir, outfile_name)
params.seed = options.SEED

#check access
if os.access(os.path.join(pathes.tmpdir, infile_name),os.R_OK) == False:
  exit('cannot access input file %s for reading!' % os.path.join(pathes.tmpdir, infile_name))
if os.access(pathes.scratchdir,os.W_OK) == False:
  exit('cannot access scratch directory %s for writing!' % pathes.scratchdir)
if os.access(pathes.tmpdir,os.W_OK) == False:
  exit('cannot access out directory %s for writing!' % pathes.outdir)

# Start collecting information about the node I'm running on.
print ("This is" +str(os.uname())+': '+str(datetime.now())) 
print ("Execution begins for WIMPEVENT: Channel %d Mass %d File %d" % (options.CHANNEL, options.MASS, options.FILEINDEX))

print ('Fetching '+infile_name)
shutil.copy(os.path.join(pathes.tmpdir, infile_name), pathes.scratchdir)
print ("Processing events")

cmdstring = '"%s\n%d\n%d\n%s\n%s\n%d"' % (params.infile_earth, params.cc_nc_both, params.target_material, params.outfile_earth, params.summery_dir, params.seed)

os.system('echo -e '+cmdstring+' | '+pathes.workdir+'/bin/wimpevent')

print("packing outfile")
os.system('gzip %s' % (params.outfile_earth))
print ("Moving final outfiles")
shutil.move(params.outfile_earth+'.gz', os.path.join(pathes.outdir, final_name))
print ("Delete files")
os.remove(params.infile_earth)
os.remove(os.path.join(pathes.tmpdir, infile_name))
print ("Finished")
