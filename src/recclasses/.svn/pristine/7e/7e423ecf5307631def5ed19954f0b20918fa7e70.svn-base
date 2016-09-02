# be nice and pull in our dependencies
from icecube import icetray, dataclasses
from icecube.load_pybindings import load_pybindings
load_pybindings(__name__,__path__)

try:
    import icecube.tableio
    from . import converters
    # mix pyconverters into converters
    import pyconverters as pyc
    import inspect
    for key in dir(pyc):
        thing = getattr(pyc, key)
        if inspect.isclass(thing) and issubclass(thing, icecube.tableio.I3Converter):
            setattr(converters, key, thing)
except ImportError:
    raise
