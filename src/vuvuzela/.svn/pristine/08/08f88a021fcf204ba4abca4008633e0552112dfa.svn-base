# -*- coding: utf-8 -*-
from icecube.load_pybindings import load_pybindings
load_pybindings(__name__, __path__)

from os.path import expandvars
from icecube import icetray
from icecube.icetray import I3Units

@icetray.traysegment
def AddNoise(tray, name,
	     InputName ="I3MCPESeriesMap",
	     OutputName = "",
	     RandomServiceName = "I3RandomService",
	     ExcludeList = [],
	     StartTime = -10*I3Units.microsecond,
	     EndTime = 10*I3Units.microsecond,
	     ScaleFactor = 1.0,
	     UseEventHeaderTimeWindow = False,
	     SimulateNewDOMs        = True,
	     DisableLowDTCutoff     = True,
	     If = lambda f: True):
	"""
        Module to add correlated noise. Currently uses constant parameters for each DOM, but will be updated with individual numbers soon.
	
	:param InputName: Name of the I3MCHitSeriesMap to add noise. If blank, will produce a pure noise event.
	:param OutputName: Name of the output I3MCHitSeriesMap. If blank, will replace the input hit series.
	:param RandomServiceName: The name of the I3RandomService to use for noise simulation
	:param ExcludeList: List of OMKeys to avoid sinulating.
	:param StartTime: The amount of time before the first hit to simulate noise
	:param EndTime: The amount of time after the last hit to simulate noise
	"""
                
	tray.AddModule("Inject", "AddNoiseParams_" + name,
		       InputNoiseFile = expandvars("$I3_SRC/vuvuzela/resources/data/parameters.dat"),
		       )

	tray.AddModule("Vuvuzela", "vuvuzela_" + name,
		       InputHitSeriesMapName  = InputName,
		       OutputHitSeriesMapName = OutputName,
		       StartWindow            = StartTime,
		       EndWindow              = EndTime,
		       IceTop                 = False,
		       InIce                  = True,
		       ScaleFactor            = ScaleFactor,
		       DeepCoreScaleFactor    = 1,
		       DOMsToExclude          = ExcludeList,
		       RandomService          = RandomServiceName,
		       SimulateNewDOMs        = SimulateNewDOMs,
		       DisableLowDTCutoff     = DisableLowDTCutoff,
		       UseIndividual          = True
		       )

