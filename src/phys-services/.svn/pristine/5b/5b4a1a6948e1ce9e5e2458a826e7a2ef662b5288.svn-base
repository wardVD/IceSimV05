#!/usr/bin/env python

"""
author: mzoll <marcel.zoll@fysik.su.se>
please report any errors and possible improvements to the author

script to read and process information in the plain-tabulated Goodrun-list in the new format:
format is #(RunNum/Good_i3/Good_it/LiveTime(s)/ActiveStrings/ActiveDoms/ActiveInIceDoms/OutDir/Comment(s))

use this like this:
  from goodrunlist import *
  g = good_run_list()
  g.add_good_run_list('/data/exp/IceCube/2011/filtered/level2/IC86_2011_GoodRunInfo.txt')
  g.exclude_runs([1,2,3]) #remove some unwanted runs
  g.exclude_runs("/home/user/IC86-I_exclude.txt") #alternatively found in a tabulated file
  for i in g.runs(): #remove all runs which have no good in_ice
    if not g.get_run_info(i).inice_ok:
      g.pop(i)

or load the standart-set of goodrun-files:
  import goodrunlist
  g = goodrunlist.GRL()
  ...
"""

import os, sys, glob

class run_info():
  """
  holds information for each run
  check obj.run_id if the object is healty
  """
  #___________________________________________________________________________
  def __init__(self):
    self.run_id = -1
    self.detector = ''
    self.livetime_sec = -1
    self.inice_ok = -1
    self.icetop_ok = -1
    self.active_strings = -1
    self.active_doms = -1
    self.active_inice = -1
    self.path = ''
    self.comment = ''
  #___________________________________________________________________________
  def get_date(self):
    """ extract the date of this run from the folder it is present in """
    if (self.run_id!=-1 and self.path!=''):
      try:
        d = self.path.split("/")
        d.pop(0) #zeroth place is a supposibly "" in an absolute path
        if d[-1]=='':
          d.pop(-1)
        return (int(d[3]),int(d[6][:2]),int(d[6][2:])) #format (yyyy,mm,dd)
      except:
        raise ValueError("unexpected path format")
    return (-1,-1,-1)
  #___________________________________________________________________________
  def n_files(self):
    """ retrieve the number of Parts/Subruns this run produced, by peaking into the folder """
    print ("DEPRICATED: produces potentially wrong results in connection with runs with subruns not having consecutive numbers with gaps\nUse ::file_index_list() instead")
    if (self.run_id!=-1 and self.path!=''):
      try:
        files = glob.glob(os.path.join(self.path,"Level2*_IC??*_data_Run%08d_*[0-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9].i3.bz2"%(self.run_id)))
        n_files = len(files)
        return n_files
      except:
        pass
    return -1
  #___________________________________________________________________________
  def file_index_list(self):
    if (self.run_id!=-1 and self.path!=''):
      try:
        files = glob.glob(os.path.join(self.path,"Level2*_IC??*_data_Run%08d_*[0-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9].i3.bz2"%(self.run_id)))
        file_indexes = [int(f[-15:-7]) for f in files]
        file_indexes.sort()
        return file_indexes
      except:
        pass
    return -1
  #___________________________________________________________________________
  def __str__(self):
    """ dump the info from this object """
    print ("Run %d\nDetector %s\nDate %s\nLivetime(sec) %d"%(self.run_id, self.detector, "tba",self.livetime_sec))
  
  
