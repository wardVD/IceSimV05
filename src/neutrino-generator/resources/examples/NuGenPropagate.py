#!/usr/bin/env python
#
# Example script to propagate CORSIKA neutrino to detector
#

from I3Tray import *

from os.path import expandvars

from icecube import icetray, dataclasses, phys_services, sim_services, dataio, earthmodel_service, neutrino_generator

import os
import sys

infiles = sys.argv[1] # i3 file
outfile = "NuGprop_out.i3"

#
# setup parameters
#

# number of processed evts
ev_n = 10

# Because this is atmospheric, switch off weighted propataion.
propmode = neutrino_generator.nopropweight # NoPropWeight mode

# name of cross section model.
# use 'cteq5' 'css' 'csms'
xsecmodel = "csms"

# name of earth model and material model
earthmodel = ["PREM_mmc"]
materialmodel = ["Standard"]

# ice cap setting
icecapmodel = "IceSheet"
icecapangle = 30.*I3Units.degree
detdepth = 1948.*I3Units.m

# injection type
# default is now Surface (old name : Cylinder), which is similar to MuonGun(more efficient).
# You need to set Circle explicitly if you want to reproduce old simulation.
injectionmode = neutrino_generator.circle
injectionrad = 1200.*I3Units.m
distanceEntrance = 1200*I3Units.m
distanceExit = 1200*I3Units.m
cylinderparams = [injectionrad, distanceEntrance, distanceExit]
#injectionmode = "Surface"
#detcylrad = 600.*I3Units.m
#detcyllen = 1200.*I3Units.m
#origin_x = 0.*I3Units.m
#origin_y = 0.*I3Units.m
#origin_z = 0.*I3Units.m
#cylinderparams = [detcylrad,detcyllen,origin_x,origin_y,origin_z]

#for primary gen
gammaindex = 1.0
elogmin = 1.0
elogmax = 8.0

#
# for logging
#
icetray.I3Logger.global_logger = icetray.I3PrintfLogger()
icetray.set_log_level(icetray.I3LogLevel.LOG_TRACE)

#icetray.set_log_level_for_unit("EarthModelService",icetray.I3LogLevel.LOG_TRACE)
# icetray.set_log_level_for_unit("I3NuG",icetray.I3LogLevel.LOG_TRACE)
# icetray.logging.set_level_for_unit('I3PropagatorModule', 'TRACE')

#
# generate random service
#
random = phys_services.I3SPRNGRandomService(1, 10000, 1)

#
# generate earthmodel service
#
earth = earthmodel_service.EarthModelService("EarthModelService","",
                              earthmodel, materialmodel, 
                              icecapmodel, icecapangle, detdepth)

#
# generate steering service
#
steer = neutrino_generator.Steering(earth, 
                                    neutrino_generator.full,
                                    neutrino_generator.nugen, 
                                    injectionmode)

steer.cylinder_params = cylinderparams

#
# generate interaction service
#
interaction = neutrino_generator.I3NuGInteractionInfo(random, steer, xsecmodel)
interaction.initialize()
#interaction.view_interaction_info()


#
# generate propagator
#

nugen = neutrino_generator.I3NeutrinoPropagator(random, steer, interaction)
nugen.prop_mode = propmode



# Base propagators for charged leptons
from icecube.sim_services.propagation import get_propagators
propagators = get_propagators()

for flavor in 'E', 'Mu', 'Tau':
	for ptype in '', 'Bar':
		propagators[getattr(dataclasses.I3Particle.ParticleType, 'Nu'+flavor+ptype)] = nugen

#
# create I3Tray
#
tray = I3Tray()
tray.context['I3RandomService'] = random
tray.context['EarthModel'] = earth 
tray.context['NuGSteer'] = steer

from os.path import expandvars
from icecube import corsika_reader
#tray.Add("I3CORSIKAReader", Filenamelist=infiles, prefix=expandvars("$I3_TESTDATA/sim/GeoCalibDetectorStatus_IC86.55380_corrected.i3.gz"))
tray.Add("I3Reader", Filenamelist=[expandvars('$I3_TESTDATA/sim/GeoCalibDetectorStatus_IC86.55380_corrected.i3.gz'), infiles])

tray.Add(lambda frame: len(frame['I3MCTree']) > 1, Streams=[icetray.I3Frame.DAQ])

#
# start icetray
#

tray.AddModule('I3NuGSourceSelector', 'select')(
              ('RandomService', random),
              ('SteeringName', 'NuGSteer'),
              ('EnergyBiasPowerIndex', 0.3), # 0 = no energy weight
              ('KeepDarkNeutrinos', False)
              )

tray.Add('I3PropagatorModule', PropagatorServices=propagators, 
          RandomService=random)

def printy(frame):
	print frame['I3MCTree']
tray.Add(printy, Streams=[icetray.I3Frame.DAQ])

tray.AddModule("I3Writer","writer", filename=outfile, streams=[icetray.I3Frame.DAQ])

tray.AddModule("TrashCan", "the can")

tray.Execute()
tray.Finish()

