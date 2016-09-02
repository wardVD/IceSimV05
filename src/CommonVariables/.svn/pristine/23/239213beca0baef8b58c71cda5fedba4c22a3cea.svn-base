.. _CommonVariables__howto_write_subproject:

How to write a sub project (some conventions)
=============================================

This chapter should give you some advise how to write a sub project for the
CommonVariables project, especially where to put your files and what they should
contain.

The name of a sub project should always be in lower case and should have the
*snake* notation, because the name will be also the name of the sub project's
Python module!

Let's consider the fictive new sub project named *my_sub_project*.

The directory and file structure of *my_sub_project* should look like so::

    CommonVariables/
        public/CommonVariables/my_sub_project/
            calculator.h                           (1a)
            I3MySubProjectValues.h                 (2a)
        private/CommonVariables/
            my_sub_project/
                calculator.cxx                     (1b)
                I3MySubProjectValues.cxx           (2b)
            pybindings/my_sub_project/             (3)
                module.cxx                         (3a)
        python/my_sub_project/
            __init__.py                            (4)
            I3MySubProjectCalculator.py            (5)
            converters.py                          (6)
                I3MySubProjectValuesConverter      (6a)
            segments.py                            (7)
                I3MySubProjectCalculatorSegment    (7a)
                I3MySubProjectValuesBookerSegment  (7b)
        resources/
            examples/my_sub_project/               (8)
            tests/my_sub_project/                  (9)
            docs/my_sub_project/
                index.rst                          (10)

The directories and files listed above have the following meaning and should
have the following content:

(1a): CommonVariables/public/CommonVariables/my_sub_project/calculator.h

    This is the C++ header file, which should contain all the definitions of the
    sub project's C++ utility functions. All C++ utility functions should be
    defined inside a *namespace*. For this sub project the following namespace
    would be appropriate::

        namespace common_variables { namespace my_sub_project {

        ... (my C++ utility function definitions) ...

        }/*my_sub_project*/}/*common_variables*/

    Your utility functions can then be accessed by other users by prefixing the
    namespace specifier ``common_variables::my_sub_project::`` to your C++
    utility functions.

(1b): CommonVariables/private/CommonVariables/my_sub_project/calculator.cxx

    This is the C++ source file, which should contain the implementations of all
    C++ utility functions defined in (1a).

(2a):

    This is the C++ header file, which should contain the definition of the sub
    project's I3FrameObject class, that holds the values of the calculation
    results.

(2b):

    This is the C++ source file, which should contain the implementation of the
    sub project's I3FrameObject class defined in (2a).

(3):

    This is the directory where the Python bindings for the C++ utility
    functions and the C++ I3FrameObject class(es) should be placed to.


        The name of the pybindings project must be made of the string
        ``common_variables__`` and the name of your sub project, e.g.
        ``my_sub_project``, in order to load the pybindings (almost)
        automatically within the sub project's Python module init file (4).
        The two underscores (``__``) after the ``common_variables`` string are
        very important here!

    Don't forget to add the line

        add_subdirectory(my_sub_project)

    to the ``$I3_SRC/CommonVariables/private/pybindings/CMakeLists.txt`` file!

(3a):

    This is the C++ implementation of the *boost::python* python bindings of
    your sub project's C++ utility functions and I3FrameObject class(es).

    It must define a Python module via the BOOST_PYTHON_MODULE macro::

        #include <boost/python.hpp>

        #include "icetray/load_project.h"

        namespace bp = boost::python;

        void register_my_sub_projectModule()
        {
            ...
        }

        BOOST_PYTHON_MODULE(my_sub_project)
        {
            load_project("CommonVariables", false);
            register_my_sub_projectModule();
        }

    The name of the Python module must be the same as the sub project's name,
    e.g. ``my_sub_project``!

    **Note:**

        The Python module and each exposed C++ class (and its class properties
        and methods!) and C++ utility function should get their appropriate
        Python docstrings here!
        This will enable you to use the *Sphinx* auto documentation module with
        its documentation directives ``.. automodule:``, ``.. autoclass:``,
        ``.. autoattribute:``, and ``.. automethod:``.

    A full source code file example can be seen for instance from the
    *direct_hits* sub project.

