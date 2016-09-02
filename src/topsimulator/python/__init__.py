from icecube import icetray

from icecube.load_pybindings import load_pybindings
load_pybindings(__name__, __path__)
del load_pybindings

from icecube.topsimulator import segments

del icetray

