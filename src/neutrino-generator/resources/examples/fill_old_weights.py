########################################################
# Rename new names in MCWeightDict to old one
#
# If you want to use old names in I3MCWeightDict,
# add this module just before your hdfwriter (or rootwriter)
#

from icecube import icetray, dataclasses 
import math

class fill_old_weights(icetray.I3ConditionalModule):
    def __init__(self, ctx):
        icetray.I3ConditionalModule.__init__(self, ctx)
        self.AddParameter("WeightDictName","I3MCWeightDict name","I3MCWeightDict")
        self.AddOutBox("OutBox")
	
    def Configure(self):
        self.wmapname = self.GetParameter("WeightDictName")


    def DAQ(self, frame) :

        # make copy of weight map
        wmap = dataclasses.I3MapStringDouble(frame[self.wmapname])

        wmap["InjectionSurfaceR"] = wmap["CylinderRadius"]
        wmap["GeneratorVolume"] = math.pi *  wmap["CylinderRadius"]**2 * wmap["CylinderHeight"]
        wmap["TotalInteractionProbabilityWeight"] = wmap["TotalWeight"]
        wmap["TotalInteractionProbability"] = wmap["InteractionWeight"]
        wmap["TotalPropagationProbability"] = wmap["PropagationWeight"]
        wmap["TotalCrosssectionCGS"] = wmap["TotalXsectionCGS"]
        wmap["InteractionCrosssectionCGS"] = wmap["InteractionXsectionCGS"]
        wmap["InjectionAreaNormCGS"] = wmap["InjectionAreaCGS"]
        wmap["TotalDetectionLength"] = wmap["TrueActiveLengthBefore"] + wmap["TrueActiveLengthAfter"]
        wmap["RangeInMeterWaterEquiv"] = wmap["RangeInMWE"]

        frame.Delete(self.wmapname)
        frame.Put(self.wmapname, wmap)

        self.PushFrame(frame,"OutBox");

        return True



