# $Id: __init__.py 125466 2014-11-05 23:17:19Z mwolf $
#
# Copyright (C) 2012
# Martin Wolf <martin.wolf@icecube.wisc.edu>
# and the IceCube Collaboration <http://www.icecube.wisc.edu>
#
# Version: $Revision$
# Date:    $Date: 2014-11-05 18:17:19 -0500 (Wed, 05 Nov 2014) $
#
# This module is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
#
# This module is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>

"""The *track_characteristics* sub project of CommonVariables provides
C++ utility functions (with pybindings), icetray modules, and icetray segments
to calculate and to book the following track characteristics cut variables:

- AvgDomDistQTotDom (formerly named "AvgDistQ")
- EmptyHitsTrackLength (formerly named "LEmpty")
- TrackHitsSeparationLength (formerly named "Separation")
- TrackHitsDistributionSmoothness

.. note::

    The TrackHitsDistributionSmoothness value ``[-1;+1]`` describes how
    uniformly the hits of a given I3RecoPulseSeriesMap are distributed along the
    track (given by its I3Particle object), where 0 means uniformly distributed.

    The I3Cuts implementation of this cut variable is wrong if there are more
    than only one pulse in a I3RecoPulseSeries, because it takes the hit
    distances along the track (which depends only on the DOM position) for every
    pulse of a DOM and not only once!

    The implementation of the TrackHitsDistributionSmoothness cut variable of
    this CommonVariables sub project takes the hit distance along the track only
    once for a DOM.

Variable Definitions
--------------------

This section lists the definitions of the variables calculated by the
``common_variables.track_characteristics`` module.

.. note::

    All variables consider only pulses which are within the configured cylinder
    radius around the configured track!

    So the mathematical variable :math:`NHitDoms` in this sub project denotes
    the number of hit DOMs within the configured cylinder radius around the
    configured track.

AvgDomDistQTotDom
^^^^^^^^^^^^^^^^^

The average DOM distance from the track weighted by the total charge of each
DOM. It is calculated using the following formula:

.. math::

    AvgDomDistQTotDom := \\frac{1}{\\sum_{i=1}^{NHitDoms} QTotDom_i} \\sum_{i=1}^{NHitDoms} dist(Dom_i,Track) \\cdot QTotDom_i

where :math:`QTotDom_i` is the total charge of the i'th DOM and
:math:`dist(Dom_i,Track)` is the closest approach distance of the i'th DOM to
the configured track.

EmptyHitsTrackLength
^^^^^^^^^^^^^^^^^^^^

The maximal track length of the track, which got no hits from hit DOMs within
the specified cylinder radius around the track.

.. note::

    This variable is only defined for :math:`NHitDoms \\geq 2`. Otherwise this
    variable is set to ``NAN``.

The algorithm to obtain this variable can be described as follows:

First a list of the projected position of the hit DOMs is created.

.. math::

    domDistList := \\left\\{ distAlongTrack(Dom_i, Track) \\right\\}

where :math:`distAlongTrack(Dom_i, Track)` is the projected position of the
i'th DOM on the line of the configured track, where :math:`i` goes from
:math:`1` to :math:`NHitDoms`.

Then this distance list is sorted in ascending order:

.. math::

    ascDomDistList := ascendingSort(domDistList)

The EmptyHitsTrackLength variable is then simply the maximal difference of
successive distance elements of the sorted distance list:

.. math::

    EmptyHitsTrackLength := max( ascDomDistList[i+1] - ascDomDistList[i] )

where :math:`i` goes from :math:`1` to :math:`NHitDoms-1`.

"""
IS_LOADED = True
from icecube.common_variables.load_pybindings_enhanced import load_pybindings
load_pybindings(__name__, __path__)
del(load_pybindings)

# Import the track_characteristics icetray modules.
from I3TrackCharacteristicsCalculator import I3TrackCharacteristicsCalculator

# Import the track_characteristics traysegments.
from segments import I3TrackCharacteristicsCalculatorSegment
from segments import I3TrackCharacteristicsValuesBookerSegment
