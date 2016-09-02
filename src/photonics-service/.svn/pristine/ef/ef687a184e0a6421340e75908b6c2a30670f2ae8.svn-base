#########################################
# This is an example how to use photonics spline tables in I3PhotoSplineService
#########################################
from icecube import icetray, photonics_service, dataclasses
from icecube.icetray import I3Units
import os
import matplotlib.pyplot as plt
import numpy as np
from copy import deepcopy

PhotonicsDir = "/cvmfs/icecube.opensciencegrid.org/data/photon-tables/"

PhotonicsSplineDirectory = os.path.join(PhotonicsDir, "splines")



cascade_service = photonics_service.I3PhotoSplineService(
                   amplitudetable = os.path.join(PhotonicsSplineDirectory ,"LowEnergyCorrectedCascades_z20_a10_250m.abs.fits"),  ## Amplitude tables 
                   timingtable = os.path.join(PhotonicsSplineDirectory ,"LowEnergyCorrectedCascades_z20_a10_250m.prob.fits"),    ## Timing tables
                   timingSigma  = 0.0,
                   maxRadius    = 600.0 )        ## If the light sensor is further from the source than this distance - zero will be returned, default value is infinite


track_service = photonics_service.I3PhotoSplineService(
                   amplitudetable = os.path.join(PhotonicsSplineDirectory ,"ZeroLengthMieMuons_250_z20_a10.abs.fits"),  
                   timingtable    = os.path.join(PhotonicsSplineDirectory ,"ZeroLengthMieMuons_250_z20_a10.prob.fits"),   
                   timingSigma    = 0.0,
                   maxRadius      = 600.0 )  ## If the light sensor is further from the source than this distance - zero will be returned, default value is infinite


segment_length = 15.0 ##  the spline tables are done with segments for tracks 
segment_energy = 3.40 ##  segment energy (l = 15 m)

# Creating particles for sources
segment1 = dataclasses.I3Particle()
segment1.pos = dataclasses.I3Position(0.0,0.0,0.0)
segment1.dir  = dataclasses.I3Direction(1.0,0.0,0.0)
segment1.length = segment_length
segment1.energy = segment_energy
segment1.shape = segment1.shape.ContainedTrack
segment2 = deepcopy(segment1)
segment2.pos.x = segment1.pos.x + segment1.length*segment1.dir.x 
segment2.pos.y = segment1.pos.y + segment1.length*segment1.dir.y 
segment2.pos.z = segment1.pos.z + segment1.length*segment1.dir.z

# Creating sources from particles
tr_source1 = photonics_service.PhotonicsSource(segment1)
tr_source2 = photonics_service.PhotonicsSource(segment2)

# Creating particle for cascade sources
cascade = deepcopy(segment1)
cascade.shape = cascade.shape.Cascade
cascade.length = 0.0 
cascade.energy = 10.
# Creating photonics sources for cascade
cscd_source = photonics_service.PhotonicsSource(cascade)

time_edges = np.linspace(-25.0, 2500.0, 102) 

### Order of obtaining information :
# 1) Select the coordinates of OM
# 2) Select source to get expectations 
# 3) Get probability quantiles (time distribution) from previous source, normalized to 1 

# Reading photonics services
quantiles = [] ; expectations = []; distances = [] 
for i in xrange(0, 21):
    distances.append(5.0 + i*5.0)
    track_service.SelectModuleCoordinates(0.0,0.0,distances[-1]) # selecting module coordinates
    ### 
    res_tr1 = track_service.SelectSource(tr_source1) # reading amplitude information
    quantiles_tr1 = track_service.GetProbabilityQuantiles(time_edges, res_tr1[1], False)*res_tr1[0]*tr_source1.length 
    # Expectation for track tables are normalized per 1 m of the track, has to be multiplied by length
    res_tr2 = track_service.SelectSource(tr_source2) # reading amplitude information
    quantiles_tr2 = track_service.GetProbabilityQuantiles(time_edges, res_tr2[1], False)*res_tr2[0]*tr_source2.length # Expectation for track tables are normalized per 1 m of the track
    ###########
    cascade_service.SelectModuleCoordinates(0.0,0.0,distances[-1])  # Doing the same for cascade
    res_cscd = cascade_service.SelectSource(cscd_source)
    quantiles_cscd = cascade_service.GetProbabilityQuantiles(time_edges, res_tr2[1], False)*res_cscd[0]  # expectation for cascade is scaled by photonics service (iniside, no need in further multiplications)
    ####
    quantiles_total = quantiles_tr1 + quantiles_tr2 + quantiles_cscd # Getting total time distribution
    expectations.append(res_tr1[0]*tr_source1.length + res_tr2[0]*tr_source2.length + res_cscd[0]) # total expectation ,for tracks numbers are normalized per 1 meter
    quantiles.append(deepcopy(quantiles_total)) 

# plotting the results    
fig = plt.figure(figsize=(8,15))
ax1 = fig.add_subplot(211)
plt.plot(distances, expectations, lw = 2)
plt.yscale("log")
plt.xlabel(" Distance [ m ] ")
plt.ylabel(" Expectation [ PE ] ")

ax2 = fig.add_subplot(212)
centers = 0.5*(time_edges[1::] + time_edges[0:-1])
for i in [0, 2, 4, 9 , 14, 19]:
    plt.plot(centers, quantiles[i], label = "%0.2f m "%(5.0 + 5.0 * i), lw =2 )
plt.yscale("log")
plt.legend()
plt.xlabel(" Time [ ns ] ")
plt.ylabel(" Expectation in bin [ PE ] ")
plt.show()