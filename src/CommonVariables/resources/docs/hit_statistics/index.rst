.. _CommonVariables__hit_statistics:

Hit Statistics
==============

.. automodule:: icecube.common_variables.hit_statistics

The I3HitStatisticsValues class
-------------------------------

.. autoclass:: I3HitStatisticsValues

   **Class Properties**

   .. autoattribute:: cog
   .. autoattribute:: cog_z_sigma
   .. autoattribute:: min_pulse_time
   .. autoattribute:: max_pulse_time
   .. autoattribute:: q_max_doms
   .. autoattribute:: q_tot_pulses
   .. autoattribute:: z_min
   .. autoattribute:: z_max
   .. autoattribute:: z_mean
   .. autoattribute:: z_sigma
   .. autoattribute:: z_travel

The I3HitStatisticsCalculator icetray module
--------------------------------------------

.. autoclass:: I3HitStatisticsCalculator

    **Module Parameters**:

    .. autoattribute:: PulseSeriesMapName
    .. autoattribute:: OutputI3HitStatisticsValuesName
    .. autoattribute:: PyLogLevel

tableio converters
------------------

This section lists the existing *tableio* converters for the hit_statistics
sub project.

The I3HitStatisticsValuesConverter
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. autoclass:: I3HitStatisticsValuesConverter

    .. automethod:: __init__

Available traysegments
----------------------

.. autofunction:: I3HitStatisticsCalculatorSegment

.. autofunction:: I3HitStatisticsValuesBookerSegment

Utility functions
-----------------

This section lists the utility functions, which are defined to calculate the
hit statistics values.

Utility function ``CalculateHitStatistics``
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

**C++ Definition**::

    I3HitStatisticsValuesPtr
    common_variables::hit_statistics::
    CalculateHitStatistics(
        const I3Geometry&           geometry,
        const I3RecoPulseSeriesMap& pulsesMap
    );

**Python bindings**:

.. autofunction:: calculate_hit_statistics

Utility function ``CalculateCOG``
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

**C++ Definition**::

    I3PositionPtr
    common_variables::hit_statistics::
    CalculateCOG(
        const I3Geometry&           geometry,
        const I3RecoPulseSeriesMap& pulsesMap
    );

**Python bindings**:

.. autofunction:: calculate_cog

Utility function ``CalculateCOGZSigma``
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

**C++ Definition**::

    I3DoublePtr
    common_variables::hit_statistics::
    CalculateCOGZSigma(
        const I3Geometry&           geometry,
        const I3RecoPulseSeriesMap& pulsesMap
    );

**Python bindings**:

.. autofunction:: calculate_cog_z_sigma

Utility function ``CalculateMinPulseTime``
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

**C++ Definition**::

    I3DoublePtr
    common_variables::hit_statistics::
    CalculateMinPulseTime(
        const I3Geometry&           geometry,
        const I3RecoPulseSeriesMap& pulsesMap
    );

**Python bindings**:

.. autofunction:: calculate_min_pulse_time

Utility function ``CalculateMaxPulseTime``
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

**C++ Definition**::

    I3DoublePtr
    common_variables::hit_statistics::
    CalculateMaxPulseTime(
        const I3Geometry&           geometry,
        const I3RecoPulseSeriesMap& pulsesMap
    );

**Python bindings**:

.. autofunction:: calculate_max_pulse_time

Utility function ``CalculateQMaxDoms``
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

**C++ Definition**::

    I3DoublePtr
    common_variables::hit_statistics::
    CalculateQMaxDoms(
        const I3Geometry&           geometry,
        const I3RecoPulseSeriesMap& pulsesMap
    );

**Python bindings**:

.. autofunction:: calculate_q_max_doms

Utility function ``CalculateQTotPulses``
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

**C++ Definition**::

    I3DoublePtr
    common_variables::hit_statistics::
    CalculateQTotPulses(
        const I3Geometry&           geometry,
        const I3RecoPulseSeriesMap& pulsesMap
    );

**Python bindings**:

.. autofunction:: calculate_q_tot_pulses

Utility function ``CalculateZMin``
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

**C++ Definition**::

    I3DoublePtr
    common_variables::hit_statistics::
    CalculateZMin(
        const I3Geometry&           geometry,
        const I3RecoPulseSeriesMap& pulsesMap
    );

