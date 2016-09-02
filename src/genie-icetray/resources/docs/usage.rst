Usage
=====

The script in `genie-icetray/resources/scripts/GENIEGen.py` shows the basic
uses of the genie-icetray project. Besides the normal creation of a tray
and an i3 file the other necessary components are described below.

Genie needs a random number generator in order to make each of the
simulation sets different. That is implements in the line::

    randomService = phys_services.I3SPRNGRandomService(
        seed = options.SEED,
        nstreams = 10000,
        streamnum = options.RUNNUMBER)

The Icetray Q frame object is then created::

    tray.AddModule("I3InfiniteSource","streams",
                Stream=icetray.I3Frame.DAQ)

And then the actual Genie generation can be run::

    tray.AddModule("I3GENIEGenerator","genie_generator",  
        RandomService = randomService, # alternatively, this can be None and the I3RandomService can be installed using tray.AddService()  
        SplineFilename = expandvars("$I3_SRC/genie-icetray/resources/splines/splines_water_2.6.4.xml"),
        LHAPDFPath = expandvars("$I3_SRC/genie-icetray/resources/PDFsets"),
        NuEnergyMin = 10.*I3Units.GeV,
        NuEnergyMax = 200.*I3Units.GeV,
        PowerLawIndex = 2., # E^-2 spectrum
        GenVolRadius = 1200.*I3Units.m,
        GenVolLength = 2000.*I3Units.m,
        GenVolDepth = 1950.*I3Units.m,
        NeutrinoFlavor = "NuMu", # generates neutrinos and anti-neutrinos (1:1)
        MaterialDensity = 0.93*I3Units.g/I3Units.cm3, # ice density
        TargetMixIngredients = [1000080160,1000010010], # O16, H1
        TargetMixQuantities = [1,2], # H2O (O16->1x, H1->2x)
        ForceSingleProbScale = False,
        NEvents = options.NUMEVENTS)

The variables that can be taken by the I3GenieGenerator are as follows:

RandomService :
  The I3RandomService we are going to use.
  If None, the module tries to get it from the context.

SplineFilename :
  Spline XML filename for GENIE. If you do not include splines, the generation time will increase dramatically.

LHAPDFPath :
  Path to the LHAPDF data files. The directory should contain .LHgrid and .LHpdf files.

NuEnergyMin :
  Minimum neutrino energy of events generated

NuEnergyMax :
  Maximum neutrino energy of events generated

PowerLawIndex :
  Spectrum power law index (as in `E^-index`)

GenVolRadius :
  Radius of the generation volume cylinder

GenVolLength :
  Length of the generation volume cylinder

GenVolDepth :
  Depth of the generation volume (@ z==0)

NeutrinoFlavor :
  Flavor of neutrinos generated

MaterialDensity :
  Ice/water density.

TargetMixIngredients :
  List of PDG codes specifying the target material composition

TargetMixQuantities :
  List of quantities of the different atoms of your material composition

ForceSingleProbScale :
  Forces a single interaction probability scale
  
  The actual number of generated frames will be much less
  than `NEvents`, but your events will basically be usable un-weighted.

NEvents :
  Number of events generated

