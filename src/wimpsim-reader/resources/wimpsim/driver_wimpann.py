#!/usr/bin/python
###
# A driver script to correctly compose the operations needed to process files. Conceived to run union with ClusterSubmitManager submit-scripts.
###
# For calling wimpsim-3.03/bin/wimpann :
# --------------------------------------
# if ran manually:
# option 1: trust the script
#   call it specifying all options in the parser
# option 2: do not trust or modify
#   fix all member-variables of the classes wimpann_params,
#   fix all pathes in filelocation
#   take care with file operations at the bottom of the script 

import sys, os, numpy, shutil
from os.path import expandvars
from optparse import OptionParser
from datetime import datetime

### === DEFINITIONS ===

class wimpann_params:
  channel = 0
  mass = 0
  do_earth = 'y'
  total_annh = 0
  to_screen = 'n'
  #mixing parameters (taken from PDG 2012-06-20 [1]):
  theta_12 = 33.89
  theta_13 = 9.12
  theta_23 = 38.54
  delta = 0.0 
  delta_m2_21 = 7.50e-5
  delta_m2_31 = 2.32e-3
  # files
  outfile_sun = ''
  outfile_earth = ''
  outfile_info = ''
  splits = 0
  summary_files = 'n'
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
parser.add_option("-f", "--files", action="store", type="int", default=float(numpy.nan), dest="FILES", help="that many files")
parser.add_option("-m", "--mass", action="store", type="int", default=float(numpy.nan), dest="MASS", help="MASS of the WIMP")
parser.add_option("-c", "--channel", action="store", type="int", default=float(numpy.nan), dest="CHANNEL", help="CHANNEL of the WIMP")
parser.add_option("-s", "--seed", action="store", type="int", default=-1, dest="SEED", help="SEED for everything")
parser.add_option("-r", "--run", action="store", type="int", default=float(numpy.nan), dest="RUN", help="RUN to process files")
parser.add_option("-w", "--workpath", action="store", default='', dest="WORKPATH", help="Path to WimpSim")
(options,args) = parser.parse_args()

### === CONFIGURATION ===

### filelocation preparation
pathes = filelocation()
pathes.workdir = options.WORKPATH
pathes.indir = ''
pathes.tmpdir = '/net/user/mzoll/npx3/tmp'
pathes.scratchdir = os.getenv('_CONDOR_SCRATCH_DIR')
pathes.outdir = '/data/sim/IceCube/2011/generated/WimpSim/Sun-INPUT/%04d'% (options.RUN)

### check parameter correctness                                                                                                                               
print(pathes.scratchdir)
if pathes.scratchdir == None:
  exit('_CONDOR_SCRATCH_DIR not set!')

### wimpann_parameter preparation
params = wimpann_params()
events_per_file = 1E6

params.channel = options.CHANNEL
params.mass = options.MASS
params.total_annh = options.FILES * events_per_file
params.outfile_sun = os.path.join(pathes.scratchdir, 'wa-m%d-ch%d-sun-%06d.dat' % (options.MASS, options.CHANNEL, options.RUN))
params.outfile_earth = os.path.join(pathes.scratchdir, 'wa-m%d-ch%d-earth-%06d.dat' % (options.MASS, options.CHANNEL, options.RUN))
params.outfile_info = os.path.join(pathes.scratchdir, 'wa-m%d-ch%d-info-%06d.dat' % (options.MASS, options.CHANNEL, options.RUN))
params.splits = options.FILES
params.seed = options.SEED

###check access
if os.access(pathes.scratchdir,os.W_OK) == False:
  exit('cannot access scratch directory %s for writing!' % pathes.scratchdir)
if os.access(pathes.tmpdir,os.W_OK) == False:
  exit('cannot access temp directory %s for writing!' % pathes.tmpdir)

### === EXECUTION and FILEOPERATIONS===

# Start collecting information about the node I'm running on.
print ("This is" +str(os.uname())+': '+str(datetime.now())) 
print ("Execution begins for WIMPANN: Channel %d Mass %d Annihilations %d*1E6" % (options.CHANNEL, options.MASS, options.FILES))

print ("Processing events")

cmdstring = '"%d\n%d\n%s\n%d\n%s\n%f\n%f\n%f\n%f\n%f\n%f\n%s\n%s\n%d\n%s\n%s\n%d"' % (params.channel, params.mass, params.do_earth, params.total_annh, params.to_screen, params.theta_12, params.theta_13, params.theta_23, params.delta, params.delta_m2_21, params.delta_m2_31, params.outfile_sun, params.outfile_earth, params.splits, params.outfile_info, params.summary_files, params.seed)

os.system('echo -e '+cmdstring+' | '+pathes.workdir+'/bin/wimpann')

print ("Moving final outfiles and deleting info-files")
#each wimpann-run will create a certain number of subfiles
point_pos_sun= params.outfile_sun.rfind('.',0)
point_pos_earth= params.outfile_earth.rfind('.',0)
for i in range (1, options.FILES+1):
  final_sun = params.outfile_sun[0:point_pos_sun]+'-'+str(i)+params.outfile_sun[point_pos_sun:]
  final_earth = params.outfile_earth[0:point_pos_earth]+'-'+str(i)+params.outfile_earth[point_pos_earth:]
  shutil.move( os.path.join(pathes.scratchdir,final_sun), pathes.tmpdir)
  shutil.move( os.path.join(pathes.scratchdir,final_earth), pathes.tmpdir)
print ("Delete info-files")
os.remove( os.path.join(pathes.scratchdir, params.outfile_info))
print ("Finished")
