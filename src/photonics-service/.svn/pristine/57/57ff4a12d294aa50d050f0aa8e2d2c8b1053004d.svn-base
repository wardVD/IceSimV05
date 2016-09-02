from icecube import icetray, photonics_service
from icecube.icetray import I3Units
import os
import matplotlib.pyplot as plt
import numpy as np
from copy import deepcopy

## Creating a dummy photonics service to show the usage 
## of the photonics services
dummy_service = photonics_service.I3DummyPhotonicsService()

# all following calls go more or less directly to photonics. use photonics units instead.
# define the cascade
x,y,z = 0., 0., 0.          # meters
zenith, azimuth = 0., 180. # degree
energy = 1e5                # GeV
photonicsType = 1           # e.m. cascade
length = 0                  # m (set to 0 for cascades)


## Creating photonics source
src = photonics_service.PhotonicsSource(x=x,
                                        y=y,
                                        z=z,
                                        zenith=zenith,
                                        azimuth=azimuth,
                                        speed = 1,
                                        length = length,
                                        energy = energy,
                                        type = photonicsType)
## 
### Order of obtaining information :
# 1) Select the coordinates of OM
# 2) Select source to get expectations 
# 3) Get probability quantiles

distances = []; amplitudes = []; 
quantiles = []; time_edges = np.linspace(0.0,3000.0,201)
for i in xrange(0,21):
    dummy_service.SelectModuleCoordinates(0.0, 0.0,  5.0 * i) ### Selecting source coordinates 
    amp, dist, geotime = dummy_service.SelectSource(src)      ### Selecting source, returns amplitude (in PEs), 
                                                              ### distance to emmission point and geometrical time
                                                              ### 
    amplitudes.append(amp)
    distances.append(dist)
    quantile = dummy_service.GetProbabilityQuantiles(time_edges,geotime, False) ### Reading time  distribution of the signal 
    quantiles.append(deepcopy(quantile))

fig = plt.figure(figsize=(8,16))
fig.add_subplot(211)
plt.plot(distances, amplitudes, lw =2 )
plt.xlabel("Distance to source [ m ]")
plt.ylabel("Expectation [ PE ] ")
fig.add_subplot(212)
centers = 0.5*(time_edges[1::] + time_edges[0::-1])
for i in [1,3,5,10, 15,20]: 
    plt.plot(centers, quantiles[i], label = "%0.2f m "%(5.0 * i), lw =2 )
plt.legend()
plt.show()

