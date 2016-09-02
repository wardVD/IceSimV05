.. _CommonVariables:

CommonVariables
===============

This project is maintained by Martin Wolf <martin.wolf@fysik.su.se>.

.. toctree::
   :maxdepth: 1

   release_notes

The CommonVariables project provides a collection of C++ utility functions
(with python-bindings), icetray modules, tableio converters, and icetray
traysegments for common IceCube cut variables.

The project has been started with the intension to have a better, more
ordered and more flexible implementation of the I3Cuts code, which is currently
part of the *phys-services* project.

Having one (code reviewed) project for common cut variables, which will be used
by the entire collaboration will speedup physics analyses and will ensure that
everyone uses the same (correct) code for the same cut variables.

Since each utility function is implemented in C++ and has python bindings, the
cut variables calculation codes can be used in both, C++ and Python, user
programs. (See also :ref:`section-the-projects-structure` section below.)

For a list of currently supported common IceCube cut variables, see the
documentation of the individual sub-projects of CommonVariables:

- :ref:`CommonVariables__direct_hits`
- :ref:`CommonVariables__hit_multiplicity`
- :ref:`CommonVariables__hit_statistics`
- :ref:`CommonVariables__track_characteristics`
- :ref:`CommonVariables__time_characteristics`

Dependencies
------------

The CommonVariables project depends on the following IceCube projects:

- icetray (>= V12-08-00)

- dataclasses (>= V12-08-00)

- phys-services (>= V12-08-00)

    Uses the utility functions contained inside the ``I3Calculator::``
    namespace.

- tableio (>= V12-08-00)

    For tableio converters, which book the calculation results into a data
    output file, e.g. a ROOT file or a HDF file.

.. _section-the-projects-structure:

The project's structure
-----------------------

The CommonVariables project is divided into several sub projects. Each sub
project is responsible for calculating and booking one or a set of
related-to-each-other common IceCube cut variable(s). One of these sub projects
is for instance "direct_hits" for calculating direct hits.

From the C++ point of view
^^^^^^^^^^^^^^^^^^^^^^^^^^

In C++ the entire project is contained in the *namespace*
``common_variables::``. Each sub project has its own namespace inside the
common_variables namespace. For instance the direct hits sub project has the
namespace ``direct_hits::`` and direct hits utility functions can be accessed by
prefixing the namespace ``common_variables::direct_hits::`` to those functions.

From the Python point of view
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Each C++ utility function has python-bindings, so it can be accessed also from
within a Python script. In Python the common_variables project is structured
in the same way as it is from the C++ point of view. It has a main python module
named ``common_variables`` and each sub project has its own python sub module
within this python main module. For instance utility functions of the direct
hits sub project can be accessed through the python module
``icecube.common_variables.direct_hits``.

Utility functions, classes, icetray modules, tableio converters, and
traysegments of a particular sub project are all contained inside that
particular python sub module. For instance the icetray module
*I3DirectHitsCalculator* that calculates direct hits can be accessed in Python
through ``icecube.common_variables.direct_hits.I3DirectHitsCalculator``.

Given the information above, one can draw the following Python module structure
diagram for the *direct_hits* sub project::

    common_variables
        .direct_hits
            .I3DirectHitsCalculator (an icetray module)
            .I3DirectHitsCalculatorSegment (an icetray traysegment)
            .I3DirectHitsValues (an I3FrameObject)
            .I3DirectHitsValuesConverter (a tableio converter)
            .I3DirectHitsDefinition (a simple class)
            .I3DirectHitsTimeWindow (a simple class)

.. _CommonVariables_howto_inspect_a_subproject_with_icetrayinspect:

Important Python Import note for CommonVariables
------------------------------------------------

CommonVariables can contain *optional* sub-projects, which will only get
compiled, when all of the IceCube software projects, on which they depend on,
are available in the particular meta-project. This "optional-sub-project"
feature allows the usage of CommonVariables within a very light IceCube
meta-project, like "offline-software".

In order to avoid run-time errors of your Python script, when importing the
python module of an optional sub-project, you should always **import the
CommonVariables sub-project's python module explicitly at the beginning of your
Python script**::

    from icecube.common_variables import direct_hits

Optional sub-project will not be loaded automatically, when you import the main
Python module of CommonVariables by typing:
``from icecube import common_variables``! They need to get imported
explicitly: ``from icecube.common_variables import direct_hits``. The good
thing is, when you do the sub-project import right at the beginning of your
script, the script will fail (with some usefull instructions) immediately after
it had been started, instead of somewhere in the middle of your script, if
dependencies cannot be fulfilled.

So you do not need to know what sub-projects are *static*, and what sub-projects
are *optional*, if you do the sub-project's import always explicitly.

The old and the new variable names
----------------------------------

CommonVariables renames almost all existing variables with names, which are
(hopefully) more descriptive than before. The following table gives an overview
on the old and the new variable names, and in which CommonVariables sub-project
they can be found.

+------------+-----------------------+---------------------------------------+
| Old name   | CV sub-project        | CV name                               |
+============+=======================+=======================================+
| n_dir      | direct_hits           | n_dir_doms                            |
+------------+-----------------------+---------------------------------------+
| l_dir      | direct_hits           | dir_track_length                      |
+------------+-----------------------+---------------------------------------+
| s_dir      | direct_hits           | dir_track_hit_distribution_smoothness |
+------------+-----------------------+---------------------------------------+
| n_chan     | hit_multiplicity      | n_hit_doms                            |
+------------+-----------------------+---------------------------------------+
| q_tot      | hit_statistics        | q_tot_pulses                          |
+------------+-----------------------+---------------------------------------+
| s_all      | track_characteristics | track_hits_distribution_smoothness    |
+------------+-----------------------+---------------------------------------+
| LEmpty     | track_characteristics | empty_hits_track_length               |
+------------+-----------------------+---------------------------------------+
| Separation | track_characteristics | track_hits_separation_length          |
+------------+-----------------------+---------------------------------------+

Variable definition changes
---------------------------

Over time it could be possible, that variables change their definitions. The
lists below lists these changes in order to keep track of them to explain
possible differences of their distributions between different used versions of
CommonVariables.

- track_hits_separation_length (of *track_characteristics*, rev. no. of change: 97739 (2013-01-21))

  To determine the COG of the first and last quartile of the hits, it does not
  take the individual pulses anymore. Instead it takes the time of the first
  pulse of a DOM and the integrated charge of the DOM to determine the
  quartiles.

  This change was necessary because of wavedeform, which fits pulses to a
  waveform. So an individual pulse cannot be regarded as an individual photo-
  electron. Without that change, track_hits_separation_length could behave
  differently for data and MC.

How to inspect a sub project with icetray-inspect
-------------------------------------------------

If you want to know what kind of things are included in a sub project using
the ``icetray-inspect`` program, you can do so, for instance for the
*direct_hits* sub project, by typing::

    icetray-inspect common_variables.direct_hits

into your shell.

.. _CommonVariables_project_manual:

Project Manual
--------------

.. toctree::
   :maxdepth: 3

   direct_hits/index
   hit_multiplicity/index
   hit_statistics/index
   track_characteristics/index
   time_characteristics/index
   howto_write_subproject
   todo