(4):

    This is the Python init file for the Python module of your sub project. In
    there, it needs to load the python bindungs of your C++ utility functions
    and C++ I3FrameObject class(es).

    The CommonVariables project provides an enhanced version of the
    ``icecube.load_pybindings.load_pybindings`` function, which is able to load
    also Python sub modules. You can import this enhanced version so::

        from icecube.common_variables.load_pybindings_enhanced import load_pybindings

    The Python bindungs of your sub project can then be loaded easily so::

        load_pybindings(__name__, __path__)

    After the Python bindings of your sub project has been loaded, the icetray
    modules (5), the tableio converters (6), and the icetray traysegments (7),
    that come along with your sub project, should also be imported into this
    module scope, so the *icetray-inspect* program is able to inspect your sub
    project (see also
    :ref:`CommonVariables_howto_inspect_a_subproject_with_icetrayinspect`).

    The Python module of your sub project needs to be imported into the
    ``common_variables`` Python module. This should be done by adding the line::

        from icecube.common_variables import my_sub_project

    to the ``$I3_SRC/CommonVariables/python/__init__.py`` file.

(5):

    This is the Python file that should contain the icetray module, that uses
    the utility function(s) to calculate your cut variable(s). In should have
    the string ``"Calculator"`` at the end of its name. In the case here, the
    name *I3MySubProjectCalculator* is an appropriate name.
    (See also for instance the
    :py:class:`icecube.common_variables.direct_hits.I3DirectHitsCalculator`
    icetray module for an example.)

    If the Python docstrings of the icetray module parameters are defined
    through Python class properties, like so::

        @property
        def PulseSeriesMapName(self):
            """The frame object name of the pulse series map used to calculate
            the hit multiplicity values.
            """
            pass

    those docstrings can be used also in the Sphinx HTML documentation using the
    ``.. autoattribute:`` directive. To use the same docstring also as icetray
    module parameter description, you can call the ``AddParameter()`` icetray
    module method like this::

        self.pulse_series_map_name = ""
        self.AddParameter("PulseSeriesMapName",
            I3MySubProjectCalculator.PulseSeriesMapName.__doc__,
            self.pulse_series_map_name
        )

    Doing it this way, the icetray module parameter description will be only at
    one single place, binding the source code and the documentation closer
    together.

(6):

    This Python file should contain all the *tableio* converters of the sub
    project. There should not be the need to write tableio converters in C++.
    It's just easier to write *tableio* converters in Python, so Python should
    be preferred where it is appropriate.

(6a):

    An appropriate name for a tableio converter, that converts the calculation
    result values for the *my_sub_project* sub project could be
    ``I3MySubProjectValuesConverter``. Tableio converters must have the string
    ``"Converter"`` at the end of their names!

(7), (7a), and (7b):

    This file should contain all the icetray traysegments for your sub project.
    It is wise to write at least two icetray traysegments: One for adding the
    calculator icetray module (7a) and one for booking the calculation result
    values (7b). The traysegment that adds the calculator should have a boolean
    option named ``BookIt``, so the user can specify if the calculation result
    values should also get booked using tableio. The booker traysegment (7b)
    should return a list of *tableio* converter keys, which specify what icetray
    frame objects should get booked and how (using what tableio converter) they
    should be booked. It should be called by the calculator traysegment (7a) if
    the *BookIt* parameter has been set to ``True``, and its return value (the
    list of tableio converter keys, should be returned.

(8):

    This directory should contain all example Python scripts, which demonstrates
    the usage of your sub project. It could access some test data from the
    test data directory ``$I3_TESTDATA``. So users can easily execute
    those example scripts.

(9):

    This directory should contain all test scripts and unit test scripts of
    your sub project. Such test scripts could also demonstrate that a
    implementation is at least as good as an old implementation.

(10):

    This file should contain the ReStructuredText (parsed by the Sphinx program)
    documentation of your sub project. This file needs to be included inside the
    ``$I3_SRC/CommonVariables/resources/docs/index.rst`` file by adding the
    line::

        my_sub_project/index

    to the doctree in the :ref:`CommonVariables_project_manual` section of the
    *CommonVariables* documentation index file.

    The documentation of your software is one of the most important thing your
    project needs, because without a proper documentation it is very difficult
    for users to use your project. It is much more likely that users use
    software with proper documentation. This will also save a lot of user time,
    which can than be invested into research! So, please always try to write
    proper documentation for your software!

    The documentation should make extensive use of Sphinx's Python code
    auto-documentation module by using the Sphinx documentation directives
    ``.. automodule:``, ``.. autoclass:``, ``.. automethod``,
    ``.. autoattribute:``, and ``.. autofunction:``.
    Examples how to use them can be seen for instance in
    the ReStructuredText documentation files of the *direct_hits* sub project.

    The sub project's documentation should consists of sections describing the
    following project parts (in that order):

    - Extra user classes.
    - Calculation result value classes.
    - Available icetray modules.
    - Available tableio converters.
    - Available C++ utility functions and their Python bindings.
    - Examples, how to use the available utility functions and the icetray
      traysegments.

Happy coding ... ;o)
