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

"""The hit statistics sub project of CommonVariables provides C++ utility
functions (with pybindings), icetray modules, tableio converters, and icetray
traysegments to calculate and to book the following hit statistics variables:

- COG
- COGZSigma
- MinPulseTime
- MaxPulseTime
- QMaxDoms
- QTotPulses
- ZMin
- ZMax
- ZMean
- ZSigma
- ZTravel

For an explaination of these variables see the documentation of the
:py:class:`icecube.recclasses.I3HitStatisticsValues`
class properties.

"""
IS_LOADED = True
from icecube.common_variables.load_pybindings_enhanced import load_pybindings
load_pybindings(__name__, __path__)
del(load_pybindings)

# Import icetray modules so icetray-inspect can inspect them.
from I3HitStatisticsCalculator import I3HitStatisticsCalculator

# Import hit_statistics icetray traysegments so icetray-inspect can inspect
# them.
from segments import I3HitStatisticsCalculatorSegment
from segments import I3HitStatisticsValuesBookerSegment
