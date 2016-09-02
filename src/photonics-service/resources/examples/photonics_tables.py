from icecube import icetray, photonics_service
from icecube.icetray import I3Units
import os
import matplotlib.pyplot as plt
import numpy as np
from copy import deepcopy

PhotonicsDir = "/cvmfs/icecube.opensciencegrid.org/data/photon-tables/"
## Creating photonics serivce 
PhotonicsTopLevelDirectory = os.path.join(PhotonicsDir, "SPICEMie")
PhotonicsDriverDirectory = os.path.join(PhotonicsTopLevelDirectory, "driverfiles")

## Names of driver files
L1_drivername = "L1_showers.list"            ## Level1 is generally used for cascades
L2_drivername = "finitereco_photorec.list"   ## Level2 corresponds to track


ps =  photonics_service.I3PhotonicsTableService(
                    photonicsTopLevelDirectory=PhotonicsTopLevelDirectory, 
                    driverFileDirectory=PhotonicsDriverDirectory, 
                    level1DriverFile=L1_drivername,
                    level2DriverFile=L2_drivername, 
                    interpolMode = 255, ## default mode
                    verbosity = 0, 
                    angularSelectLow = 0*icetray.I3Units.degree,
                    angularSelectHigh = 180*icetray.I3Units.degree , 
                    zSelectLow = -2000.0,
                    zSelectHigh = 2000.0,  
                    photonicsTableSelection = 2,  ## Selecting level of the files
                    isPhotorecTable=False)

# all following calls go more or less directly to photonics. use photonics units instead.
# define the cascade
x,y,z = 0., 0., 0.          # meters
zenith, azimuth = 90., 180. # degree
energy = 2e3                # GeV
photonicsType = 0           # Track, 1 for e.m. cascade
length = 500.                 # m (set to 0 for cascades)


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
quantiles = []; time_edges = np.linspace(-25.0,3000.0,122)
for i in xrange(0,21):
    ps.SelectModuleCoordinates(0.0, 0.0, 5.0+ 5.0 * i)  # Selecting source coordinates 
    amp, dist, geotime = ps.SelectSource(src)           # Selecting source, returns amplitude (in PEs), 
                                                        # distance to emmission point and geometrical time
                                                        # 
    amplitudes.append(amp)
    distances.append(5.0+ 5.0 * i)
    quantile = ps.GetProbabilityQuantiles(time_edges,geotime, False) ### Reading time  distribution of the signal 
    quantiles.append(deepcopy(quantile*amp ))

fig = plt.figure(figsize=(8,15))
fig.add_subplot(211)
plt.plot(distances, amplitudes, lw =2 )
plt.xlabel("Distance to source [ m ]")
plt.ylabel("Expectation [ PE ] ")
plt.yscale("log")

fig.add_subplot(212)
centers = 0.5*(time_edges[1::] + time_edges[0:-1])
for i in [2,4,9, 14,19]: 
    plt.plot(centers, quantiles[i], label = "%0.2f m "%(5.0 + 5.0 * i), lw =2 )
plt.yscale("log")
plt.xlabel("Time [ ns ] ")
plt.ylabel("Expectation in bin [ PE ]")

plt.legend()
plt.show()
