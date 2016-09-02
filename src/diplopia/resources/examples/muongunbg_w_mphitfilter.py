import icecube
from icecube import icetray, dataio, phys_services, diplopia, MuonGun
from icecube.simprod import segments
from optparse import OptionParser

from I3Tray import *

usage = "usage: %prog [options]"
parser = OptionParser(usage)
parser.add_option("-o", "--outfile",default="merged.i3", dest="OUTFILE", help="Write output to OUTFILE (.i3{.gz} format)")
parser.add_option("-g", "--gcdfile",dest="GCDFILE", help="Input Geometry Calibration Detector status (.i3{.gz} format)")
parser.add_option("-n", "--numevents",default=100, dest="NUMEVENTS", help="Number of primary events to generate")
parser.add_option("-f", "--flavor",default="NuMu", dest="FLAVOR", help="Neutrino flavor to generate")
parser.add_option("--seed",type="int",default=12344, dest="SEED", help="Initial seed for the random number generator")
parser.add_option("--from-energy",type="float",default=10., dest="FROMENERGY", help="Energy range lower bound")
parser.add_option("--to-energy",type="float",default=1.0e4, dest="TOENERGY", help="Energy range lower bound")
parser.add_option("--inputfile",dest="INPUTFILE", help="pregenerated input events")

(options,args) = parser.parse_args()
if len(args) != 0:
        crap = "Got undefined options:"
        for a in args:
                crap += a
                crap += " "
        parser.error(crap)


tray = I3Tray()

gcdfile = options.GCDFILE
inputfile = options.INPUTFILE

randomService = phys_services.I3SPRNGRandomService(options.SEED,nstreams=2,streamnum=0)
randomServiceForPropagators = phys_services.I3SPRNGRandomService(options.SEED,nstreams=2,streamnum=1)

tray.context["I3RandomService"] = randomService
tray.context["I3RandomServiceForPropagators"] = randomServiceForPropagators

tray.AddService("I3XMLSummaryServiceFactory","summary", outputfilename="muongunbg.xml")

# Default: use Hoerandel as a template for generating muons.
model = icecube.MuonGun.load_model("Hoerandel5_atmod12_SIBYLL")

# Generate only single muons, no bundles.
model.flux.max_multiplicity = 1

gamma_index=2.
energy_offset=700.
energy_min=1e4
energy_max=1e7
cylinder_length=1600.
cylinder_radius=800.
cylinder_x=0.
cylinder_y=0.
cylinder_z=0.
inner_cylinder_length=500.
inner_cylinder_radius=150.
inner_cylinder_x=46.3
inner_cylinder_y=-34.9
inner_cylinder_z=-300.



# Default: cylinder aligned with z-axis at detector center as injection
# surface.
outsurface_center = icecube.dataclasses.I3Position(
        cylinder_x*icecube.icetray.I3Units.m,
        cylinder_y*icecube.icetray.I3Units.m,
        cylinder_z*icecube.icetray.I3Units.m)

outsurface = icecube.MuonGun.Cylinder(
        length=cylinder_length*icecube.icetray.I3Units.m,
        radius=cylinder_radius*icecube.icetray.I3Units.m,
        center=outsurface_center)

# Draw energies from a power law with offset.
spectrum = icecube.MuonGun.OffsetPowerLaw(
        gamma=gamma_index,
        offset=energy_offset*icecube.icetray.I3Units.GeV,
        min=energy_min*icecube.icetray.I3Units.GeV,
        max=energy_max*icecube.icetray.I3Units.GeV)

generator = icecube.MuonGun.StaticSurfaceInjector(
            outsurface, model.flux, spectrum, model.radius)

background = diplopia.MuonGunBackgroundService()
background.set_generator(generator)
background.set_rng(randomService)
background.set_rate(5.0*I3Units.kilohertz)
background.set_mctree_name("I3MCTree")


#background = diplopia.CoincidentI3ReaderService()
#background.open(options.BACKGROUNDFILE)


tray.AddModule("I3Reader","reader",filenamelist=[gcdfile,inputfile])
tray.AddModule("Delete","del", keys=["I3MCTree","MMCTrackList"])
tray.AddModule("Rename","rename", keys=["I3MCTree_preMuonProp","I3MCTree"])


tray.AddModule("PoissonMerger","merge",
       CoincidentEventService = background,
)

tray.AddModule("Rename",keys=["I3MCTree","I3MCTree_preMuonProp"])
tray.AddSegment(segments.PropagateMuons, "PropagateMuons",
    RandomService = randomServiceForPropagators)


#os.putenv("CUDA_VISIBLE_DEVICES",str(self.gpu))
#os.putenv("COMPUTE",":0."+str(self.gpu))
#os.putenv("GPU_DEVICE_ORDINAL",str(self.gpu))
from icecube.simprod import segments

tray.AddSegment(segments.PropagatePhotons, "normalpes",
            RandomService = "I3RandomService",
            HybridMode = False,
            IgnoreMuons = False,
            IgnoreCascades = False,
            UseGPUs = True,
            UseAllCPUCores = False,
            InputMCTree = "I3MCTree",
            UseGeant4 = False)

tray.AddModule("Dump")

tray.AddModule("MPHitFilter","hitfilter", 
   HitOMThreshold=1,
   WeightMap="CorsikaWeightMap"
 )


tray.AddModule("I3Writer","write",Filename=options.OUTFILE)

tray.Execute(1003)
tray.Finish()

"""
"""
#for e in range(100):
#    print background.get_next_frame()
