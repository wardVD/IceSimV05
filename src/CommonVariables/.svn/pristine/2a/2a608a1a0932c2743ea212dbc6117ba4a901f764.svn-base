# $Id: segments.py 123227 2014-09-08 08:51:29Z mwolf $
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
#
"""The segments Python module of the hit multiplicity sub project of the
CommonVariables project contains all available icetray traysegments for
calculating and booking hit multiplicity values.

"""
from icecube import icetray, dataclasses, tableio

from icecube.common_variables.hit_multiplicity import I3HitMultiplicityCalculator
from icecube.recclasses.converters import I3HitMultiplicityValuesConverter

#_______________________________________________________________________________
@icetray.traysegment_inherit(I3HitMultiplicityCalculator)
def I3HitMultiplicityCalculatorSegment(tray, name,
    BookIt = False,
    **i3module_kwargs
):
    """This tray segment adds the
    :py:class:`icecube.common_variables.I3HitMultiplicityCalculator`
    icetray module to the tray. The traysegment takes the same arguments as the
    icetray module does, plus the following additional keyword arguments:

    :type  BookIt: bool
    :param BookIt: The switch if this traysegment should also generate and
        return the tableio converter keys for the calculated hit multiplicity
        values.

        The name of the output tableio table for the calculated multiplicity
        values will be the same as the name of the frame object holding the
        hit multiplicity values.

        Default value is ``False``.

    :return None: If the "BookIt" keyword argument has been set to ``False``.
    :return list: The list of tableio converter keys to book the hit
        multiplicity values, if the "BookIt" keyword argument has been set to
        ``True``.

    """
    # By default, run the calculator module only on physics frames.
    i3module_kwargs_ = dict(
        If = lambda frame: frame.Stop == icetray.I3Frame.Physics
    )
    i3module_kwargs_.update(i3module_kwargs)

    tray.AddModule(I3HitMultiplicityCalculator, "I3HitMultiplicityCalculator_"+name,
        **i3module_kwargs_
    )

    if BookIt:
        if(not 'OutputI3HitMultiplicityValuesName' in i3module_kwargs_.keys()):
            raise ValueError(
                'The BookIt keyword argument of this traysegment has been set '\
                'to ``True`` but the "OutputI3HitMultiplicityValuesName" '\
                'parameter of the '\
                'common_variables.hit_multiplicity.I3HitMultiplicityCalculator '\
                'icetray module has not been specified, which is needed in '\
                'this case!')

        tableio_keys =\
        tray.AddSegment(I3HitMultiplicityValuesBookerSegment, name,
            OutputI3HitMultiplicityValuesName = i3module_kwargs_['OutputI3HitMultiplicityValuesName']
        )
        return tableio_keys

    return None

#_______________________________________________________________________________
@icetray.traysegment
def I3HitMultiplicityValuesBookerSegment(tray, name,
    OutputI3HitMultiplicityValuesName
):
    """This traysegment generates and returns tableio converter keys to book the
    hit multiplicity calculation results from the frame.

    The name of the output tableio table for the calculated multiplicity
    values will be the same as the name of the frame object holding the
    hit multiplicity values, e.g. the name specified through the
    *OutputI3HitMultiplicityValuesName* parameter.

    :type  OutputI3HitMultiplicityValuesName: str
    :param OutputI3HitMultiplicityValuesName: The name of the
        :py:class:`icecube.common_variables.hit_multiplicity.I3HitMultiplicityValues`
        object inside the frame.

    :return list: The list of *tableio* converter keys.

    """
    tableio_keys = [
        dict(
            key       = OutputI3HitMultiplicityValuesName,
            converter = I3HitMultiplicityValuesConverter(),
            name      = OutputI3HitMultiplicityValuesName
        )
    ]

    return tableio_keys
