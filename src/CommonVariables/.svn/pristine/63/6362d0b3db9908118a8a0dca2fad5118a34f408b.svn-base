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

"""The segments Python module of the hit statistics sub project of the
CommonVariables project contains all available icetray traysegments for
calculating and booking hit statistics values.

"""
from icecube import icetray, dataclasses, tableio

from icecube.common_variables.hit_statistics import I3HitStatisticsCalculator
from icecube.recclasses.converters import I3HitStatisticsValuesConverter

#_______________________________________________________________________________
@icetray.traysegment_inherit(I3HitStatisticsCalculator)
def I3HitStatisticsCalculatorSegment(tray, name,
    BookIt          = False,
    COGBookRefFrame = dataclasses.converters.I3PositionConverter.BookRefFrame.Car,
    **i3module_kwargs
):
    """This tray segment adds the
    :py:class:`icecube.common_variables.I3HitStatisticsCalculator`
    icetray module to the tray. The traysegment takes the same arguments as the
    icetray module does, plus the following additional keyword arguments:

    :type  BookIt: bool
    :param BookIt: The switch if this traysegment should also generate and
        return the tableio converter keys for the calculated hit statistics
        values.

        The name of the output tableio table for the calculated statistics
        values will be the same as the name of the frame object holding the
        hit statistics values.

        Default value is ``False``.

    :type  COGBookRefFrame: enum ``dataclasses.converters.I3PositionConverter.BookRefFrame``
    :param COGBookRefFrame: The specifier of the reference frame into which the
        COG position should be booked.

        Default value is
        ``dataclasses.converters.I3PositionConverter.BookRefFrame.Car``.

    :return None: If the "BookIt" keyword argument has been set to ``False``.
    :return list: The list of tableio converter keys to book the hit
        statistics values, if the "BookIt" keyword argument has been set to
        ``True``.

    """
    # By default, run the calculator module only on physics frames.
    i3module_kwargs_ = dict(
        If = lambda frame: frame.Stop == icetray.I3Frame.Physics
    )
    i3module_kwargs_.update(i3module_kwargs)

    tray.AddModule(I3HitStatisticsCalculator, "I3HitStatisticsCalculator_"+name,
        **i3module_kwargs_
    )

    if BookIt:
        if(not 'OutputI3HitStatisticsValuesName' in i3module_kwargs_.keys()):
            raise ValueError(
                'The BookIt keyword argument of this traysegment has been set '\
                'to ``True`` but the "OutputI3HitStatisticsValuesName" '\
                'parameter of the '\
                'common_variables.hit_statistics.I3HitStatisticsCalculator '\
                'icetray module has not been specified, which is needed in '\
                'this case!')

        tableio_keys =\
        tray.AddSegment(I3HitStatisticsValuesBookerSegment, name,
            OutputI3HitStatisticsValuesName = i3module_kwargs_['OutputI3HitStatisticsValuesName'],
            COGBookRefFrame                 = COGBookRefFrame
        )
        return tableio_keys

    return None

#_______________________________________________________________________________
@icetray.traysegment
def I3HitStatisticsValuesBookerSegment(tray, name,
    OutputI3HitStatisticsValuesName,
    COGBookRefFrame = dataclasses.converters.I3PositionConverter.BookRefFrame.Car
):
    """This traysegment generates and returns tableio converter keys to book the
    hit statistics calculation results from the frame.

    The name of the output tableio table for the calculated statistics
    values will be the same as the name of the frame object holding the
    hit statistics values, e.g. the name specified through the
    *OutputI3HitStatisticsValuesName* parameter.

    :type  OutputI3HitStatisticsValuesName: str
    :param OutputI3HitStatisticsValuesName: The name of the
        :py:class:`icecube.common_variables.hit_statistics.I3HitStatisticsValues`
        object inside the frame.

    :type  COGBookRefFrame: enum ``dataclasses.converters.I3PositionConverter.BookRefFrame``
    :param COGBookRefFrame: The specifier of the reference frame in which the
        COG position values should be booked.

        Default value is
        ``dataclasses.converters.I3PositionConverter.BookRefFrame.Car``.

    :return list: The list of *tableio* converter keys.

    """
    tableio_keys = [
        dict(
            key       = OutputI3HitStatisticsValuesName,
            converter = I3HitStatisticsValuesConverter(
                COGBookRefFrame = COGBookRefFrame
            ),
            name      = OutputI3HitStatisticsValuesName
        )
    ]

    return tableio_keys