class good_run_list():
  """ this is the object you want to operate on """
  #___________________________________________________________________________
  def __init__(self):
    self.run_info_dict = {} #format (run_id: run_info)
    self.detectors = ['IC79.2010', 'IC86.2011', 'IC86.2012', 'IC86.2013', 'IC86.2014', 'IC86.2015']
  #___________________________________________________________________________
  def add_good_run_list(self, filepath, detector, format='new_v1'):
    """ add the info from a goodrun_list file """
    if (not detector in self.detectors):
      print detector
      raise ValueError("configure a proper detector from this list : %s"%(str(self.detectors)))

    infile = open(filepath, 'r')
    lines = infile.readlines()
    if (format=='new_v1'):
      #skip the first two lines
      for thisline in lines[2:]:
        thiswords = thisline.split(" ")
        thiswords = [v for v in thiswords if v not in ['','\n'] ]
        try:
          ri = run_info()
          ri.detector = detector
          ri.run_id = int(thiswords[0])
          ri.inice_ok = bool(thiswords[1])
          ri.icetop_ok = bool(thiswords[2])
          ri.livetime_sec = float(thiswords[3])
          ri.active_strings = int(thiswords[4])
          ri.active_doms = int(thiswords[5])
          ri.active_inice = int(thiswords[6])
          ri.path = thiswords[7]
          if (len(thiswords)>8):
            ri.comment = ' '.join(thiswords[8:])
          #now append it to the dict
          self.run_info_dict[ri.run_id] =ri
        except:
          pass
          #print "Unexpected file format: !!!SKIP!!!: '%s' "%(str(thiswords))
          #raise ValueError("Unexpected file format")
    elif (format=='new_v0'):
      for thisline in lines[1:]:
        thiswords = thisline.split(" ")
        thiswords = [v for v in thiswords if v not in ['','\n'] ]
        try:
          ri = run_info()
          ri.detector = detector
          ri.run_id = int(thiswords[0])
          ri.inice_ok = bool(thiswords[1])
          ri.icetop_ok = bool(thiswords[2])
          ri.livetime_sec = float(thiswords[3])
          ri.path = thiswords[4].strip('\n').strip('\t')
          if (len(thiswords)>5):
            ri.comment = ' '.join(thiswords[5:])
          #now append it to the dict
          self.run_info_dict[ri.run_id] =ri
        except:
          pass
          #print "Unexpected file format: !!!SKIP!!!: '%s' "%(str(thiswords))
          #raise ValueError("Unexpected file format")
    else:
      raise ValueError("Unrecognized format prescription of the goodrunlist : %s"%(format))

    infile.close()
  #___________________________________________________________________________
  def runs(self):
    return self.run_info_dict.keys()
  #___________________________________________________________________________  
  def exclude_runs(self, arg):
    """ remove a list of runs from the goodrun-list """
    n_removed = 0
    if (isinstance(arg, list)):
      for r in arg:
        try:
          self.run_info_dict.pop(r)
          n_removed+=1
        except:
          pass
    elif (isinstance(arg,str)):
      try:
        infile = open(arg, 'r')
        lines = infile.readlines()
        for thisline in lines:
          thisword = thisline.strip('\n')
          r = int(thisword)
          if r in self.run_info_dict.keys():
            self.run_info_dict.pop(r)
            n_removed+=1
          else:
            #print("to be excluded run %d is not in goodrunlist"%(r))
            pass
      except:
        pass
    else:
      raise ValueError("Unknown function argument")
    print("Removed %d runs"%(n_removed))
  #___________________________________________________________________________  
  def get_run_info(self,run_id):
    """ retrieve the info for a specific specific run """
    if run_id in self.run_info_dict.keys():
      return self.run_info_dict[run_id]
    else:
      print "ATTENTION: run not found in goodrunlist!"
      return run_info()

    

def GRL():
  """ load the standard set of Good runs that are in the data warehouse"""
  g = good_run_list()
  g.add_good_run_list(os.path.join("/data/exp/IceCube/2010/filtered/level2/IC79_GRLists/IC79_GRL_NewFormat.txt"), 'IC79.2010', 'new_v0')
  g.add_good_run_list(os.path.join("/data/exp/IceCube/2011/filtered/level2/IC86_2011_GoodRunInfo.txt"), 'IC86.2011')
  g.add_good_run_list(os.path.join("/data/exp/IceCube/2012/filtered/level2/IC86_2012_GoodRunInfo.txt"), 'IC86.2012')
  g.add_good_run_list(os.path.join("/data/exp/IceCube/2013/filtered/level2/IC86_2013_GoodRunInfo.txt"), 'IC86.2013')
  g.add_good_run_list(os.path.join("/data/exp/IceCube/2014/filtered/level2/IC86_2014_GoodRunInfo.txt"), 'IC86.2014')
  g.add_good_run_list(os.path.join("/data/exp/IceCube/2015/filtered/level2/IC86_2015_GoodRunInfo.txt"), 'IC86.2015')
  return g

