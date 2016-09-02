#!/usr/bin/env python
#
# I do WimpSim-Jobs:
# -----------------
# CONFIGURE ME:
# glob_script_dir: driver scripts are found here,
# glob_log_dir : logfiles and others are written there
# specify ALL paramters in the option parsing

# import the Job Submit Manager of ClusterJobManager
from icecube.ClusterJobManager import JobSubmitManager
from os.path import expandvars
from optparse import OptionParser

import os, numpy, sys

parser = OptionParser()
usage = """%prog [options]"""
parser.set_usage(usage)
parser.add_option("-f", "--files", action="store", type="int", default=0, dest="FILES", help="number of FILES to to create, each file will contain 1E6 annihilations")
parser.add_option("-m", "--mass", action="store", type="int", default=1000, dest="MASS", help="MASS of the WIMP")
parser.add_option("-c", "--channel", action="store", type="int", default=8, dest="CHANNEL", help="CHANNEL of the WIMP")
parser.add_option("-s", "--seed", action="store", type="int", default=0, dest="SEED", help="SEED for everything (-1 for Pythia-default)")
parser.add_option("-r", "--run", action="store", type="int", default=0, dest="RUN", help="RUN number to tag files")
parser.add_option("--mjdstart", action="store", type="float", default=float(numpy.nan), dest="MJDSTART", help="START mjd of the simulation")
parser.add_option("--mjdstop", action="store", type="float", default=float(numpy.nan), dest="MJDSTOP", help="STOP mjd of the simulation")
parser.add_option("--workpath", action="store", default='', dest="WORKPATH", help="Path to WimpSim root folder")
parser.add_option("--scriptpath", action="store", default='', dest="SCRIPTPATH", help="Path to scripts root folder")
(options,args) = parser.parse_args()

### failsaves ###
if (options.FILES <=0) or (options.MASS <= 0) or (options.CHANNEL <= 0) or (options.SEED < -1) or (options.RUN < 0) or (numpy.isnan(options.MJDSTOP)) or (numpy.isnan(options.MJDSTART)) or (options.SCRIPTPATH == '') or (options.WORKPATH == ''):
  exit("missconfigured or unconfigured parameteres")
    
glob_script_dir = options.SCRIPTPATH
glob_log_dir = '/net/user/mzoll/npx3/logs'

# create a JobSubmitManager object
jsm = JobSubmitManager(
    cluster_type             = 'npx3',
    user_id                  = expandvars('$USER'),
    resources_check_interval = 120.
)

# create a task class for data processing
tc_wimpann = jsm.create_task_class(
    #---------------------------------------------------------------------------
    # base task class options
    name                  = 'wimpann',
    env_shell             = None,
    script_interpreter    = '/usr/bin/python',
    executable            = os.path.join(glob_script_dir, 'driver_wimpann.py'),
    executable_args       = [],
    executable_opts       = { '--mass' : str(options.MASS),
			      '--channel' : str(options.CHANNEL),
			      '--files' : str(options.FILES),
			      '--run' : str(options.RUN),
			      '--workpath' : str(options.WORKPATH)},
    task_index_list       = [],
    task_index_offset     = 0,
    ntasks_total          = 1,
    ntasks_per_job        = 1,
    max_njobs_in_parallel = 1000,
    job_submission_delay  = 3.,
#    log_pathfilename      = glob_log_dir + '/wa-m%d-ch%d.%06d.log' % (options.MASS, options.CHANNEL, options.RUN),
    error_pathfilename    = glob_log_dir + '/wa-m%d-ch%d.%06d.err' % (options.MASS, options.CHANNEL, options.RUN),
    output_pathfilename   = glob_log_dir + '/wa-m%d-ch%d.%06d.out' % (options.MASS, options.CHANNEL, options.RUN),
    jdl_pathfilename      = None,
    operating_system      = None,
    notification          = "Never",
    bad_cn_list           = None,
    #---------------------------------------------------------------------------
    # NPX3 specific options
    npx3_use_gpu = False
)

