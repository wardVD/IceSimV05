.. _CommonVariables__hit_multiplicity:

Hit Multiplicity
================

.. automodule:: icecube.common_variables.hit_multiplicity

The I3HitMultiplicityValues class
---------------------------------

.. autoclass:: I3HitMultiplicityValues

   **Class Properties**

   .. autoattribute:: n_hit_strings
   .. autoattribute:: n_hit_doms
   .. autoattribute:: n_hit_doms_one_pulse
   .. autoattribute:: n_pulses

The I3HitMultiplicityCalculator icetray module
----------------------------------------------

.. autoclass:: I3HitMultiplicityCalculator

    **Module Parameters**:

    .. autoattribute:: PulseSeriesMapName
    .. autoattribute:: OutputI3HitMultiplicityValuesName
    .. autoattribute:: PyLogLevel

tableio converters
------------------

This section lists the existing *tableio* converters for the hit_multiplicity
sub project.

The I3HitMultiplicityValuesConverter
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. autoclass:: I3HitMultiplicityValuesConverter

    .. automethod:: __init__

Available traysegments
----------------------

.. autofunction:: I3HitMultiplicityCalculatorSegment

.. autofunction:: I3HitMultiplicityValuesBookerSegment

Utility functions
-----------------

This section lists the utility functions, which are defined to calculate the
hit multiplicity values.

Utility function ``CalculateHitMultiplicity``
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

**C++ Definition**::

    I3HitMultiplicityValuesPtr
    common_variables::hit_multiplicity::
    CalculateHitMultiplicity(
        const I3Geometry&           geometry,
        const I3RecoPulseSeriesMap& pulsesMap
    );

Calculates the hit multiplicity, e.g. NHitStrings, NHitDoms, NHitDomsOnePulse,
and NPulses, for the given I3Geometry, and the given I3RecoPulseSeriesMap.

**Python bindings**:

.. autofunction:: calculate_hit_multiplicity

Examples
--------

This section should give some examples how to calculate the hit multiplicity
values for different use-cases.

Calculating hit multiplicity values using utility functions
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

To calculate the hit multiplicity values for a given I3RecoPulseSeriesMap using
the
:py:func:`calculate_hit_multiplicity` utility function within your Python
script, you could do::

    from icecube.common_variables import hit_multiplicity

    geometry   = frame['I3Geometry']
    pulses_map = frame['my_I3RecoPulseSeriesMap']

    hit_multiplicity_values = hit_multiplicity.calculate_hit_multiplicity(geometry, pulses_map)

    print "Calculation results:"
    print "NHitStrings     : %d"%(hit_multiplicity_values.n_hit_strings)
    print "NHitDoms        : %d"%(hit_multiplicity_values.n_hit_doms)
    print "NHitDomsOnePulse: %d"%(hit_multiplicity_values.n_hit_doms_one_pulse)
    print "NPulses         : %d"%(hit_multiplicity_values.n_pulses)

    # Put values into the frame.
    frame["MyHitMultiplicityValues"] = hit_multiplicity_values

A full script example can be found in the file
``$I3_SRC/CommonVariables/resources/examples/hit_multiplicity/calculate_hit_multiplicity_values.py``.

Using the I3HitMultiplicityCalculatorSegment traysegment
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Using the :py:func:`I3HitMultiplicityCalculatorSegment` icetray traysegment to
calculate and to book hit multiplicity values is the preferred and easiest way
of doing it. A full script example can be found in the file
``$I3_SRC/CommonVariables/resources/examples/hit_multiplicity/I3HitMultiplicityCalculatorSegment.py``.
The following code snippet illustrates the core points::

    ...
    from icecube import hdfwriter
    from icecube.common_variables import hit_multiplicity

    pulses_map_name = 'MaskedOfflinePulses'

    tableio_keys_to_book = []

    tableio_keys_to_book +=\
    tray.AddSegment(hit_multiplicity.I3HitMultiplicityCalculatorSegment, 'hm',
        PulseSeriesMapName                = pulses_map_name,
        OutputI3HitMultiplicityValuesName = 'HitMultiplicityValues',
        BookIt                            = True
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
