# $Id: __init__.py 123227 2014-09-08 08:51:29Z mwolf $
#
# Copyright (C) 2012
# Martin Wolf <martin.wolf@icecube.wisc.edu>
# and the IceCube Collaboration <http://www.icecube.wisc.edu>
#
# Version: $Revision: 123227 $
# Date:    $Date: 2014-09-08 04:51:29 -0400 (Mon, 08 Sep 2014) $
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

"""The *direct_hits* sub project of CommonVariables provides C++ utility
functions (with pybindings), icetray modules, tableio converters, and icetray
traysegments to calculate and to book the following direct hits cut variables:

- NDirStrings
- NDirDoms
- NDirPulses
- QDirPulses
- NEarlyStrings
- NEarlyDoms
- NEarlyPulses
- QEarlyPulses
- NLateStrings
- NLateDoms
- NLatePulses
- QLatePulses
- DirTrackLength
- DirTrackHitDistributionSmoothness

It is designed to enable the user to calculate direct hits for different direct
hits definitions, e.g. different direct hits time windows. The definition of one
particular class of direct hits is described by the
:py:class:`icecube.common_variables.direct_hits.I3DirectHitsDefinition` class.

"""
from icecube.common_variables.load_pybindings_enhanced import load_pybindings
IS_LOADED = True
#try:
#    load_pybindings(__name__, __path__)
#except:
#    IS_LOADED = False
load_pybindings(__name__, __path__)
del(load_pybindings)

#if(IS_LOADED):
# Import icetray modules so icetray-inspect can inspect them.
from I3DirectHitsCalculator import I3DirectHitsCalculator

# Define an attribute with the default direct hits definitions.
default_definitions = get_default_definitions()

# Import direct_hits tray segments
from segments import I3DirectHitsCalculatorSegment
from segments import I3DirectHitsValuesBookerSegment