**Python bindings**:

.. autofunction:: calculate_z_min

Utility function ``CalculateZMax``
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

**C++ Definition**::

    I3DoublePtr
    common_variables::hit_statistics::
    CalculateZMax(
        const I3Geometry&           geometry,
        const I3RecoPulseSeriesMap& pulsesMap
    );

**Python bindings**:

.. autofunction:: calculate_z_max

Utility function ``CalculateZMean``
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

**C++ Definition**::

    I3DoublePtr
    common_variables::hit_statistics::
    CalculateZMean(
        const I3Geometry&           geometry,
        const I3RecoPulseSeriesMap& pulsesMap
    );

**Python bindings**:

.. autofunction:: calculate_z_mean

Utility function ``CalculateZSigma``
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

**C++ Definition**::

    I3DoublePtr
    common_variables::hit_statistics::
    CalculateZSigma(
        const I3Geometry&           geometry,
        const I3RecoPulseSeriesMap& pulsesMap
    );

**Python bindings**:

.. autofunction:: calculate_z_sigma

Utility function ``CalculateZTravel``
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

**C++ Definition**::

    I3DoublePtr
    common_variables::hit_statistics::
    CalculateZTravel(
        const I3Geometry&           geometry,
        const I3RecoPulseSeriesMap& pulsesMap
    );

**Python bindings**:

.. autofunction:: calculate_z_travel

Examples
--------

This section should give some examples how to calculate the hit statistics
values for different use-cases.

Calculating hit statistics values using utility functions
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

To calculate the hit statistics values for a given I3RecoPulseSeriesMap using
the :py:func:`calculate_hit_statistics` utility function within your Python
script, you could do::

    from icecube.icetray import I3Units
    from icecube.common_variables import hit_statistics

    geometry   = frame['I3Geometry']
    pulses_map = frame['my_I3RecoPulseSeriesMap']

    hit_statistics_values = hit_statistics.calculate_hit_statistics(geometry, pulses_map)

    print "Calculation results:"
    print "COG         : %s"%(hit_statistics_values.cog)
    print "COGZSigma   : %.2f m"%(hit_statistics_values.cog_z_sigma/I3Units.m)
    print "MinPulseTime: %.2f ns"%(hit_statistics_values.min_pulse_time/I3Units.ns)
    print "MaxPulseTime: %.2f ns"%(hit_statistics_values.max_pulse_time/I3Units.ns)
    print "QMaxDoms    : %.2f PE"%(hit_statistics_values.q_max_doms)
    print "QTotPulses  : %.2f PE"%(hit_statistics_values.q_tot_pulses)
    print "ZMin        : %.2f m"%(hit_statistics_values.z_min/I3Units.m)
    print "ZMax        : %.2f m"%(hit_statistics_values.z_max/I3Units.m)
    print "ZMean       : %.2f m"%(hit_statistics_values.z_mean/I3Units.m)
    print "ZSigma      : %.2f m"%(hit_statistics_values.z_sigma/I3Units.m)
    print "ZTravel     : %.2f m"%(hit_statistics_values.z_travel/I3Units.m)

    # Put values into the frame.
    frame["MyHitStatisticsValues"] = hit_statistics_values

A full script example can be found in the file
``$I3_SRC/CommonVariables/resources/examples/hit_statistics/calculate_hit_statistics_values.py``.

Using the I3HitStatisticsCalculatorSegment traysegment
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Using the :func:`I3HitStatisticsCalculatorSegment` icetray traysegment to
calculate and to book hit statistics values is the preferred and easiest way of
doing it. A full script example can be found in the file
``$I3_SRC/CommonVariables/resources/examples/hit_statistics/I3HitStatisticsCalculatorSegment.py``.
The following code snippet illustrates the core points::

    ...
    from icecube import hdfwriter
    from icecube.common_variables import hit_statistics

    pulses_map_name = 'MaskedOfflinePulses'

    tableio_keys_to_book = []

    tableio_keys_to_book +=\
    tray.AddSegment(hit_statistics.I3HitStatisticsCalculatorSegment, 'hs',
        PulseSeriesMapName              = pulses_map_name,
        OutputI3HitStatisticsValuesName = 'HitStatisticsValues',
        BookIt                          = True
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
