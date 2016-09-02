# $Id: __init__.py 123227 2014-09-08 08:51:29Z mwolf $
#
# Copyright (C) 2012
# Martin Wolf <martin.wolf@icecube.wisc.edu>
# and the IceCube Collaboration <http://www.icecube.wisc.edu>
#
# version: $Revision: 123227 $
# date:    $Date: 2014-09-08 04:51:29 -0400 (Mon, 08 Sep 2014) $
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
"""This python module contains everything of the CommonVariables project.

"""
import icecube.recclasses

from load_pybindings_enhanced import load_pybindings
load_pybindings(__name__, __path__)
del(load_pybindings)

import logging
logging.basicConfig()

# Import all the static python sub modules of CommonVariables.
import direct_hits
import hit_multiplicity
import hit_statistics
import track_characteristics
import time_characteristics
