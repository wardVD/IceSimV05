.. _CommonVariables__direct_hits:

Direct Hits
===========

.. automodule:: icecube.common_variables.direct_hits

The I3DirectHitsTimeWindow class
--------------------------------

.. autoclass:: I3DirectHitsTimeWindow

   .. automethod:: __init__

   **Class Properties**

   .. autoattribute:: time_0
   .. autoattribute:: time_1

   **Class Methods**

   .. automethod:: is_time_after_time_window
   .. automethod:: is_time_before_time_window
   .. automethod:: is_time_inside_time_window

The I3DirectHitsDefinition class
--------------------------------

.. autoclass:: I3DirectHitsDefinition

    **Class Properties**

    .. autoattribute:: name
    .. autoattribute:: time_window

The I3DirectHitsDefinitionSeries class
--------------------------------------

.. autoclass:: I3DirectHitsDefinitionSeries

The I3DirectHitsValues class
----------------------------

.. autoclass:: I3DirectHitsValues

    .. automethod:: __init__

    **Class Properties**

    .. autoattribute:: n_dir_strings
    .. autoattribute:: n_dir_doms
    .. autoattribute:: n_dir_pulses
    .. autoattribute:: q_dir_pulses
    .. autoattribute:: n_early_strings
    .. autoattribute:: n_early_doms
    .. autoattribute:: n_early_pulses
    .. autoattribute:: q_early_pulses
    .. autoattribute:: n_late_strings
    .. autoattribute:: n_late_doms
    .. autoattribute:: n_late_pulses
    .. autoattribute:: q_late_pulses
    .. autoattribute:: dir_track_length
    .. autoattribute:: dir_track_hit_distribution_smoothness

The I3DirectHitsCalculator icetray module
-----------------------------------------

.. autoclass:: I3DirectHitsCalculator

    **Module Parameters**:

    .. autoattribute:: DirectHitsDefinitionSeries
    .. autoattribute:: PulseSeriesMapName
    .. autoattribute:: ParticleName
    .. autoattribute:: OutputI3DirectHitsValuesBaseName
    .. autoattribute:: PyLogLevel

tableio converters
------------------

This section lists the existing *tableio* converters for the direct_hits sub
project.

The I3DirectHitsValuesConverter
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. autoclass:: I3DirectHitsValuesConverter

Available traysegments
----------------------

.. autofunction:: I3DirectHitsCalculatorSegment

.. autofunction:: I3DirectHitsValuesBookerSegment

Utility functions
-----------------

This section lists the utility functions, which are defined to calculate the
direct hits.

Utility function ``CalculateDirectHits``
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

**C++ Definition 1**::

    I3DirectHitsValuesMapPtr
    common_variables::direct_hits::
    CalculateDirectHits(
        const I3DirectHitsDefinitionSeries& dhDefinitions,
        const I3Geometry&                   geometry,
        const I3RecoPulseSeriesMap&         pulsesMap,
        const I3Particle&                   particle
    );

