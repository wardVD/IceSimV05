from I3Tray import *
from icecube import icetray, dataio, dataclasses, phys_services, corsika_reader

tray = I3Tray()
tray.context['I3RandomService'] = phys_services.I3GSLRandomService(42)
tray.AddModule('I3CORSIKAReader', FilenameList=[os.getenv('I3_TESTDATA') + '/DAT010000'],
               Prefix=os.getenv('I3_TESTDATA') + '/sim/GeoCalibDetectorStatus_IC86.55697_corrected_V2.i3.gz',
               NEvents=1)
tray.AddModule('Dump')

def primary_info(fr):
	print(dataclasses.get_most_energetic_primary(fr['I3MCTree']))
tray.AddModule(primary_info, Streams=[icetray.I3Frame.DAQ])
tray.AddModule('I3Writer', Filename='foo.i3')

tray.Execute()
