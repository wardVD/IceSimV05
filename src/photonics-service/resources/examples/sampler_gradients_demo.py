from icecube import icetray, photonics_service, dataclasses
from icecube.icetray import I3Units
import os
import matplotlib.pyplot as plt
import numpy as np
from copy import deepcopy
from sampler_functions import sampler_demo, gradient_demo

PhotonicsDir = "/cvmfs/icecube.opensciencegrid.org/data/photon-tables/"

PhotonicsSplineDirectory = os.path.join(PhotonicsDir, "splines")

cascade_service = photonics_service.I3PhotoSplineService(
                   amplitudetable = os.path.join(PhotonicsSplineDirectory ,"LowEnergyCorrectedCascades_z20_a10_250m.abs.fits"),  ## Amplitude tables 
                   timingtable = os.path.join(PhotonicsSplineDirectory ,"LowEnergyCorrectedCascades_z20_a10_250m.prob.fits"),    ## Timing tables
                   timingSigma  = 0.0,
                   maxRadius    = np.inf)        ## If the light sensor is further from the source than this distance - zero will be returned, default value is infinite


# calling functions from sampler_demo.py 
# 
print "Running script with demos, for comments check sampler_functions.py "
sampler_demo(cascade_service) 
gradient_demo(cascade_service) 
plt.show()
