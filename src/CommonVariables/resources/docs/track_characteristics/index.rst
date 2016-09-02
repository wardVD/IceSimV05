.. _CommonVariables__track_characteristics:

Track Characteristics
=====================

.. automodule:: icecube.common_variables.track_characteristics

The I3TrackCharacteristicsValues class
--------------------------------------

.. autoclass:: I3TrackCharacteristicsValues

   **Class Properties**

   .. autoattribute:: avg_dom_dist_q_tot_dom
   .. autoattribute:: empty_hits_track_length
   .. autoattribute:: track_hits_distribution_smoothness
   .. autoattribute:: track_hits_separation_length

The I3TrackCharacteristicsCalculator icetray module
---------------------------------------------------

.. autoclass:: I3TrackCharacteristicsCalculator

    **Module Parameters**:

    .. autoattribute:: PulseSeriesMapName
    .. autoattribute:: ParticleName
    .. autoattribute:: OutputI3TrackCharacteristicsValuesName
    .. autoattribute:: TrackCylinderRadius
    .. autoattribute:: PyLogLevel

Available traysegments
----------------------

.. autofunction:: I3TrackCharacteristicsCalculatorSegment

.. autofunction:: I3TrackCharacteristicsValuesBookerSegment

Utility functions
-----------------

This section lists the utility functions, which are defined to calculate the
track characteristics cut variables.

Utility function ``CalculateTrackCharacteristics``
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

**C++ Definition**::

    I3TrackCharacteristicsValuesPtr
    common_variables::track_characteristics::
    CalculateTrackCharacteristics(
        const I3Geometry&           geometry,
        const I3RecoPulseSeriesMap& pulsesMap,
        const I3Particle&           particle,
        double                      trackCylinderRadius
    );

**Python bindings**:

.. autofunction:: calculate_track_characteristics

Examples
--------

This section should give some examples how to calculate the track
characteristics for different use-cases.

Calculating the track characteristics within Python
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

To calculate the track characteristics for a given I3RecoPulseSeriesMap and a
given I3Particle using the :py:func:`calculate_track_characteristics` utility
function within your Python script, you could do::

    from icecube.icetray import I3Units
    from icecube.common_variables import track_characteristics

    geometry        = frame['I3Geometry']
    pulses_map      = frame['my_I3RecoPulseSeriesMap']
    particle        = frame['my_I3Particle']
    cylinder_radius = 150*I3Units.m

    track_characteristics_values = track_characteristics.calculate_track_characteristics(
        geometry,
        pulses_map,
        particle,
        cylinder_radius
    )

    print "Calculation results: %s"%(track_characteristics_values)

A full script example can be found in the file
``$I3_SRC/CommonVariables/resources/examples/track_characteristics/calculate_track_characteristics.py``.

Using the I3TrackCharacteristicsCalculatorSegment traysegment
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Using the :func:`I3TrackCharacteristicsCalculatorSegment` icetray traysegment to
calculate and to book track characteristics cut variables is the preferred and
easiest way of doing it. A full script example can be found in the file
``$I3_SRC/CommonVariables/resources/examples/track_characteristics/I3TrackCharacteristicsCalculatorSegment.py``.
The following code snippet illustrates the core points::

    ...
    from icecube.icetray import I3Units
    from icecube import hdfwriter
    from icecube.common_variables import track_characteristics

    pulses_map_name    = 'MaskedOfflinePulses'
    reco_particle_name = 'MPEFit_SLC'

    tableio_keys_to_book = []

    tableio_keys_to_book +=\
    tray.AddSegment(track_characteristics.I3TrackCharacteristicsCalculatorSegment, 'tc',
        PulseSeriesMapName                     = pulses_map_name,
        ParticleName                           = reco_particle_name,
        OutputI3TrackCharacteristicsValuesName = reco_particle_name+'Characteristics',
        TrackCylinderRadius                    = 150.*I3Units.m,
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
