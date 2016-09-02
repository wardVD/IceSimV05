.. _CommonVariables__time_characteristics:

Time Characteristics
=====================

.. automodule:: icecube.common_variables.time_characteristics

The I3TimeCharacteristicsValues class
--------------------------------------

.. autoclass:: I3TimeCharacteristicsValues

   **Class Properties**

   .. autoattribute:: timelength_fwhm
   .. autoattribute:: timelength_last_first
   .. autoattribute:: timelength_maxgap
   .. autoattribute:: zpattern


The I3TimeCharacteristicsCalculator icetray module
---------------------------------------------------

.. autoclass:: I3TimeCharacteristicsCalculator

    **Module Parameters**:

    .. autoattribute:: PulseSeriesMapName
    .. autoattribute:: OutputI3TimeCharacteristicsValuesName
    .. autoattribute:: PyLogLevel

Available traysegments
----------------------

.. autofunction:: I3TimeCharacteristicsCalculatorSegment

.. autofunction:: I3TimeCharacteristicsValuesBookerSegment

Utility functions
-----------------

This section lists the utility functions, which are defined to calculate the
time characteristics cut variables.

Utility function ``CalculateTimeCharacteristics``
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

**C++ Definition**::

    I3TimeCharacteristicsValuesPtr
    common_variables::time_characteristics::
    CalculateTimeCharacteristics(
        const I3Geometry&           geometry,
        const I3RecoPulseSeriesMap& pulsesMap,
    );

**Python bindings**:

.. autofunction:: calculate_time_characteristics

Examples
--------

This section should give some examples how to calculate the time
characteristics for different use-cases.

Calculating the time characteristics within Python
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

To calculate the time characteristics for a given I3RecoPulseSeriesMap using the
:py:func:`calculate_time_characteristics` utility
function within your Python script, you could do::

    from icecube.icetray import I3Units
    from icecube.common_variables import time_characteristics

    geometry        = frame['I3Geometry']
    pulses_map      = frame['my_I3RecoPulseSeriesMap']

    time_characteristics_values = time_characteristics.calculate_time_characteristics(
        geometry,
        pulses_map,
    )

    print "Calculation results: %s"%(time_characteristics_values)

A full script example can be found in the file
``$I3_SRC/CommonVariables/resources/examples/time_characteristics/calculate_time_characteristics.py``.

Using the I3TimeCharacteristicsCalculatorSegment traysegment
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Using the :func:`I3TimeCharacteristicsCalculatorSegment` icetray traysegment to
calculate and to book time characteristics cut variables is the preferred and
easiest way of doing it. A full script example can be found in the file
``$I3_SRC/CommonVariables/resources/examples/time_characteristics/I3TimeCharacteristicsCalculatorSegment.py``.
The following code snippet illustrates the core points::

    ...
    from icecube.icetray import I3Units
    from icecube import hdfwriter
    from icecube.common_variables import time_characteristics

    pulses_map_name    = 'MaskedOfflinePulses'
    reco_particle_name = 'MPEFit_SLC'

    tableio_keys_to_book = []

    tableio_keys_to_book +=\
    tray.AddSegment(time_characteristics.I3TimeCharacteristicsCalculatorSegment, 'tc',
        PulseSeriesMapName                     = pulses_map_name,
        OutputI3TimeCharacteristicsValuesName = reco_particle_name+'Characteristics',
        BookIt                                 = True
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
