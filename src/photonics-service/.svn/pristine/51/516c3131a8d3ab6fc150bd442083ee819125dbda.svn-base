# 
# photorec_test.py
#
# Used to test if changes to photonics effect the output of photorec reconstruction
# Performs Photorec Energy Estimator on simulatio no see if there are any changes
# Performs energy estimation for four ice models: AHAv1ice AHA07v2h2 SPICE1 SPICEMie
#
# Outputs gzipped text file which contains five columns: The originl energy estimation
# and one column for each ice model. One line per event.


import gzip,glob
from os.path import expandvars
from I3Tray import *
from icecube import icetray,photonics_service,dataio,dataclasses,cramer_rao,paraboloid,finiteReco

ice_models = ["AHAv1ice","AHA07v2h2","SPICE1","SPICEMie"] 
codebase = expandvars("$I3_BUILD").split('/')[-2]
outfile = gzip.open("photorec_test_"+codebase+".txt.gz",'w',compresslevel=9)

load("photorec-llh")

tray = I3Tray()

tray.AddModule("I3Reader","reader",
               FilenameList = [expandvars("$I3_TESTDATA/sim/GeoCalibDetectorStatus_IC79.55380_corrected.i3.gz")] +
               sorted(glob.glob("/data/icecube02/users/kjmeagher/sim/6308/Level3a_IC79_nugen_numu.006308.*.i3.bz2"))
               )

for model in ice_models:
    print(model)
    PhotonicsTopLevelDirectory  = "/opt/photorec/"+model
    PhotonicsLevel2DriverFile = { "AHAv1ice"  : "AHAv1ice_photorec_cascreco_z80_a20_level2_showers.list",
                                  "AHA07v2h2" : "AHA07v2h2_photorec_mureco_z80_a20_level2_showers.list",
                                  "SPICE1"    : "mu_photorec.list",
                                  "SPICEMie"  : "level2_table_DEGEN.list",
                                  }[model]
    ServiceName = "PhotonicsService_"+model
    OutputName = "MPEFitPhotorecEnergy_"+model

    tray.AddService( 'I3PhotonicsServiceFactory', ServiceName ,
                     PhotonicsTopLevelDirectory = PhotonicsTopLevelDirectory,
                     DriverFileDirectory        = PhotonicsTopLevelDirectory,
                     PhotonicsLevel2DriverFile  = PhotonicsLevel2DriverFile,
                     PhotonicsTableSelection    = 2 ,
                     ServiceName                = ServiceName,
                     )

    tray.AddModule( "I3PhotorecEnergyEstimator", "MPEFitPhotorecEnergy_" + model,
                    RecoPulsesName         = "OfflinePulses" ,
                    RecoParticleName       = "MPEFit_SLC",
                    ResultParticleName     = OutputName,
                    I3PhotonicsServiceName = ServiceName,
                    MaxDistance            = 450.0 * I3Units.m ,
                    GeoMode                = "AllOMs" ,
                    HypothesisType         = "Lightsaber" ,
                    ConstEventLength       = 1000.0 * I3Units.ns,
                    NoiseRate              = 0.0 * I3Units.hertz ,
    )

def f(frame):
    try:
        s = " ".join([str(frame["MPEFitPhotorecEnergy_"+m+'_dEdX'].value) for m in  ["SLC_Spice1"]+ice_models])
        outfile.write(s+'\n')
    except KeyError:
        print(frame)

tray.AddModule(f,"f")

tray.Execute()
tray.Finish()
