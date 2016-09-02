# Anna Pollmann <anna.pollmann@uni-wuppertal.de>

"""The *time_characteristics* sub project of CommonVariables provides
C++ utility functions (with pybindings), icetray modules, and icetray segments
to calculate and to book the following track characteristics cut variables:

- TimeLengthFWHM
- TimeLengthLastFirst
- TimeLengthMaxGap
- ZPattern

Variable Definitions
--------------------

This section lists the definitions of the variables calculated by the
``common_variables.time_characteristics`` module.

TimeLengthFWHM
^^^^^^^^^^^^^^^^^

Considered are only the first pulses per DOM. These pulses are ordered in time.
This variable gives the full width at half maximum of all those pulses.

TimeLengthLastFirst
^^^^^^^^^^^^^^^^^

Considered are only the first pulses per DOM. These pulses are ordered in time.
This variable gives the difference between the last minus the first pulse in this order.

TimeLengthMaxGap
^^^^^^^^^^^^^^^^^

Considered are only the first pulses per DOM. These pulses are ordered in time.
This variable gives the maximum gap in time between those pulses.

ZPattern
^^^^^^^^^^^^^^^^^

Considered are only the first pulses per DOM. These pulses are ordered in time.
It is looped over those pulses. If a pulse is at a DOM with higher z-position than its
preceeding pulses' DOM, zpattern is enhanced by 1.
If the DOM is lower in the detector, zpattern is decreased by 1. If the DOM is at
the same height, nothing is done.




"""
IS_LOADED = True
from icecube.common_variables.load_pybindings_enhanced import load_pybindings
load_pybindings(__name__, __path__)
del(load_pybindings)

# Import the track_characteristics icetray modules.
from I3TimeCharacteristicsCalculator import I3TimeCharacteristicsCalculator

# Import the track_characteristics traysegments.
from segments import I3TimeCharacteristicsCalculatorSegment
from segments import I3TimeCharacteristicsValuesBookerSegment
