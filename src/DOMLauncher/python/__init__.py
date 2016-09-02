from icecube.load_pybindings import load_pybindings
from icecube import icetray
import os
load_pybindings(__name__, __path__)
icetray.load("libDOMLauncher",False)

from icecube.DOMLauncher.domlauncher import DetectorResponse
from icecube.DOMLauncher.domlauncher import launch_splitter