Calculates the direct hits for the given time windows (given
through the :py:class:`I3DirectHitsDefinitionSeries` object, the given
I3Geometry, the given I3RecoPulseSeriesMap and the given I3Particle.

**C++ Definition 2**::

    I3DirectHitsValuesMapPtr
    common_variables::direct_hits::
    CalculateDirectHits(
        const I3Geometry&           geometry,
        const I3RecoPulseSeriesMap& pulsesMap,
        const I3Particle&           particle
    );

Calculates the direct hits for the default direct hits time windows
(defined through the :py:func:`get_default_definitions` function), the given
I3Geometry, the given I3RecoPulseSeriesMap and the given I3Particle.

**Python bindings**:

.. autofunction:: calculate_direct_hits

Utility function ``GetDefaultDefinitions``
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

**C++ Definition**::

    I3DirectHitsDefinitionSeries
    common_variables::direct_hits::
    GetDefaultDefinitions();

Gets an I3DirectHitsDefinitionSeries object with the default direct hits
definitions (e.g. time windows). For the definition of that list see the
description of the python-bindings of this function below!

**Python bindings**:

.. autofunction:: get_default_definitions

Utility function ``GetDirectHitsPulseMap``
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

**C++ Definition**::

    I3RecoPulseSeriesMapPtr
    common_variables::direct_hits::
    GetDirectHitsPulseMap(
        const I3DirectHitsDefinition dhDefinition,
        const I3Geometry&            geometry,
        const I3RecoPulseSeriesMap&  pulsesMap,
        const I3Particle&            particle,
        bool                         allPulsesOfDirectDoms = false
    );

**Python bindings**:

.. autofunction:: get_direct_hits_pulse_map

Utility function ``GetDirectPulsesTimeResiduals``
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

**C++ Definition**::

    I3VectorDoublePtr
    common_variables::direct_hits::
    GetDirectPulsesTimeResiduals(
        const I3DirectHitsDefinition dhDefinition,
        const I3Geometry&            geometry,
        const I3RecoPulseSeriesMap&  pulsesMap,
        const I3Particle&            particle
    );

**Python bindings**:

.. autofunction:: get_direct_pulses_time_residuals

Examples
--------

This section should give some examples how to calculate direct hits for
different use-cases.

Calculating direct hits for the default IceCube direct hits definitions
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

To calculate the direct hits for the default direct hits definitions for a given
I3RecoPulseSeriesMap and a given I3Particle using the
:py:func:`calculate_direct_hits` utility function within your Python script,
you could do::

    from icecube.common_variables import direct_hits

    geometry   = frame['I3Geometry']
    pulses_map = frame['my_I3RecoPulseSeriesMap']
    particle   = frame['my_I3Particle']

    direct_hits_map = direct_hits.calculate_direct_hits(geometry, pulses_map, particle)

    print "NDirA:", direct_hits_map['A'].n_dir_doms
    print "LDirA:", direct_hits_map['A'].dir_track_length
    print "SDirA:", direct_hits_map['A'].dir_track_hit_distribution_smoothness

    print "NDirB:", direct_hits_map['B'].n_dir_doms
    ...

A full script example can be found in the file
``$I3_SRC/CommonVariables/resources/examples/direct_hits/calculate_direct_hits.py``.

Calculating direct hits using specific direct hits definitions
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

To calculate direct hits for a list of specific direct hits definitions, e.g.
direct hits time windows, for a given I3RecoPulseSeriesMap and a given
I3Particle using the
:py:func:`calculate_direct_hits` utility function within your Python script,
you could do::

    from icecube.icetray import I3Units
    from icecube.common_variables import direct_hits

    geometry   = frame['I3Geometry']
    pulses_map = frame['my_I3RecoPulseSeriesMap']
    particle   = frame['my_I3Particle']

    dh_definitions = [
        direct_hits.I3DirectHitsDefinition("my_def_1", -10*I3Units.ns, +10*I3Units.ns),
        direct_hits.I3DirectHitsDefinition("my_def_2", -15*I3Units.ns, +70*I3Units.ns),
    ]

    direct_hits_map = direct_hits.calculate_direct_hits(dh_definitions, geometry, pulses_map, particle)

    for key in direct_hits_map.keys():
        print 'direct hits "%s":'%(key), direct_hits_map[key]

A full script example can be found in the file
``$I3_SRC/CommonVariables/resources/examples/direct_hits/calculate_direct_hits.py``.

Using the I3DirectHitsCalculatorSegment traysegment
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Using the
:func:`I3DirectHitsCalculatorSegment` icetray traysegment to calculate and to
book direct hits is the preferred and easiest way of doing it. A full script
example can be found in the file
``$I3_SRC/CommonVariables/resources/examples/direct_hits/I3DirectHitsCalculatorSegment.py``.
The following code snippet illustrates the core points::

    ...
    from icecube import hdfwriter
    from icecube.common_variables import direct_hits

    pulses_map_name    = 'MaskedOfflinePulses'
    reco_particle_name = 'MPEFit_SLC'

    dh_defs = direct_hits.default_definitions

    tableio_keys_to_book = []

    tableio_keys_to_book +=\
    tray.AddSegment(direct_hits.I3DirectHitsCalculatorSegment, 'dh',
        DirectHitsDefinitionSeries       = dh_defs,
        PulseSeriesMapName               = pulses_map_name,
        ParticleName                     = reco_particle_name,
        OutputI3DirectHitsValuesBaseName = reco_particle_name+'DirectHits',
        BookIt                           = True
    )

    tray.AddSegment(hdfwriter.I3HDFWriter, 'hdfwriter',
        Keys            = tableio_keys_to_book,
        SubEventStreams = ['nullsplit'],
        Output          = 'my_data.hdf'
    )
    ...

The key point here is that the *BookIt* keyword argument of the traysegment has
been set to ``True``, so it returns a list of tableio converter keys, that can
then be passed to a table writer (the hdfwriter.I3HDFWriter in the case here).