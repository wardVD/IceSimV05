from icecube.load_pybindings import load_pybindings
load_pybindings(__name__, __path__)

from icecube import icetray, dataclasses
from icecube.trigger_sim.InjectDefaultDOMSets import InjectDefaultDOMSets

@icetray.traysegment
def TriggerSim(tray,
               name,
               gcd_file,
               run_id = None,
               prune = True,
               time_shift = True,
               time_shift_args = dict(),
               filter_mode = True ):
    """
    Configure triggers according to the GCD file.  This should work
    for most seasons all the way back to IC40.

    Parameters:

    * tray - Standard for segments.
    * name - Standard for segments.
    * gcd_file - The GCD (I3File) that contains the trigger configuration.
      Note the segment figures out from the GCD file which trigger
      modules need to be loaded and configures them accordingly. 
    * prune - Whether to remove launches outside the readout windows.
      Nearly everyone will want to keep this set at True.  It makes
      simulation look more like data.
    * time_shift - Whether to time shift time-like frame objects.
      Nearly everyone will want to keep this set at True.
      It makes simulation look more like data.
    * time_shift_args - dict that's forwarded to the I3TimeShifter module.
    * filter_mode - Whether to filter frames that do not trigger.    

    This ignores AMANDA triggers and only supports the following modules:

    * SimpleMajorityTrigger
    * ClusterTrigger
    * CylinderTrigger
    * SlowMonopoleTrigger

    This segment also includes the I3GlobalTriggerSim as well as the
    I3Pruner and I3TimeShifter.  It's job is to make simulation look
    like data that the pole filters get.
    """

    if run_id == None :
        icetray.logging.log_fatal("You have to set run_id to a valid number.")

    from icecube.trigger_sim.modules.time_shifter import I3TimeShifter

    # get the trigger status
    fr = gcd_file.pop_frame()
    while "I3DetectorStatus" not in fr :
        fr = gcd_file.pop_frame()
    tsmap = fr.Get("I3DetectorStatus").trigger_status

    # loop through the trigger status map and configure what we can
    # we still need to do this for STRING and SIMPLE_MULTIPLICITY
    # triggers since there can be multiple trigger configurations
    # of this type.  With the smarter trigger-sim modules we *only*
    # need to specify the configID though since that's unique for
    # each trigger.
    for tkey, ts in tsmap :
        # Ignore the TWR/AMANDA triggers
        if tkey.source == dataclasses.I3Trigger.AMANDA_TWR_DAQ:
            continue
        if tkey.source == dataclasses.I3Trigger.AMANDA_MUON_DAQ:
            continue

        if tkey.type == dataclasses.I3Trigger.SIMPLE_MULTIPLICITY :
            suffix = "_SMT%s_%i" % (ts.trigger_settings["threshold"], tkey.config_id)
            tray.AddModule("SimpleMajorityTrigger", name + suffix ,
                           TriggerConfigID = tkey.config_id,
                           )

        if tkey.type == dataclasses.I3Trigger.STRING :
            # guard against old GCD files with ULEE configs
            if "coherenceLength" in ts.trigger_settings and \
                   "domSet" in ts.trigger_settings and \
                   "multiplicity" in ts.trigger_settings and \
                   "timeWindow" in ts.trigger_settings :
                suffix = "_string"
                tray.AddModule("ClusterTrigger", name + suffix ,
                               TriggerConfigID = tkey.config_id,
                               )
            else:
                # must be ULEE
                if "maxLength" in ts.trigger_settings and \
                       "multiplicity" in ts.trigger_settings and \
                       "string" in ts.trigger_settings and \
                       "timeWindow" in ts.trigger_settings :
                    cid = tkey.config_id
                    tray.AddModule("MultiplicityStringTrigger","ULEE_%d" % cid,
                                   TriggerConfigID = tkey.config_id,
                                   VetoDepthName = "numberOfVetoTopDoms")
                else:
                    print("Don't know how to configure this trigger %s." % str(tkey))

        if tkey.type == dataclasses.I3Trigger.VOLUME :
            tray.AddModule("CylinderTrigger", name + "_cyl")

        if tkey.type == dataclasses.I3Trigger.SLOW_PARTICLE :
            tray.AddModule("SlowMonopoleTrigger", name + "_slop")
            
    tray.AddModule("I3GlobalTriggerSim",name + "_global_trig",
                   RunID = run_id,
                   FilterMode = filter_mode)
    if prune :
        tray.AddModule("I3Pruner")

    if time_shift :
        tray.AddModule(I3TimeShifter, **time_shift_args)
                       
