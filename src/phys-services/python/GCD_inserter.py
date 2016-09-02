#!/usr/bin/env python

"""
author: mzoll <marcel.zoll@fysik.su.se>
please report any errors and possible improvements to the author

"""

import os, numpy, glob
from icecube import icetray, dataclasses, dataio

from icecube.phys_services import goodrunlist
class GCD_inserter(icetray.I3PacketModule):
  """
  inserts the GCD-frames before each event that is seen.
  gets the GCD-files from the goodrunlist-info.
  NOTE: can only be used for experimental data (use the 'If'-option in the tray)
  """
  def __init__(self,context):
    super(GCD_inserter, self).__init__(context, icetray.I3Frame.DAQ)
    self.ignoreTypeList = []
    self.AddParameter("IgnoreFrameTypes",
                      "Ignore this list of frametypes from the read GCD file",
                      self.ignoreTypeList) 
    self.AddOutBox("OutBox")
    self.GRL = goodrunlist.GRL()

  def Configure(self):
    super(GCD_inserter, self).Configure()
    self.ignoreTypeList = self.GetParameter("IgnoreFrameTypes")
    self.last_seen_run = float('nan')

  def FramePacket(self, frames):
    eh = frames[0]["I3EventHeader"]
    if eh.run_id <= 99999:
      icetray.logging.log_fatal("Events seem not to be experimental data (run_id <= 99999)")

    if self.last_seen_run==eh.run_id:
      for frame in frames:
        self.PushFrame(frame)
      return
    icetray.logging.log_info("Event with a new run_id encountered: %d; pushing in GCD-frames!"%(eh.run_id))
    ri = self.GRL.get_run_info(eh.run_id)
    
    if ri.run_id == -1:
      icetray.logging.log_fatal"Goodrun-lists do not contain an entry for this run (%d); cannot infer GCD-file path"%(eh.run_id))

    icetray.logging.log_trace("searching GCD : %s", os.path.join(ri.path,'*%s*GCD*.i3*'%(eh.run_id)))
    gcd_path = glob.glob(os.path.join(ri.path,'*%s*GCD*.i3*'%(eh.run_id)))

    if len(gcd_path) != 1:
      icetray.logging.log_fatal("Cannot infer unique GCD-file path for run %d; Please report this error!"%(eh.run_id))
    
    #clip in the gcd-file content
    gcd_file = dataio.I3File(gcd_path[0])
    while (gcd_file.more()):
      gcd_frame = gcd_file.pop_frame()
      if gcd_frame.Stop not in self.ignoreTypeList:
        self.PushFrame(gcd_frame)

    self.last_seen_run = eh.run_id
    #push all other frames
    for frame in frames:
      self.PushFrame(frame)
    return
  
