from icecube import icetray, dataclasses
import os, sys


class FilterMaskMaker(icetray.I3ConditionalModule):
    """
    A module to generate a FilterMask (stored at OutputMask parameter name)
    based on an input list of Filter names (expected as I3Bools in frame)
    and prescales (as input FilterConfigs list, expected as a list:
      filter_pairs = [('MuonFilter_11',1),
                ('CascadeFilter_11',1),
                ....
    Can a use provided random service, or will generate one on it's own.
    """

    def __init__(self, context):
        icetray.I3ConditionalModule.__init__(self, context)
        self.AddOutBox("OutBox")
        self.AddParameter("OutputMaskName", "Filter mask output name", "FilterMask")
        self.AddParameter("FilterConfigs", "Dictionart of filter names and prescales", None)
        self.AddParameter("RandomService", "RandomService to use with prescale calculation", None)
        
    def Configure(self):
        self.maskname = self.GetParameter("OutputMaskName")
        self.filtconfigs = self.GetParameter("FilterConfigs")
        if self.filtconfigs == None:
            icetray.logging.log_fatal("No filter configurations provided", "FilterMaskMaker")
        self.randserv = self.GetParameter("RandomService")
        if self.randserv == None:
            icetray.logging.log_info("No random service given, taking a basic one internally", "FilterMaskMaker")
            from icecube import phys_services
            self.randserv = phys_services.I3GSLRandomService(13337)

    def Physics(self,frame):
        ## A place to store ouputs
        mymask = dataclasses.I3FilterResultMap()
        ## Loop over all keys in FilterConfigs, look for bools, apply prescales and add entry to FilterMask
        for filter_name, filter_prescale in self.filtconfigs:
            myfiltobj = dataclasses.I3FilterResult()
            if frame.Has(filter_name):
                filt_decision = frame.Get(filter_name)
                # assign the bool value to condition_passed AND prescale_passed for the filt obj
                if filt_decision.value:
                   myfiltobj.condition_passed = True 
                   # calc prescale
                   if filter_prescale == 0:  #Filter off by definition
                       myfiltobj.prescale_passed = False
                   if filter_prescale == 1:  #Filter on, no prescale
                       myfiltobj.prescale_passed = True
                   if filter_prescale > 1:
                       if (self.randserv.uniform(1.0)*filter_prescale < 1.0):
                           myfiltobj.prescale_passed = True
                       else:
                           myfiltobj.prescale_passed = False
                else:
                    myfiltobj.condition_passed = False
                    myfiltobj.prescale_passed = False
                                           
            else:
                ## filter bool not in frame, didn't pass or run:
                myfiltobj.condition_passed = False
                myfiltobj.prescale_passed = False

            # store the filt obj in the mask
            mymask[filter_name] = myfiltobj

        ## Wedge your new filter mask into the frame
        frame[self.maskname] = mymask
        ## Don't forget to push frames!
        self.PushFrame(frame)