tc_wimpevent_sun = jsm.create_task_class(
    #---------------------------------------------------------------------------
    # base task class options
    name                  = 'wimpevent_sun',
    env_shell             = None,
    script_interpreter    = '/usr/bin/python',
    executable            = os.path.join(glob_script_dir, 'driver_wimpevent_sun.py'),
    executable_args       = [],
    executable_opts       = { '--fileindex' : '%(job_first_task_index)d',
			      '--mass' : str(options.MASS),
			      '--channel' : str(options.CHANNEL),
			      '--seed' : '%(job_first_task_index)d',
			      '--run' : str(options.RUN),
			      '--mjdstart' : str(options.MJDSTART),
			      '--mjdstop' : str(options.MJDSTOP),
                              '--workpath' : str(options.WORKPATH)},
    task_index_list       = [],
    task_index_offset     = 1,
    ntasks_total          = options.FILES,
    ntasks_per_job        = 1,
    max_njobs_in_parallel = 1000,
    job_submission_delay  = 3.,
#    log_pathfilename      = glob_log_dir + '/we-m%d-ch%d-sun.%06d.'%(options.MASS, options.CHANNEL, options.RUN)+'%(job_first_task_index)d.log',
    error_pathfilename    = glob_log_dir + '/we-m%d-ch%d-sun.%06d.'%(options.MASS, options.CHANNEL, options.RUN)+'%(job_first_task_index)d.err',
    output_pathfilename   = glob_log_dir + '/we-m%d-ch%d-sun.%06d.'%(options.MASS, options.CHANNEL, options.RUN)+'%(job_first_task_index)d.out',
    jdl_pathfilename      = None,
    operating_system      = None,
    notification          = "Never",
    bad_cn_list           = None,
    #---------------------------------------------------------------------------
    # NPX3 specific options
    npx3_use_gpu = False
  ).add_dependency(tc_wimpann, mode='task_class')

tc_wimpevent_earth = jsm.create_task_class(
    #---------------------------------------------------------------------------
    # base task class options
    name                  = 'wimpevent_earth',
    env_shell             = None,
    script_interpreter    = '/usr/bin/python',
    executable            = os.path.join(glob_script_dir, 'driver_wimpevent_earth.py'),
    executable_args       = [],
    executable_opts       = { '--fileindex' : '%(job_first_task_index)d',
			      '--mass' : str(options.MASS),
			      '--channel' : str(options.CHANNEL),
			      '--seed' :  '%(job_first_task_index)d',
			      '--run' : str(options.RUN),
                              '--workpath' : str(options.WORKPATH)},
    task_index_list       = [],
    task_index_offset     = 1,
    ntasks_total          = options.FILES,
    ntasks_per_job        = 1,
    max_njobs_in_parallel = 1000,
    job_submission_delay  = 3.,
#    log_pathfilename      = glob_log_dir + '/we-m%d-ch%d-earth.%06d.'%(options.MASS, options.CHANNEL, options.RUN)+'%(job_first_task_index)d.log',
    error_pathfilename    = glob_log_dir + '/we-m%d-ch%d-earth.%06d.'%(options.MASS, options.CHANNEL, options.RUN)+'%(job_first_task_index)d.err',
    output_pathfilename   = glob_log_dir + '/we-m%d-ch%d-earth.%06d.'%(options.MASS, options.CHANNEL, options.RUN)+'%(job_first_task_index)d.out',
    jdl_pathfilename      = None,
    operating_system      = None,
    notification          = "Never",
    bad_cn_list           = None,
    #---------------------------------------------------------------------------
    # NPX3 specific options
    npx3_use_gpu = False
  ).add_dependency(tc_wimpann, mode='task_class')

# submit the task classes to the cluster
jsm.submit(
    task_class_list = [tc_wimpann, tc_wimpevent_sun, tc_wimpevent_earth]
)
