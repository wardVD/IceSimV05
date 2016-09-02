"""
$Id: load_pybindings_enhanced.py 107662 2013-06-29 14:16:35Z nwhitehorn $

Copyright (C) 2008 - 2012
Troy D. Straszheim <troy@icecube.umd.edu>
Martin Wolf <martin.wolf@icecube.wisc.edu>
and the IceCube Collaboration <http://www.icecube.wisc.edu>

version: $Revision: 107662 $
date:    $Date: 2013-06-29 10:16:35 -0400 (Sat, 29 Jun 2013) $

This python module contains functions to load IceCube C++ python bindings.

This module is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.

This module is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>
"""
import platform,sys

from icecube import icetray

if platform.system().startswith('freebsd'):
    # C++ modules are extremely fragile when loaded with RTLD_LOCAL,
    # which is what Python uses on FreeBSD by default, and maybe other
    # systems. Convince it to use RTLD_GLOBAL.

    # See thread by Abrahams et al:
    # http://mail.python.org/pipermail/python-dev/2002-May/024074.html
    sys.setdlopenflags(0x102)

#_______________________________________________________________________________
def load_pybindings(
    full_pymodule_name,
    shared_object_filename,
    extern_c_initfunc_name = None,
    verbose = False
):
    """
    Merges C++ python bindings from the shared library *shared_object_filename*
    into the python module having the name *full_pymodule_name*.

    Use when you have a directory structure like this::

      lib/
        foo.so
        foo__bar.so
        foo/
          __init__.py
          something.py
          bar/
            __init__.py
            something_else.py

    Here, inside ``foo/__init__.py`` call
    ``load_pybindings(__name__, __path__)``. This will invoke the
    initfoo() function inside the shared object file *foo.so*.

    To load pybindings also for the module foo.bar, call
    ``load_pybindings(__name__, __path__)``. This will invoke the
    initbar() function inside the shared object file *foo__bar.so*.

    Python code located in something.py and something_else.py, imported by their
    __init__.py files will not be overwritten by the C++ python bindings.

    :type  full_pymodule_name: str
    :param full_pymodule_name: The full name of the python module, e.g.
        "icecube.common_variables.direct_hits"

    :type  shared_object_filename: str | list of str
    :param shared_object_filename: The name of the shared object (.so) file.
        This name must contain the full path to the shared object file but the
        suffix ".so" does not need to be specified.

        It can also be a list of strings. In this case the first element of the
        list will be used as the name of the shared object file.
        This way, one can pass also ``__path__`` to this parameter.

        If this value is not an absolut path, the *$I3_BUILD/lib/icecube*
        directory will be added as prefix.

    :type  extern_c_initfunc_name: str | None
    :param extern_c_initfunc_name: The name of the extern C init function
        without the prefix "init". This is the name one specifies in the C++
        pybindings as the argument of the preprocessor function
        ``BOOST_PYTHON_MODULE``.

        If this parameter has been set to ``None``, the python module name after
        the last dot in *full_pymodule_name* will be used as name.

    :type  verbose: bool
    :param verbose: Switch if verbose mode should be On (``True``) or Off
        (``False``). If set to ``True`` it will print out what it loads.
    """
    import imp, os, sys

    if isinstance(shared_object_filename, list):
        shared_object_filename = shared_object_filename[0]
    if shared_object_filename[-3:] != '.so':
        shared_object_filename += '.so'
    if not os.path.isabs(shared_object_filename):
        shared_object_filename = os.path.join(os.getenv("I3_BUILD"), "lib", "icecube", shared_object_filename)

    # from the "$I3_BUILD/lib/icecube" directory on, every is flat, so we need
    # to replace the os.sep (e.g. slashes) by duble underscores (__)
    ic_base_dir = os.path.join(os.getenv("I3_BUILD"), "lib", "icecube")+os.sep
    if shared_object_filename.startswith(ic_base_dir):
        flat_shared_object_filename = shared_object_filename[len(ic_base_dir):].replace(os.sep, "__")
        shared_object_filename = os.path.join(ic_base_dir, flat_shared_object_filename)

    if extern_c_initfunc_name is None:
        extern_c_initfunc_name = full_pymodule_name.split(".")[-1]

    if verbose:
        print(('Load from shared object file "%s" the extern C init function '\
              '"init%s()", and fill the python code module "%s".'%
            (shared_object_filename, extern_c_initfunc_name, full_pymodule_name)
        ))

    thismod = sys.modules[full_pymodule_name]
    m = imp.load_dynamic(extern_c_initfunc_name, shared_object_filename)
    sys.modules[full_pymodule_name] = thismod # See load_pybindings.py

    for (k,v) in m.__dict__.items():
        if not k.startswith("_"):
            thismod.__dict__[k] = v
