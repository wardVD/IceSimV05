from icecube import icetray, photonics_service
from icecube.icetray import I3Units
import os
import pylab as p
import numpy as n


# setup I3PhotoSplineService with tables from cvmfs

PhotonicsDir = "/cvmfs/icecube.opensciencegrid.org/data/photon-tables/"

PhotonicsSplineDirectory = os.path.join(PhotonicsDir, "splines")

ps = photonics_service.I3PhotoSplineService(
                   amplitudetable = os.path.join(PhotonicsSplineDirectory ,"LowEnergyCorrectedCascades_z20_a10_250m.abs.fits"),  ## Amplitude tables 
                   timingtable = os.path.join(PhotonicsSplineDirectory ,"LowEnergyCorrectedCascades_z20_a10_250m.prob.fits"),    ## Timing tables
                   timingSigma  = 0.0,
                   maxRadius    = np.inf)        ## If the light sensor is further from the source than this distance - zero will be returned, default value is infinite

# all following calls go more or less directly to photonics. use photonics units instead.
# define the cascade
x,y,z = 0., 0., 0.          # meters
zenith, azimuth = 45., 180. # degree
energy = 1e5                # GeV
photonicsType = 1           # e.m. cascade
length =  0                 # m (irrelevant for cascades)
speed = 1.                  # in fractions of c 
# create source object and define module coordinates
src = photonics_service.PhotonicsSource(x,y,z,zenith,azimuth,
                                        speed,length,energy,photonicsType) # Creating photonics source, 1 in the middle - is speed
ps.SelectModuleCoordinates(-100, 0, 0) # selecting cooridnates of module

# make some manual calls to the service
for delay in [100,500, 1000,2000]:
    totamp, prob = ps.GetPhotorecInfo(delay, src) ## Getting expectation and probability dp/dt at delay time for given source
    print("delay: %5.0f meanamp: %10f dpdt: %10f" % (delay, totamp, prob))

# use numpy.vectorize to create a function that accepts arrays as arguments
dpdt = n.vectorize(lambda t : ps.GetPhotorecInfo(t, src)[1]) 

# plot the delay time pdf
delays = n.linspace(0,3000,1000)

p.ioff() # turn of interactive mode
p.plot(delays, dpdt(delays), "r.-")
p.xlabel("delay time")
p.ylabel("dp/dt")
p.show()

